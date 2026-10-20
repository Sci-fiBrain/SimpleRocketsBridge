#pragma once

#include <atomic>
#include <mutex>

#include <Windows.h>

#include "SimpleRockets/SDK/Log/ILogger.h"

namespace SimpleRockets::Bridge::Infrastructure::Log
{
    namespace SdkLog = SimpleRockets::SDK::Log;

    /// <summary>
    /// 通过管道把日志送到 Launcher
    ///
    /// 一次调用写出一条完整记录, 写入失败后永久关闭, 后续调用直接返回
    /// </summary>
    class PipeLogger final : public SdkLog::ILogger
    {
    public:
        /// <summary>pipe 为可写端句柄, minimumSeverity 为输出阈值</summary>
        PipeLogger(HANDLE pipe, SdkLog::LogSeverity minimumSeverity);

        PipeLogger(const PipeLogger&) = delete;
        PipeLogger& operator=(const PipeLogger&) = delete;

        [[nodiscard]] Boolean IsEnabled(SdkLog::LogSeverity severity) const noexcept override;

        void Log(SdkLog::LogSeverity severity, std::string_view message) noexcept override;

    private:
        HANDLE pipe;
        SdkLog::LogSeverity minimumSeverity;
        std::mutex lock;
        std::atomic<Boolean> broken;
    };
}
