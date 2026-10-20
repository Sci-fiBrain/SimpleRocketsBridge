#pragma once

#include <filesystem>

#include <Windows.h>

#include "SimpleRockets/Types.h"

namespace SimpleRockets::Launcher::Infrastructure
{
    /// <summary>
    /// 启动并持有目标进程
    ///
    /// 以挂起方式创建, 恢复主线程后等待模块加载稳定
    /// </summary>
    class ProcessHost final
    {
    public:
        ProcessHost();
        ~ProcessHost();

        ProcessHost(const ProcessHost&) = delete;
        ProcessHost& operator=(const ProcessHost&) = delete;

        /// <summary>创建挂起的目标进程并恢复主线程</summary>
        [[nodiscard]] Boolean Start(const std::filesystem::path& executablePath);

        /// <summary>等待模块加载稳定, 超时或目标提前退出时返回 0</summary>
        [[nodiscard]] Boolean WaitUntilReady(UInt32 timeoutMilliseconds) const;

        /// <summary>目标进程是否仍在运行</summary>
        [[nodiscard]] Boolean IsRunning() const;

        /// <summary>等待目标退出并返回其退出码</summary>
        [[nodiscard]] UInt32 WaitForExit() const;

        [[nodiscard]] HANDLE Process() const;
        [[nodiscard]] DWORD ProcessId() const;

        void Close();

    private:
        [[nodiscard]] UInt32 CountModules() const;

        PROCESS_INFORMATION processInfo;
        HANDLE jobObject;
    };
}
