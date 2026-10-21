#include "Infrastructure/ProcessHost.h"

#include <TlHelp32.h>

namespace SimpleRockets::Launcher::Infrastructure
{
    namespace
    {
        /// <summary>判定加载稳定所需的连续相同模块数次数</summary>
        constexpr UInt32 StableRoundsRequired = 3;

        /// <summary>每次轮询的间隔, 单位毫秒</summary>
        constexpr DWORD PollIntervalMilliseconds = 50;
    }

    ProcessHost::ProcessHost()
        : processInfo{}
        , jobObject(nullptr)
    {
    }

    ProcessHost::~ProcessHost()
    {
        Close();
    }

    Boolean ProcessHost::Start(const std::filesystem::path& executablePath)
    {
        jobObject = CreateJobObjectW(nullptr, nullptr);
        if (jobObject == nullptr)
        {
            return false;
        }

        JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits = {};
        limits.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
        if (!SetInformationJobObject(jobObject, JobObjectExtendedLimitInformation, &limits, sizeof(limits)))
        {
            Close();
            return false;
        }

        STARTUPINFOW startupInfo = {};
        startupInfo.cb = sizeof(startupInfo);

        const std::filesystem::path workingDirectory = executablePath.parent_path();
        const Boolean created = CreateProcessW(executablePath.c_str(), nullptr, nullptr, nullptr, TRUE,
            CREATE_SUSPENDED, nullptr, workingDirectory.c_str(), &startupInfo, &processInfo) != 0;
        if (!created)
        {
            Close();
            return false;
        }

        if (!AssignProcessToJobObject(jobObject, processInfo.hProcess))
        {
            Close();
            return false;
        }

        if (ResumeThread(processInfo.hThread) == static_cast<DWORD>(-1))
        {
            Close();
            return false;
        }

        return true;
    }

    Boolean ProcessHost::WaitUntilReady(UInt32 timeoutMilliseconds) const
    {
        if (processInfo.hProcess == nullptr)
        {
            return false;
        }

        WaitForInputIdle(processInfo.hProcess, timeoutMilliseconds);

        const ULONGLONG deadline = GetTickCount64() + timeoutMilliseconds;
        UInt32 previousCount = 0;
        UInt32 stableRounds = 0;

        while (GetTickCount64() < deadline)
        {
            if (WaitForSingleObject(processInfo.hProcess, 0) == WAIT_OBJECT_0)
            {
                return false;
            }

            const UInt32 count = CountModules();
            if (count > 0 && count == previousCount)
            {
                stableRounds += 1;
                if (stableRounds >= StableRoundsRequired)
                {
                    return true;
                }
            }
            else
            {
                stableRounds = 0;
            }

            previousCount = count;
            Sleep(PollIntervalMilliseconds);
        }

        return false;
    }

    Boolean ProcessHost::IsRunning() const
    {
        if (processInfo.hProcess == nullptr)
        {
            return false;
        }

        return WaitForSingleObject(processInfo.hProcess, 0) == WAIT_TIMEOUT;
    }

    UInt32 ProcessHost::WaitForExit() const
    {
        WaitForSingleObject(processInfo.hProcess, INFINITE);

        DWORD exitCode = 0;
        GetExitCodeProcess(processInfo.hProcess, &exitCode);
        return exitCode;
    }

    HANDLE ProcessHost::Process() const
    {
        return processInfo.hProcess;
    }

    DWORD ProcessHost::ProcessId() const
    {
        return processInfo.dwProcessId;
    }

    void ProcessHost::Close()
    {
        if (processInfo.hThread != nullptr)
        {
            CloseHandle(processInfo.hThread);
            processInfo.hThread = nullptr;
        }

        if (processInfo.hProcess != nullptr)
        {
            CloseHandle(processInfo.hProcess);
            processInfo.hProcess = nullptr;
        }

        if (jobObject != nullptr)
        {
            CloseHandle(jobObject);
            jobObject = nullptr;
        }
    }

    UInt32 ProcessHost::CountModules() const
    {
        const HANDLE snapshot =
            CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processInfo.dwProcessId);
        if (snapshot == INVALID_HANDLE_VALUE)
        {
            return 0;
        }

        UInt32 count = 0;
        MODULEENTRY32W entry = {};
        entry.dwSize = sizeof(entry);

        if (Module32FirstW(snapshot, &entry))
        {
            do
            {
                count += 1;
            } while (Module32NextW(snapshot, &entry));
        }

        CloseHandle(snapshot);
        return count;
    }
}
