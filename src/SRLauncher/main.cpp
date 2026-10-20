#include <Windows.h>

#include "Application/CommandLine.h"
#include "Application/Run.h"
#include "Infrastructure/Log/ConsoleWriter.h"
#include "Infrastructure/Log/LocalLogger.h"
#include "Infrastructure/Log/LogQueue.h"
#include "Infrastructure/Log/PipeReader.h"
#include "Infrastructure/PipePair.h"
#include "Infrastructure/ProcessHost.h"

/// <summary>进程入口</summary>
int wmain(int argumentCount, wchar_t* arguments[])
{
    using namespace SimpleRockets::Launcher::Application;
    using namespace SimpleRockets::Launcher::Infrastructure;
    using namespace SimpleRockets::Launcher::Infrastructure::Log;
    using namespace SimpleRockets::SDK::Log;

    CommandLineOptions options;
    if (!TryParseCommandLine(argumentCount, arguments, options))
    {
        SetConsoleOutputCP(CP_UTF8);

        const char usage[] = "用法: SRLauncher.exe [游戏 exe 路径]\n";
        DWORD written = 0;
        WriteFile(GetStdHandle(STD_ERROR_HANDLE), usage, static_cast<DWORD>(sizeof(usage) - 1), &written, nullptr);

        return static_cast<int>(ExitCode::InvalidArguments);
    }

    constexpr SimpleRockets::UInt32 QueueCapacityBytes = 3 * 1024 * 1024;

    LogQueue queue(QueueCapacityBytes);
    LocalLogger logger(queue, LogSeverity::Trace);
    ConsoleWriter writer(queue);
    writer.Start();

    PipePair logPipe;
    if (!logPipe.IsValid())
    {
        logger.Log(LogSeverity::Error, "创建管道失败");
        return static_cast<int>(ExitCode::PipeCreateFailed);
    }

    PipeReader reader(logPipe.Read(), queue);
    ProcessHost host;

    reader.Start();

    const ExitCode code = Run(options, logger, host, logPipe.Write());
    if (code != ExitCode::Success)
    {
        return static_cast<int>(code);
    }

    const SimpleRockets::UInt32 targetExitCode = host.WaitForExit();
    reader.Stop();
    writer.Stop();

    return static_cast<int>(targetExitCode == 0 ? ExitCode::Success : ExitCode::ProcessStartFailed);
}
