#include <new>

#include <Windows.h>

#include "Infrastructure/Hook/HookBridge.h"
#include "Infrastructure/Log/PipeLogger.h"
#include "SimpleRockets/SDK/Log/ILogger.h"

namespace
{
    std::unique_ptr<SimpleRockets::Bridge::Infrastructure::Log::PipeLogger> pipeLogger;
}

/// <summary>
/// 由 Launcher 远程调用, 传入日志管道的可写端句柄
///
/// 线程入口约定为 WINAPI, 返回值即远程调用的结果
/// </summary>
extern "C" DWORD WINAPI SRBridgeStart(void* pipeHandle)
{
    using namespace SimpleRockets::Bridge::Infrastructure::Hook;
    using namespace SimpleRockets::Bridge::Infrastructure::Log;
    using namespace SimpleRockets::SDK::Log;

    if (pipeHandle == nullptr || pipeHandle == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    pipeLogger.reset(new (std::nothrow) PipeLogger(static_cast<HANDLE>(pipeHandle), LogSeverity::Trace));
    if (pipeLogger == nullptr)
    {
        return FALSE;
    }

    ILogger& logger = *pipeLogger;
    logger.Log(LogSeverity::Info, "SRBridge 已连接");

    if (!Install(GetModuleHandleW(nullptr), &logger))
    {
        logger.Log(LogSeverity::Error, "hook 安装失败");
        return FALSE;
    }

    logger.Log(LogSeverity::Info, "hook 已安装");
    return TRUE;
}
