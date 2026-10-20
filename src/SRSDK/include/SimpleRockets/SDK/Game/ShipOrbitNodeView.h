#pragma once

#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>
    /// ShipOrbitNode 的只读视图
    ///
    /// 只持有游戏侧原始指针, 字段通过 Read 与下方偏移读取
    /// </summary>
    struct ShipOrbitNodeView
    {
        /// <summary>游戏进程内的 ShipOrbitNode 地址, 仅在该对象存活期间有效</summary>
        void* node;
    };

    /// <summary>ShipOrbitNode 指向所属 Ship</summary>
    constexpr Offset OffsetNodeShip = 0x160;
}
