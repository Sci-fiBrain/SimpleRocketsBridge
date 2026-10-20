#pragma once

#include <Windows.h>

#include "Application/CommandLine.h"
#include "Infrastructure/ProcessHost.h"
#include "SimpleRockets/SDK/Log/ILogger.h"
#include "SimpleRockets/Types.h"

namespace SimpleRockets::Launcher::Application
{
    /// <summary>进程退出码, 数值即约定</summary>
    enum class ExitCode : UInt32
    {
        Success = 0,
        InvalidArguments = 1,
        PipeCreateFailed = 2,
        ProcessStartFailed = 3,
        LoaderTimeout = 4,
        InjectionFailed = 5,
        BridgeStartFailed = 6,
        ProcessExited = 7
    };

    /// <summary>启动目标进程、注入 SRBridge 并把日志管道写端交给它</summary>
    [[nodiscard]] ExitCode Run(const CommandLineOptions& options, SimpleRockets::SDK::Log::ILogger& logger,
        Infrastructure::ProcessHost& host, HANDLE logPipeWrite);
}
