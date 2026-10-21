#include "Infrastructure/Log/ConsoleWriter.h"

#include <cstring>
#include <format>

#include "SimpleRockets/SDK/Log/LogRecord.h"

namespace SimpleRockets::Launcher::Infrastructure::Log
{
    namespace SdkLog = SimpleRockets::SDK::Log;

    namespace
    {
        /// <summary>队列空转时的等待间隔, 单位毫秒</summary>
        constexpr DWORD IdleSleepMilliseconds = 5;

        /// <summary>整行缓冲, 头部前缀之外还要容纳转义序列与时区偏移</summary>
        constexpr UInt32 LineCapacity = SdkLog::LogRecordMaxLength + 96;

        /// <summary>头部中时间字段的偏移</summary>
        constexpr UInt32 TimeOffset = static_cast<UInt32>(offsetof(SdkLog::LogRecordHeader, timeMilliseconds));

        [[nodiscard]] const char* SeverityText(Byte severity)
        {
            switch (severity)
            {
            case 0:
                return "TRACE";
            case 1:
                return "DEBUG";
            case 2:
                return "INFO ";
            case 3:
                return "WARN ";
            case 4:
                return "ERROR";
            case 5:
                return "FATAL";
            default:
                return "DROPPED";
            }
        }

        [[nodiscard]] const char* SeverityColor(Byte severity)
        {
            switch (severity)
            {
            case 0:
                return "\x1b[90m";
            case 1:
                return "\x1b[36m";
            case 2:
                return "";
            case 3:
                return "\x1b[33m";
            case 4:
                return "\x1b[31m";
            case 5:
                return "\x1b[1;31m";
            default:
                return "\x1b[35m";
            }
        }
    }

    ConsoleWriter::ConsoleWriter(LogQueue& queue)
        : queue(queue)
        , running(false)
        , output(GetStdHandle(STD_OUTPUT_HANDLE))
        , ansiEnabled(false)
        , zone(nullptr)
    {
    }

    ConsoleWriter::~ConsoleWriter()
    {
        Stop();
    }

    void ConsoleWriter::Start()
    {
        if (worker.joinable())
        {
            return;
        }

        SetConsoleOutputCP(CP_UTF8);

        try
        {
            zone = std::chrono::current_zone();
        }
        catch (...)
        {
            zone = nullptr;
        }

        ansiEnabled = DetectAnsi();
        running = true;
        worker = std::thread(&ConsoleWriter::Run, this);
    }

    void ConsoleWriter::Stop()
    {
        running = false;
        if (worker.joinable())
        {
            worker.join();
        }
    }

    Boolean ConsoleWriter::DetectAnsi() const
    {
        DWORD mode = 0;
        if (!GetConsoleMode(output, &mode))
        {
            return false;
        }

        if ((mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0)
        {
            return true;
        }

        return SetConsoleMode(output, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
    }

    void ConsoleWriter::Run()
    {
        Byte buffer[SdkLog::LogRecordMaxLength];
        UInt32 length = 0;

        while (running)
        {
            Boolean wroteAny = false;
            while (queue.Pop(buffer, sizeof(buffer), length))
            {
                WriteRecord(buffer, length);
                wroteAny = true;
            }

            if (!wroteAny)
            {
                Sleep(IdleSleepMilliseconds);
            }
        }

        while (queue.Pop(buffer, sizeof(buffer), length))
        {
            WriteRecord(buffer, length);
        }
    }

    void ConsoleWriter::WriteRecord(const Byte* record, UInt32 length)
    {
        if (length <= SdkLog::LogRecordHeaderSize)
        {
            return;
        }

        const Byte severity = record[4];

        UInt64 timeMilliseconds = 0;
        std::memcpy(&timeMilliseconds, record + TimeOffset, sizeof(timeMilliseconds));

        const char* payload = reinterpret_cast<const char*>(record + SdkLog::LogRecordHeaderSize);
        const UInt32 payloadLength = length - SdkLog::LogRecordHeaderSize;

        const std::chrono::sys_time<std::chrono::milliseconds> utc{std::chrono::milliseconds(timeMilliseconds)};

        char timeText[48] = {};
        Int32 offsetMinutes = 0;
        if (zone != nullptr)
        {
            const std::chrono::zoned_time<std::chrono::milliseconds> zoned(zone, utc);
            const std::format_to_n_result<char*> written =
                std::format_to_n(timeText, sizeof(timeText) - 1, "{:%Y-%m-%dT%H:%M:%S}", zoned);
            *written.out = '\0';
            offsetMinutes =
                static_cast<Int32>(std::chrono::duration_cast<std::chrono::minutes>(zoned.get_info().offset).count());
        }
        else
        {
            const std::format_to_n_result<char*> written =
                std::format_to_n(timeText, sizeof(timeText) - 1, "{:%Y-%m-%dT%H:%M:%S}", utc);
            *written.out = '\0';
        }

        const char sign = offsetMinutes < 0 ? '-' : '+';
        const Int32 absoluteOffset = offsetMinutes < 0 ? -offsetMinutes : offsetMinutes;
        const Int32 offsetHour = absoluteOffset / 60;
        const Int32 offsetMinute = absoluteOffset % 60;
        const char* level = SeverityText(severity);
        const std::string_view timeTextView(timeText);

        char line[LineCapacity];
        std::format_to_n_result<char*> formatted = {};
        if (ansiEnabled)
        {
            formatted = std::format_to_n(line, sizeof(line) - 1, "\x1b[90m[{}{}{:02}:{:02}]\x1b[0m {}[{}]\x1b[0m ",
                timeTextView, sign, offsetHour, offsetMinute, SeverityColor(severity), level);
        }
        else
        {
            formatted = std::format_to_n(
                line, sizeof(line) - 1, "[{}{}{:02}:{:02}] [{}] ", timeTextView, sign, offsetHour, offsetMinute, level);
        }

        UInt32 cursor = static_cast<UInt32>(formatted.out - line);
        if (payloadLength > 0)
        {
            std::memcpy(line + cursor, payload, payloadLength);
            cursor += payloadLength;
        }
        line[cursor] = '\n';
        cursor += 1;

        DWORD written = 0;
        WriteFile(output, line, cursor, &written, nullptr);
    }
}
