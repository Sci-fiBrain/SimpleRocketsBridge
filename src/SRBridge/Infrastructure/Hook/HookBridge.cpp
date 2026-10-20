#include "Infrastructure/Hook/HookBridge.h"

#include <MinHook.h>

#include <format>
#include <string>

#include "SimpleRockets/SDK/Game/Offsets.h"
#include "SimpleRockets/SDK/Log/ILogger.h"

namespace SimpleRockets::Bridge::Infrastructure::Hook
{
    namespace
    {
        using HandleUpdateFn = void(__thiscall*)(void* self, float deltaSeconds);

        /// <summary>原函数入口, 由 MinHook 在创建 hook 时填入</summary>
        HandleUpdateFn handleUpdateOriginal = nullptr;

        /// <summary>由 Install 注入, detour 中写日志用</summary>
        SDK::Log::ILogger* logger = nullptr;

        /// <summary>上一帧看到的 Runtime 地址, 用于只在对象变化时记录</summary>
        void* lastRuntime = nullptr;

        /// <summary>是否已完成 MH_Initialize</summary>
        Boolean initialized = false;

        void LogRuntimeChanged(void* runtime)
        {
            const std::string message = std::format("Runtime 指针: 0x{:08X}", reinterpret_cast<UInt32>(runtime));
            logger->Log(SDK::Log::LogSeverity::Info, message);
        }

        /// <summary>Runtime::HandleUpdate 的 detour, __thiscall 的 this 落在 ECX, 与 __fastcall 首参位置一致</summary>
        void __fastcall DetourHandleUpdate(void* self, void*, float deltaSeconds)
        {
            handleUpdateOriginal(self, deltaSeconds);

            if (self != lastRuntime)
            {
                lastRuntime = self;
                LogRuntimeChanged(self);
            }
        }
    }

    Boolean Install(void* moduleBase, SDK::Log::ILogger* logTarget)
    {
        if (moduleBase == nullptr || logTarget == nullptr)
        {
            return false;
        }

        if (MH_Initialize() != MH_OK)
        {
            return false;
        }
        initialized = true;

        logger = logTarget;

        Byte* target = static_cast<Byte*>(moduleBase) + SDK::Game::RvaHookHandleUpdate;

        if (MH_CreateHook(target, reinterpret_cast<void*>(&DetourHandleUpdate),
                reinterpret_cast<void**>(&handleUpdateOriginal)) != MH_OK)
        {
            Uninstall();
            return false;
        }

        if (MH_EnableHook(target) != MH_OK)
        {
            Uninstall();
            return false;
        }

        return true;
    }

    void Uninstall()
    {
        MH_DisableHook(MH_ALL_HOOKS);
        MH_RemoveHook(MH_ALL_HOOKS);

        if (initialized)
        {
            MH_Uninitialize();
            initialized = false;
        }

        handleUpdateOriginal = nullptr;
        logger = nullptr;
        lastRuntime = nullptr;
    }
}
