#pragma once

#include <string_view>
#include <type_traits>

#include "SimpleRockets/SDK/Log/LogRecord.h"

namespace SimpleRockets::SDK::Log
{
    /// <summary>
    /// 日志写入接口
    ///
    /// message 只在本次调用期间有效, 实现必须立即拷贝
    /// </summary>
    class ILogger
    {
    public:
        virtual ~ILogger() = default;

        /// <summary>该等级当前是否会被输出, 调用方据此决定是否构造消息</summary>
        [[nodiscard]] virtual Boolean IsEnabled(LogSeverity severity) const noexcept = 0;

        /// <summary>写入一条日志, 超过 LogRecordMaxLength 的内容按实现策略截断</summary>
        virtual void Log(LogSeverity severity, std::string_view message) noexcept = 0;

        ILogger(const ILogger&) = delete;
        ILogger& operator=(const ILogger&) = delete;

    protected:
        ILogger() = default;
    };

    static_assert(std::is_abstract_v<ILogger>, "ILogger must stay abstract");
    static_assert(sizeof(ILogger) == sizeof(void*), "ILogger must not carry data");
}
