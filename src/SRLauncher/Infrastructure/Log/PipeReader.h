#pragma once

#include <atomic>
#include <thread>

#include <Windows.h>

#include "Infrastructure/Log/LogQueue.h"
#include "SimpleRockets/Types.h"

namespace SimpleRockets::Launcher::Infrastructure::Log
{
    /// <summary>
    /// 从管道读取字节流并还原成完整记录
    ///
    /// 独立线程阻塞读取, 累积不完整的记录直到凑齐后交给队列
    /// </summary>
    class PipeReader final
    {
    public:
        /// <summary>pipe 为可读端句柄</summary>
        PipeReader(HANDLE pipe, LogQueue& queue);
        ~PipeReader();

        PipeReader(const PipeReader&) = delete;
        PipeReader& operator=(const PipeReader&) = delete;

        /// <summary>启动读取线程</summary>
        void Start();

        /// <summary>中断阻塞读取并等待线程退出</summary>
        void Stop();

    private:
        void Run();
        void Feed(const Byte* data, UInt32 length);
        void Reset();

        HANDLE pipe;
        LogQueue& queue;
        std::atomic<Boolean> running;
        std::thread worker;

        /// <summary>未凑成整条记录的残片, 容量需容纳一条最大记录再加一次读取</summary>
        Byte pending[2 * 4096];
        UInt32 pendingLength;
    };
}
