#pragma once

#include <cstddef>

#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Log
{
    /// <summary>
    /// 日志严重性, 数值即 ABI, 只允许追加
    /// </summary>
    enum class LogSeverity : UInt8
    {
        Trace = 0,
        Debug = 1,
        Info = 2,
        Warn = 3,
        Error = 4,
        Fatal = 5,

        /// <summary>队列丢弃最旧日志时插入的播报记录</summary>
        Dropped = 0xFF
    };

    /// <summary>
    /// 日志包头部, 其后紧跟 payload 字节
    /// </summary>
    struct LogRecordHeader
    {
        /// <summary>整包长度, 含头部自身</summary>
        UInt32 length;

        /// <summary>取值为 LogSeverity</summary>
        UInt8 severity;

        /// <summary>reserved[0] 为 1 表示 payload 已截断</summary>
        UInt8 reserved[3];

        /// <summary>自 Unix epoch 起的 UTC 毫秒</summary>
        UInt64 timeMilliseconds;
    };

    /// <summary>头部长度, 同时是 payload 的起点偏移</summary>
    constexpr UInt32 LogRecordHeaderSize = 16;

    /// <summary>整包长度上限, payload 上限为 LogRecordMaxLength - LogRecordHeaderSize</summary>
    constexpr UInt32 LogRecordMaxLength = 4096;

    static_assert(sizeof(LogSeverity) == 1, "LogSeverity layout changed");
    static_assert(sizeof(LogRecordHeader) == LogRecordHeaderSize, "LogRecordHeader layout changed");
    static_assert(offsetof(LogRecordHeader, timeMilliseconds) == 8, "LogRecordHeader.timeMilliseconds offset changed");
}
