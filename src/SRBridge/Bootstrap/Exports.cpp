#include <new>

#include <Windows.h>

#include "Bootstrap/Bridge.h"
#include "Infrastructure/Log/PipeLogger.h"
#include "SimpleRockets/SDK/Log/ILogger.h"

namespace
{
    std::unique_ptr<SimpleRockets::Bridge::Infrastructure::Log::PipeLogger> logger;
}

namespace SimpleRockets::Bridge::Bootstrap
{
    SDK::Log::ILogger* GetLogger()
    {
        return logger.get();
    }
}

/// <summary>
/// 由 Launcher 远程调用, 传入日志管道的可写端句柄
///
/// 线程入口约定为 WINAPI, 返回值即远程调用的结果
/// </summary>
extern "C" DWORD WINAPI SRBridgeStart(void* pipeHandle)
{
    using namespace SimpleRockets::Bridge::Infrastructure::Log;
    using namespace SimpleRockets::SDK::Log;

    if (pipeHandle == nullptr || pipeHandle == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    logger.reset(new (std::nothrow) PipeLogger(static_cast<HANDLE>(pipeHandle), LogSeverity::Trace));
    if (logger == nullptr)
    {
        return FALSE;
    }

    logger->Log(LogSeverity::Info, "SRBridge 已连接");
    return TRUE;
}
