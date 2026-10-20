#include "Application/Run.h"

#include <filesystem>

#include "Infrastructure/PathUtil.h"
#include "Infrastructure/ProcessInjector.h"

namespace SimpleRockets::Launcher::Application
{
    namespace
    {
        constexpr UInt32 LoaderTimeoutMilliseconds = 15000;

        /// <summary>取 DLL 导出函数相对模块基址的偏移</summary>
        [[nodiscard]] UInt32 ExportOffset(HMODULE module, const char* exportName)
        {
            const FARPROC address = GetProcAddress(module, exportName);
            if (address == nullptr)
            {
                return 0;
            }

            return static_cast<UInt32>(reinterpret_cast<UIntPtr>(address) - reinterpret_cast<UIntPtr>(module));
        }
    }

    ExitCode Run(const CommandLineOptions& options, SimpleRockets::SDK::Log::ILogger& logger,
        Infrastructure::ProcessHost& host, HANDLE logPipeWrite)
    {
        using namespace SimpleRockets::Launcher::Infrastructure;
        using namespace SimpleRockets::SDK::Log;

        if (host.Start(options.executablePath) == False)
        {
            logger.Log(LogSeverity::Error, "启动目标进程失败");
            return ExitCode::ProcessStartFailed;
        }

        logger.Log(LogSeverity::Info, "目标进程已启动, 等待加载器就绪");

        if (host.WaitUntilReady(LoaderTimeoutMilliseconds) == False)
        {
            if (host.IsRunning() == False)
            {
                logger.Log(LogSeverity::Error, "目标进程在加载完成前退出");
                return ExitCode::ProcessExited;
            }
            logger.Log(LogSeverity::Error, "等待加载器就绪超时");
            return ExitCode::LoaderTimeout;
        }

        const std::filesystem::path dllPath = ModuleDirectory() / L"SRBridge.dll";
        ProcessInjector injector(host.Process(), host.ProcessId());

        const UInt32 remoteBase = injector.InjectLibrary(dllPath);
        if (remoteBase == 0)
        {
            logger.Log(LogSeverity::Error, "注入 SRBridge.dll 失败");
            return ExitCode::InjectionFailed;
        }

        logger.Log(LogSeverity::Info, "SRBridge.dll 已载入目标进程");

        const HMODULE localBridge = LoadLibraryW(dllPath.c_str());
        if (localBridge == nullptr)
        {
            logger.Log(LogSeverity::Error, "无法读取 SRBridge.dll 导出表");
            return ExitCode::InjectionFailed;
        }

        const UInt32 exportOffset = ExportOffset(localBridge, "SRBridgeStart");
        if (exportOffset == 0)
        {
            logger.Log(LogSeverity::Error, "SRBridgeStart 导出缺失");
            return ExitCode::InjectionFailed;
        }

        const UInt32 pipeValue = static_cast<UInt32>(reinterpret_cast<UIntPtr>(logPipeWrite));
        if (injector.CallExport(remoteBase, exportOffset, pipeValue) == False)
        {
            logger.Log(LogSeverity::Error, "SRBridgeStart 调用失败");
            return ExitCode::BridgeStartFailed;
        }

        logger.Log(LogSeverity::Info, "SRBridge 已就绪");
        return ExitCode::Success;
    }
}
