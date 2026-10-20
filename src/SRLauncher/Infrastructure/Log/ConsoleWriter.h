#pragma once

#include <atomic>
#include <chrono>
#include <thread>

#include <Windows.h>

#include "Infrastructure/Log/LogQueue.h"
#include "SimpleRockets/Types.h"

namespace SimpleRockets::Launcher::Infrastructure::Log
{
    /// <summary>
    /// 把队列中的日志记录输出到控制台
    ///
    /// 独立线程轮询队列, 控制台支持 ANSI 转义时按等级着色, 重定向输出时退化为纯文本
    /// </summary>
    class ConsoleWriter final
    {
    public:
        explicit ConsoleWriter(LogQueue& queue);
        ~ConsoleWriter();

        ConsoleWriter(const ConsoleWriter&) = delete;
        ConsoleWriter& operator=(const ConsoleWriter&) = delete;

        /// <summary>把控制台代码页切到 UTF-8 并启动输出线程</summary>
        void Start();

        /// <summary>停止输出线程并排空队列剩余记录</summary>
        void Stop();

    private:
        void Run();
        void WriteRecord(const Byte* record, UInt32 length);
        Boolean DetectAnsi() const;

        LogQueue& queue;
        std::atomic<Boolean> running;
        std::thread worker;
        HANDLE output;
        Boolean ansiEnabled;
        const std::chrono::time_zone* zone;
    };
}
