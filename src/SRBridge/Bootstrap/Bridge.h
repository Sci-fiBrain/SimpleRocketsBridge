#pragma once

#include "SimpleRockets/SDK/Log/ILogger.h"

namespace SimpleRockets::Bridge::Bootstrap
{
    /// <summary>取日志接口, 空表示尚未连接 Launcher</summary>
    [[nodiscard]] SDK::Log::ILogger* GetLogger();
}
