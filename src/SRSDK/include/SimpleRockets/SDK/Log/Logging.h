#pragma once

#include <format>
#include <string>
#include <string_view>
#include <utility>

#include "SimpleRockets/SDK/Log/ILogger.h"

namespace SimpleRockets::SDK::Log
{
    /// <summary>
    /// logger 非空且该等级会输出时为真, 用来决定是否构造消息
    /// </summary>
    [[nodiscard]] inline Boolean IsLogEnabled(ILogger* logger, LogSeverity severity) noexcept
    {
        return logger != nullptr && logger->IsEnabled(severity);
    }

    /// <summary>
    /// 写出已拼好的消息, logger 为空或该等级不输出时直接返回
    /// </summary>
    inline void LogMessage(ILogger* logger, LogSeverity severity, std::string_view message) noexcept
    {
        if (!IsLogEnabled(logger, severity))
        {
            return;
        }

        logger->Log(severity, message);
    }

    /// <summary>
    /// 按格式构造消息后写出, 该等级不输出时连格式化一起跳过
    /// </summary>
    template <typename... TArguments>
    void LogFormat(ILogger* logger, LogSeverity severity, std::format_string<TArguments...> formatString,
        TArguments&&... arguments)
    {
        if (!IsLogEnabled(logger, severity))
        {
            return;
        }

        const std::string message = std::format(formatString, std::forward<TArguments>(arguments)...);
        logger->Log(severity, message);
    }
}
