#pragma once

#include "SimpleRockets/Types.h"

namespace SimpleRockets::Bridge::Infrastructure::Hook
{
    /// <summary>按版本锁定的 RVA 安装全部 hook, 任一步失败则回滚已安装的部分</summary>
    [[nodiscard]] Boolean Install();

    /// <summary>撤掉全部 hook</summary>
    void Uninstall();
}
