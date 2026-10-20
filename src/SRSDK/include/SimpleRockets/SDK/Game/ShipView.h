#pragma once

#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>
    /// Ship 的只读视图
    ///
    /// 只持有游戏侧原始指针, 字段通过 Read 与下方偏移读取
    /// </summary>
    struct ShipView
    {
        /// <summary>游戏进程内的 Ship 地址, 仅在该对象存活期间有效</summary>
        void* ship;
    };

    /// <summary>Ship 指向根 PodObject</summary>
    constexpr Offset OffsetShipPod = 0x24;
}
