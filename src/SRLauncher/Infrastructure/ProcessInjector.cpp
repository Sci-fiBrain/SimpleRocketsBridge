#include "Infrastructure/ProcessInjector.h"

#include <TlHelp32.h>

#include <string>

namespace SimpleRockets::Launcher::Infrastructure
{
    namespace
    {
        /// <summary>把本地模块中的函数地址换算为目标进程中的地址</summary>
        [[nodiscard]] UInt32 RemoteFunctionAddress(
            const wchar_t* moduleName, const char* functionName, UInt32 remoteModuleBase)
        {
            const HMODULE localModule = GetModuleHandleW(moduleName);
            if (localModule == nullptr)
            {
                return 0;
            }

            const FARPROC localAddress = GetProcAddress(localModule, functionName);
            if (localAddress == nullptr)
            {
                return 0;
            }

            const UIntPtr offset = reinterpret_cast<UIntPtr>(localAddress) - reinterpret_cast<UIntPtr>(localModule);
            return remoteModuleBase + static_cast<UInt32>(offset);
        }

        [[nodiscard]] LPTHREAD_START_ROUTINE ToThreadStart(UInt32 address)
        {
            return reinterpret_cast<LPTHREAD_START_ROUTINE>(static_cast<UIntPtr>(address));
        }
    }

    ProcessInjector::ProcessInjector(HANDLE process, DWORD processId)
        : process(process)
        , processId(processId)
    {
    }

    UInt32 ProcessInjector::InjectLibrary(const std::filesystem::path& dllPath) const
    {
        const UInt32 remoteKernel32 = FindModule(L"kernel32.dll");
        if (remoteKernel32 == 0)
        {
            return 0;
        }

        const UInt32 remoteLoadLibrary = RemoteFunctionAddress(L"kernel32.dll", "LoadLibraryW", remoteKernel32);
        if (remoteLoadLibrary == 0)
        {
            return 0;
        }

        const SIZE_T pathBytes = (dllPath.native().size() + 1) * sizeof(wchar_t);
        void* remoteBuffer = VirtualAllocEx(process, nullptr, pathBytes, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (remoteBuffer == nullptr)
        {
            return 0;
        }

        if (WriteProcessMemory(process, remoteBuffer, dllPath.c_str(), pathBytes, nullptr) == FALSE)
        {
            VirtualFreeEx(process, remoteBuffer, 0, MEM_RELEASE);
            return 0;
        }

        const HANDLE thread =
            CreateRemoteThread(process, nullptr, 0, ToThreadStart(remoteLoadLibrary), remoteBuffer, 0, nullptr);
        if (thread == nullptr)
        {
            VirtualFreeEx(process, remoteBuffer, 0, MEM_RELEASE);
            return 0;
        }

        WaitForSingleObject(thread, INFINITE);

        DWORD moduleBase = 0;
        GetExitCodeThread(thread, &moduleBase);
        CloseHandle(thread);
        VirtualFreeEx(process, remoteBuffer, 0, MEM_RELEASE);

        return moduleBase;
    }

    UInt32 ProcessInjector::FindModule(const wchar_t* moduleName) const
    {
        const HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
        if (snapshot == INVALID_HANDLE_VALUE)
        {
            return 0;
        }

        UInt32 baseAddress = 0;
        MODULEENTRY32W entry = {};
        entry.dwSize = sizeof(entry);

        if (Module32FirstW(snapshot, &entry) != FALSE)
        {
            do
            {
                if (_wcsicmp(entry.szModule, moduleName) == 0)
                {
                    baseAddress = static_cast<UInt32>(reinterpret_cast<UIntPtr>(entry.modBaseAddr));
                    break;
                }
            } while (Module32NextW(snapshot, &entry) != FALSE);
        }

        CloseHandle(snapshot);
        return baseAddress;
    }

    Boolean ProcessInjector::CallExport(UInt32 moduleBase, UInt32 exportOffset, UInt32 argument) const
    {
        if (moduleBase == 0 || exportOffset == 0)
        {
            return False;
        }

        const HANDLE thread = CreateRemoteThread(process, nullptr, 0, ToThreadStart(moduleBase + exportOffset),
            reinterpret_cast<void*>(static_cast<UIntPtr>(argument)), 0, nullptr);
        if (thread == nullptr)
        {
            return False;
        }

        WaitForSingleObject(thread, INFINITE);

        DWORD result = 0;
        GetExitCodeThread(thread, &result);
        CloseHandle(thread);

        return result != 0 ? True : False;
    }
}
