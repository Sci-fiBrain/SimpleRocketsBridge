#include "Infrastructure/Hook/Detours.h"

#include "SimpleRockets/SDK/Game/Memory.h"
#include "SimpleRockets/SDK/Game/Layout/Rvas.h"
#include "SimpleRockets/SDK/Log/Logging.h"

namespace SimpleRockets::Bridge::Infrastructure::Hook::Detours
{
    InitializeFn initializeOriginal = nullptr;
    HandleUpdateFn handleUpdateOriginal = nullptr;
    RuntimeDestructorFn runtimeDestructorOriginal = nullptr;
    SaveXmlFn saveXmlOriginal = nullptr;
    ActivateNextStageFn activateNextStageOriginal = nullptr;
    LayerLoadedFn layerLoadedOriginal = nullptr;

    namespace
    {
        /// <summary>由 SetLogger 注入</summary>
        SDK::Log::ILogger* logger = nullptr;

        /// <summary>由 SetRuntimeHooks 注入</summary>
        std::span<SDK::Mod::RuntimeHookBase* const> runtimeHooks;

        void LogEvent(const char* name, void* object)
        {
            SDK::Log::LogFormat(logger, SDK::Log::LogSeverity::Info, "{}: 0x{:08X}", name,
                reinterpret_cast<UInt32>(object));
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
        activateNextStageOriginal = nullptr;
        layerLoadedOriginal = nullptr;
        logger = nullptr;
        runtimeHooks = {};
    }

    void __fastcall DetourInitialize(void* self, UnusedEdx, void* parameters)
    {
        initializeOriginal(self, parameters);
        LogEvent("OnInitialized", self);

        SDK::Game::RuntimeRef ref{self};
        for (SDK::Mod::RuntimeHookBase* hook : runtimeHooks)
        {
            hook->OnInitialized(ref, SDK::EventArgs{});
        }
    }

    void __fastcall DetourHandleUpdate(void* self, UnusedEdx, float deltaSeconds)
    {
        SDK::Game::RuntimeRef ref{self};
        for (SDK::Mod::RuntimeHookBase* hook : runtimeHooks)
        {
            hook->OnUpdating(ref, SDK::EventArgs{});
        }

        handleUpdateOriginal(self, deltaSeconds);

        for (SDK::Mod::RuntimeHookBase* hook : runtimeHooks)
        {
            hook->OnUpdated(ref, SDK::EventArgs{});
        }
    }

    void __fastcall DetourRuntimeDestructor(void* self, UnusedEdx)
    {
        LogEvent("OnDisposing", self);

        SDK::Game::RuntimeRef ref{self};
        for (SDK::Mod::RuntimeHookBase* hook : runtimeHooks)
        {
            hook->OnDisposing(ref, SDK::EventArgs{});
        }

        runtimeDestructorOriginal(self);
    }

    void __fastcall DetourSaveXml(void* self, UnusedEdx, void* document)
    {
        saveXmlOriginal(self, document);
        LogEvent("OnSaved", self);

        SDK::Game::RuntimeRef ref{self};
        for (SDK::Mod::RuntimeHookBase* hook : runtimeHooks)
        {
            hook->OnSaved(ref, SDK::EventArgs{});
        }
    }

    void __fastcall DetourActivateNextStage(void* self, UnusedEdx)
    {
        activateNextStageOriginal(self);
        LogEvent("OnNextStageActivated", self);

        SDK::Game::PodObjectRef ref{self};
        for (SDK::Mod::RuntimeHookBase* hook : runtimeHooks)
        {
            hook->OnNextStageActivated(ref, SDK::EventArgs{});
        }
    }

    void* __fastcall DetourLayerLoaded(void* self, UnusedEdx, void* argument1, void* argument2, void* argument3,
        void* argument4, void* argument5, void* argument6)
    {
        void* result = layerLoadedOriginal(self, argument1, argument2, argument3, argument4, argument5, argument6);
        LogEvent("OnLoaded", result);

        SDK::Game::RuntimeLayerRef ref{result};
        for (SDK::Mod::RuntimeHookBase* hook : runtimeHooks)
        {
            hook->OnLoaded(ref, SDK::EventArgs{});
        }

        return result;
    }

    std::span<const HookEntry> GetEntries()
    {
        static const HookEntry entries[] = {
            {SDK::Game::Layout::RvaHookRuntimeDestructor, reinterpret_cast<void*>(&DetourRuntimeDestructor),
                reinterpret_cast<void**>(&runtimeDestructorOriginal)},
            {SDK::Game::Layout::RvaHookInitialize, reinterpret_cast<void*>(&DetourInitialize),
                reinterpret_cast<void**>(&initializeOriginal)},
            {SDK::Game::Layout::RvaHookHandleUpdate, reinterpret_cast<void*>(&DetourHandleUpdate),
                reinterpret_cast<void**>(&handleUpdateOriginal)},
            {SDK::Game::Layout::RvaHookSaveXml, reinterpret_cast<void*>(&DetourSaveXml),
                reinterpret_cast<void**>(&saveXmlOriginal)},
            {SDK::Game::Layout::RvaHookActivateNextStage, reinterpret_cast<void*>(&DetourActivateNextStage),
                reinterpret_cast<void**>(&activateNextStageOriginal)},
            {SDK::Game::Layout::RvaHookLayerLoaded, reinterpret_cast<void*>(&DetourLayerLoaded),
                reinterpret_cast<void**>(&layerLoadedOriginal)},
        };

        return entries;
    }
}
