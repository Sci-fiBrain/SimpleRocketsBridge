#include "Infrastructure/Hook/HookBridge.h"

#include <MinHook.h>

#include "Infrastructure/Hook/Detours.h"

namespace SimpleRockets::Bridge::Infrastructure::Hook
{
    namespace
    {
        /// <summary>是否已完成 MH_Initialize</summary>
        Boolean initialized = false;
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

        Detours::SetLogger(logTarget);

        for (const Detours::HookEntry& entry : Detours::GetEntries())
        {
            void* target = static_cast<Byte*>(moduleBase) + entry.rva;
            if (MH_CreateHook(target, entry.detour, entry.original) != MH_OK)
            {
                Uninstall();
                return false;
            }
            if (MH_EnableHook(target) != MH_OK)
            {
                Uninstall();
                return false;
            }
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

        Detours::Reset();
    }
}
