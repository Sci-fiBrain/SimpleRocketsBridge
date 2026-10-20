#pragma once

#include <string_view>

#include "Infrastructure/Log/LogQueue.h"
#include "SimpleRockets/SDK/Log/ILogger.h"

namespace SimpleRockets::Launcher::Infrastructure::Log
{
    namespace SdkLog = SimpleRockets::SDK::Log;

    /// <summary>
    /// 本地日志入口
    ///
    /// 只把消息组装成日志包交给队列, 不负责时间文本、着色与输出
    /// </summary>
    class LocalLogger final : public SdkLog::ILogger
    {
    public:
        /// <summary>minimumSeverity 为输出阈值, 低于该等级的调用被丢弃</summary>
        LocalLogger(LogQueue& queue, SdkLog::LogSeverity minimumSeverity);

        LocalLogger(const LocalLogger&) = delete;
        LocalLogger& operator=(const LocalLogger&) = delete;

        [[nodiscard]] Boolean IsEnabled(SdkLog::LogSeverity severity) const noexcept override;

        void Log(SdkLog::LogSeverity severity, std::string_view message) noexcept override;

    private:
        LogQueue& queue;
        SdkLog::LogSeverity minimumSeverity;
    };
}
