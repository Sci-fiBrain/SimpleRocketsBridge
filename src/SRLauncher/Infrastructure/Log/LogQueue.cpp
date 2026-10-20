#include "Infrastructure/Log/LogQueue.h"

#include <cstring>
#include <format>

#include "SimpleRockets/SDK/Log/LogRecord.h"

namespace SimpleRockets::Launcher::Infrastructure::Log
{
    namespace
    {
        namespace SdkLog = SimpleRockets::SDK::Log;

        /// <summary>记录头部中长度字段的字节数</summary>
        constexpr UInt32 LengthFieldSize = sizeof(UInt32);

        /// <summary>播报记录 payload 的缓冲容量</summary>
        constexpr UInt32 NoticeCapacity = 128;

        [[nodiscard]] UInt32 ReadLength(const Byte* address)
        {
            UInt32 value = 0;
            std::memcpy(&value, address, sizeof(value));
            return value;
        }

        void WriteLength(Byte* address, UInt32 value)
        {
            std::memcpy(address, &value, sizeof(value));
        }
    }

    LogQueue::LogQueue(UInt32 capacityBytes)
        : buffer(std::make_unique<Byte[]>(capacityBytes))
        , capacity(capacityBytes)
        , readOffset(0)
        , writeOffset(0)
        , usedBytes(0)
        , droppedRecords(0)
        , droppedBytes(0)
    {
    }

    Boolean LogQueue::Push(const Byte* record)
    {
        if (record == nullptr)
        {
            return false;
        }

        const UInt32 recordLength = ReadLength(record);
        if (recordLength < SdkLog::LogRecordHeaderSize || recordLength > SdkLog::LogRecordMaxLength)
        {
            return false;
        }

        if (recordLength > capacity)
        {
            return false;
        }

        std::lock_guard<std::mutex> guard(lock);

        EnsureRoom(recordLength);

        std::memcpy(buffer.get() + writeOffset, record, recordLength);
        writeOffset += recordLength;
        usedBytes += recordLength;

        WriteNotice();
        return true;
    }

    Boolean LogQueue::Pop(Byte* target, UInt32 targetCapacity, UInt32& length)
    {
        length = 0;

        if (target == nullptr)
        {
            return false;
        }

        std::lock_guard<std::mutex> guard(lock);

        if (usedBytes == 0)
        {
            return false;
        }

        if (readOffset + LengthFieldSize > capacity)
        {
            readOffset = 0;
        }

        UInt32 recordLength = ReadLength(buffer.get() + readOffset);
        if (recordLength == 0)
        {
            readOffset = 0;
            recordLength = ReadLength(buffer.get() + readOffset);
        }

        if (recordLength < SdkLog::LogRecordHeaderSize || recordLength > capacity || recordLength > usedBytes)
        {
            return false;
        }

        if (recordLength > targetCapacity)
        {
            return false;
        }

        std::memcpy(target, buffer.get() + readOffset, recordLength);

        readOffset += recordLength;
        if (readOffset >= capacity)
        {
            readOffset = 0;
        }
        usedBytes -= recordLength;
        length = recordLength;
        return true;
    }

    Boolean LogQueue::CanAppend(UInt32 recordLength) const
    {
        if (usedBytes + recordLength > capacity)
        {
            return false;
        }

        if (usedBytes == 0)
        {
            return true;
        }

        if (readOffset == writeOffset)
        {
            return false;
        }

        if (capacity - writeOffset < recordLength)
        {
            return readOffset >= recordLength;
        }

        if (readOffset > writeOffset && writeOffset + recordLength > readOffset)
        {
            return false;
        }

        return true;
    }

    void LogQueue::EnsureRoom(UInt32 recordLength)
    {
        if (usedBytes == 0)
        {
            readOffset = 0;
            writeOffset = 0;
            return;
        }

        if (capacity - writeOffset < recordLength)
        {
            while (usedBytes > 0 && readOffset < recordLength)
            {
                DropOldest();
            }

            if (usedBytes == 0)
            {
                readOffset = 0;
                writeOffset = 0;
                return;
            }

            if (capacity - writeOffset >= LengthFieldSize)
            {
                WriteLength(buffer.get() + writeOffset, 0);
            }
            writeOffset = 0;
            return;
        }

        while (usedBytes > 0 && readOffset > writeOffset && writeOffset + recordLength > readOffset)
        {
            DropOldest();
        }

        if (usedBytes == 0)
        {
            readOffset = 0;
            writeOffset = 0;
        }
    }

    void LogQueue::DropOldest()
    {
        if (usedBytes == 0)
        {
            return;
        }

        if (readOffset + LengthFieldSize > capacity)
        {
            readOffset = 0;
        }

        if (ReadLength(buffer.get() + readOffset) == 0)
        {
            readOffset = 0;
        }

        const UInt32 recordLength = ReadLength(buffer.get() + readOffset);
        if (recordLength < SdkLog::LogRecordHeaderSize || recordLength > capacity || recordLength > usedBytes)
        {
            readOffset = 0;
            writeOffset = 0;
            usedBytes = 0;
            return;
        }

        readOffset += recordLength;
        if (readOffset >= capacity)
        {
            readOffset = 0;
        }

        usedBytes -= recordLength;
        droppedRecords += 1;
        droppedBytes += recordLength;
    }

    void LogQueue::WriteNotice()
    {
        if (droppedRecords == 0)
        {
            return;
        }

        char payload[NoticeCapacity] = {};
        const std::format_to_n_result<char*> formatted =
            std::format_to_n(payload, NoticeCapacity - 1, "丢弃 {} 条 / {} 字节", droppedRecords, droppedBytes);

        UInt32 payloadLength = static_cast<UInt32>(formatted.size);
        if (payloadLength > NoticeCapacity - 1)
        {
            payloadLength = NoticeCapacity - 1;
        }

        const UInt32 recordLength = SdkLog::LogRecordHeaderSize + payloadLength;

        if (usedBytes == 0)
        {
            readOffset = 0;
            writeOffset = 0;
        }

        if (!CanAppend(recordLength))
        {
            return;
        }

        if (capacity - writeOffset < recordLength)
        {
            if (capacity - writeOffset >= LengthFieldSize)
            {
                WriteLength(buffer.get() + writeOffset, 0);
            }
            writeOffset = 0;
        }

        Byte header[SdkLog::LogRecordHeaderSize] = {};
        WriteLength(header, recordLength);
        header[LengthFieldSize] = static_cast<Byte>(SdkLog::LogSeverity::Dropped);

        std::memcpy(buffer.get() + writeOffset, header, sizeof(header));
        std::memcpy(buffer.get() + writeOffset + sizeof(header), payload, payloadLength);

        writeOffset += recordLength;
        usedBytes += recordLength;

        droppedRecords = 0;
        droppedBytes = 0;
    }
}
