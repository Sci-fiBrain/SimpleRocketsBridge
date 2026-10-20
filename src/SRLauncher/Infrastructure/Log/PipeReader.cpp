#include "Infrastructure/Log/PipeReader.h"

#include <cstring>

#include "SimpleRockets/SDK/Log/LogRecord.h"

namespace SimpleRockets::Launcher::Infrastructure::Log
{
    namespace SdkLog = SimpleRockets::SDK::Log;

    namespace
    {
        /// <summary>单次读取的字节数</summary>
        constexpr UInt32 ChunkCapacity = 4096;
    }

    PipeReader::PipeReader(HANDLE pipe, LogQueue& queue)
        : pipe(pipe)
        , queue(queue)
        , running(false)
        , pendingLength(0)
    {
    }

    PipeReader::~PipeReader()
    {
        Stop();
    }

    void PipeReader::Start()
    {
        if (worker.joinable())
        {
            return;
        }

        running = true;
        worker = std::thread(&PipeReader::Run, this);
    }

    void PipeReader::Stop()
    {
        running = false;
        if (worker.joinable())
        {
            CancelSynchronousIo(worker.native_handle());
            worker.join();
        }
    }

    void PipeReader::Run()
    {
        Byte chunk[ChunkCapacity];

        while (running)
        {
            DWORD read = 0;
            if (ReadFile(pipe, chunk, sizeof(chunk), &read, nullptr) == FALSE)
            {
                break;
            }

            if (read == 0)
            {
                break;
            }

            Feed(chunk, read);
        }
    }

    void PipeReader::Feed(const Byte* data, UInt32 length)
    {
        if (pendingLength + length > sizeof(pending))
        {
            Reset();
            return;
        }

        std::memcpy(pending + pendingLength, data, length);
        pendingLength += length;

        UInt32 consumed = 0;
        while (pendingLength - consumed >= sizeof(UInt32))
        {
            UInt32 recordLength = 0;
            std::memcpy(&recordLength, pending + consumed, sizeof(recordLength));

            if (recordLength < SdkLog::LogRecordHeaderSize || recordLength > SdkLog::LogRecordMaxLength)
            {
                Reset();
                return;
            }

            if (pendingLength - consumed < recordLength)
            {
                break;
            }

            (void)queue.Push(pending + consumed);
            consumed += recordLength;
        }

        if (consumed > 0)
        {
            std::memmove(pending, pending + consumed, pendingLength - consumed);
            pendingLength -= consumed;
        }
    }

    void PipeReader::Reset()
    {
        pendingLength = 0;
    }
}
