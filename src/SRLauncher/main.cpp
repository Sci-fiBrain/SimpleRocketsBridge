#include <format>
#include <string_view>

#include "Infrastructure/Log/ConsoleWriter.h"
#include "Infrastructure/Log/LocalLogger.h"
#include "Infrastructure/Log/LogQueue.h"
#include "SimpleRockets/SDK/Log/ILogger.h"
#include "SimpleRockets/SDK/Version.h"

using namespace SimpleRockets::Launcher::Infrastructure::Log;
using namespace SimpleRockets::SDK::Log;

/// <summary>
/// 进程入口, 后续负责启动目标进程、注入 SRBridge 并转发子进程输出
/// </summary>
int main()
{
    constexpr SimpleRockets::UInt32 QueueCapacityBytes = 3 * 1024 * 1024;

    LogQueue queue(QueueCapacityBytes);
    LocalLogger logger(queue, LogSeverity::Trace);
    ConsoleWriter writer(queue);

    writer.Start();

    ILogger& log = logger;
    log.Log(LogSeverity::Info,
        std::format("SRLauncher {}.{}.{}", SRSDK_VERSION_MAJOR, SRSDK_VERSION_MINOR, SRSDK_VERSION_PATCH));
    log.Log(LogSeverity::Debug, "日志队列已就绪");
    log.Log(LogSeverity::Warn, "这条是警告级别的样例输出");

    writer.Stop();
    return 0;
}
