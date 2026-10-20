#include "Infrastructure/Hook/Detours.h"

#include <format>
#include <string>

#include "SimpleRockets/SDK/Game/Memory.h"
#include "SimpleRockets/SDK/Game/Offsets.h"

namespace SimpleRockets::Bridge::Infrastructure::Hook::Detours
{
    InitializeFn initializeOriginal = nullptr;
    HandleUpdateFn handleUpdateOriginal = nullptr;
    RuntimeDestructorFn runtimeDestructorOriginal = nullptr;
    SaveXmlFn saveXmlOriginal = nullptr;
    ActivateStageFn activateStageOriginal = nullptr;

    namespace
    {
        /// <summary>由 SetLogger 注入</summary>
        SDK::Log::ILogger* logger = nullptr;

        /// <summary>由 SetRuntimeHooks 注入</summary>
        std::span<SDK::Mod::RuntimeHookBase* const> runtimeHooks;

        void LogEvent(const char* name, void* object)
        {
            const std::string message = std::format("{}: 0x{:08X}", name, reinterpret_cast<UInt32>(object));
            logger->Log(SDK::Log::LogSeverity::Info, message);
        }
    }

    void SetLogger(SDK::Log::ILogger* target)
    {
        logger = target;
    }

    void SetRuntimeHooks(std::span<SDK::Mod::RuntimeHookBase* const> hooks)
    {
        runtimeHooks = hooks;
    }

    void Reset()
    {
        initializeOriginal = nullptr;
        handleUpdateOriginal = nullptr;
        runtimeDestructorOriginal = nullptr;
        saveXmlOriginal = nullptr;
        activateStageOriginal = nullptr;
        logger = nullptr;
        runtimeHooks = {};
    }

    void __fastcall DetourInitialize(void* self, UnusedEdx, void* parameters)
    {
        initializeOriginal(self, parameters);
        LogEvent("OnInitialized", self);

        SDK::Game::RuntimeView view{self};
        for (SDK::Mod::RuntimeHookBase* hook : runtimeHooks)
        {
            hook->OnInitialized(view);
        }
    }

    void __fastcall DetourHandleUpdate(void* self, UnusedEdx, float deltaSeconds)
    {
        SDK::Game::RuntimeView view{self};
        for (SDK::Mod::RuntimeHookBase* hook : runtimeHooks)
        {
            hook->OnUpdating(view);
        }

        handleUpdateOriginal(self, deltaSeconds);

        for (SDK::Mod::RuntimeHookBase* hook : runtimeHooks)
        {
            hook->OnUpdated(view);
        }
    }

    void __fastcall DetourRuntimeDestructor(void* self, UnusedEdx)
    {
        LogEvent("OnDestroying", self);

        SDK::Game::RuntimeView view{self};
        for (SDK::Mod::RuntimeHookBase* hook : runtimeHooks)
        {
            hook->OnDestroying(view);
        }

        runtimeDestructorOriginal(self);
    }

    void __fastcall DetourSaveXml(void* self, UnusedEdx, void* document)
    {
        saveXmlOriginal(self, document);
        LogEvent("OnSaved", self);

        SDK::Game::RuntimeView view{self};
        for (SDK::Mod::RuntimeHookBase* hook : runtimeHooks)
        {
            hook->OnSaved(view);
        }
    }

    void __fastcall DetourActivateStage(void* self, UnusedEdx)
    {
        activateStageOriginal(self);
        LogEvent("OnStageActivated", self);

        const Int32 stageIndex = SDK::Game::Read<Int32>(self, SDK::Game::OffsetPodStageIndex);
        SDK::Game::PodView view{self};
        for (SDK::Mod::RuntimeHookBase* hook : runtimeHooks)
        {
            hook->OnStageActivated(view, stageIndex);
        }
    }

    std::span<const HookEntry> GetEntries()
    {
        static const HookEntry entries[] = {
            {SDK::Game::RvaHookRuntimeDestructor, reinterpret_cast<void*>(&DetourRuntimeDestructor),
                reinterpret_cast<void**>(&runtimeDestructorOriginal)},
            {SDK::Game::RvaHookInitialize, reinterpret_cast<void*>(&DetourInitialize),
                reinterpret_cast<void**>(&initializeOriginal)},
            {SDK::Game::RvaHookHandleUpdate, reinterpret_cast<void*>(&DetourHandleUpdate),
                reinterpret_cast<void**>(&handleUpdateOriginal)},
            {SDK::Game::RvaHookSaveXml, reinterpret_cast<void*>(&DetourSaveXml),
                reinterpret_cast<void**>(&saveXmlOriginal)},
            {SDK::Game::RvaHookActivateStage, reinterpret_cast<void*>(&DetourActivateStage),
                reinterpret_cast<void**>(&activateStageOriginal)},
        };

        return entries;
    }
}
