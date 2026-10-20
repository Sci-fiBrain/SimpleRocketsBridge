#include "Infrastructure/Log/PipeLogger.h"

#include <chrono>
#include <cstring>

#include "SimpleRockets/SDK/Log/LogRecord.h"

namespace SimpleRockets::Bridge::Infrastructure::Log
{
    namespace
    {
        /// <summary>自 Unix epoch 起的 UTC 毫秒</summary>
        [[nodiscard]] UInt64 CurrentTimeMilliseconds() noexcept
        {
            const std::chrono::milliseconds elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch());
            return static_cast<UInt64>(elapsed.count());
        }

        constexpr UInt32 SeverityOffset = sizeof(UInt32);
        constexpr UInt32 ReservedOffset = SeverityOffset + sizeof(UInt8);
        constexpr UInt32 TimeOffset = static_cast<UInt32>(offsetof(SdkLog::LogRecordHeader, timeMilliseconds));
        constexpr UInt32 MaxPayloadLength = SdkLog::LogRecordMaxLength - SdkLog::LogRecordHeaderSize;
    }

    PipeLogger::PipeLogger(HANDLE pipe, SdkLog::LogSeverity minimumSeverity)
        : pipe(pipe)
        , minimumSeverity(minimumSeverity)
        , broken(false)
    {
    }

    Boolean PipeLogger::IsEnabled(SdkLog::LogSeverity severity) const noexcept
    {
        if (broken)
        {
            return false;
        }

        return severity >= minimumSeverity;
    }

    void PipeLogger::Log(SdkLog::LogSeverity severity, std::string_view message) noexcept
    {
        if (!IsEnabled(severity))
        {
            return;
        }

        UInt32 payloadLength = static_cast<UInt32>(message.size());
        Byte truncated = 0;
        if (payloadLength > MaxPayloadLength)
        {
            payloadLength = MaxPayloadLength;
            truncated = 1;
        }

        const UInt32 recordLength = SdkLog::LogRecordHeaderSize + payloadLength;

        Byte record[SdkLog::LogRecordMaxLength];
        std::memcpy(record, &recordLength, sizeof(recordLength));
        record[SeverityOffset] = static_cast<Byte>(severity);
        record[ReservedOffset] = truncated;
        record[ReservedOffset + 1] = 0;
        record[ReservedOffset + 2] = 0;

        const UInt64 timeMilliseconds = CurrentTimeMilliseconds();
        std::memcpy(record + TimeOffset, &timeMilliseconds, sizeof(timeMilliseconds));

        if (payloadLength > 0)
        {
            std::memcpy(record + SdkLog::LogRecordHeaderSize, message.data(), payloadLength);
        }

        std::lock_guard<std::mutex> guard(lock);

        if (broken)
        {
            return;
        }

        DWORD written = 0;
        if (WriteFile(pipe, record, recordLength, &written, nullptr) == FALSE || written != recordLength)
        {
            broken = true;
        }
    }
}
