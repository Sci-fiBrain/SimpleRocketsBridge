#pragma once

#include "SimpleRockets/SDK/Game/Ref.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>
    /// 轨道根数, 纯数据无虚表
    ///
    /// 游戏每帧释放旧的并重新计算, 节点上的指针随时变化
    /// </summary>
    struct OrbitRef : Ref
    {
        explicit OrbitRef(void* pointer) noexcept : Ref{pointer} { }
    };
}
