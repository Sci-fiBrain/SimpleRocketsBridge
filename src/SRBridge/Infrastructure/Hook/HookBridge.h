#pragma once

#include "SimpleRockets/SDK/Log/ILogger.h"
#include "SimpleRockets/Types.h"

namespace SimpleRockets::Bridge::Infrastructure::Hook
{
    /// <summary>按版本锁定的 RVA 安装全部 hook, 任一步失败则回滚已安装的部分</summary>
    /// <param name="moduleBase">游戏模块基址, hook 目标地址由它加 RVA 得到</param>
    /// <param name="logTarget">detour 中写日志用, 存活期须覆盖整个 hook 生效期</param>
    [[nodiscard]] Boolean Install(void* moduleBase, SDK::Log::ILogger* logTarget);

    /// <summary>撤掉全部 hook</summary>
    void Uninstall();
}
