#include "Infrastructure/Hook/HookBridge.h"

#include <MinHook.h>

#include "Infrastructure/Hook/Detours.h"
#include "SimpleRockets/SDK/Environment.h"

namespace SimpleRockets::Bridge::Infrastructure::Hook
{
    namespace
    {
        /// <summary>是否已完成 MH_Initialize</summary>
        Boolean initialized = false;
    }

    Boolean Install()
    {
        if (MH_Initialize() != MH_OK)
        {
            return false;
        }
        initialized = true;

        for (const Detours::HookEntry& entry : Detours::GetEntries())
        {
            void* target = SDK::Environment::GetAddress<void*>(entry.rva);
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
