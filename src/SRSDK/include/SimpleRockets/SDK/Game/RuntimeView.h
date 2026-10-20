#pragma once

#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>
    /// Runtime 的只读视图
    ///
    /// 只持有游戏侧原始指针, 字段通过 Read 与下方偏移读取
    /// </summary>
    struct RuntimeView
    {
        /// <summary>游戏进程内的 Runtime 地址, 仅在该 Runtime 存活期间有效</summary>
        void* runtime;
    };

    /// <summary>Runtime 指向玩家飞船的轨道节点</summary>
    constexpr Offset OffsetRuntimePlayerNode = 0x0C;

    /// <summary>Runtime 的时间计数, 单位秒, 首次分级前不推进</summary>
    constexpr Offset OffsetRuntimeElapsedTime = 0x10;

    /// <summary>Runtime 的当前姿态角, 单位弧度</summary>
    constexpr Offset OffsetRuntimeAttitude = 0x40;

    /// <summary>Runtime 的角速度, 单位弧度每秒</summary>
    constexpr Offset OffsetRuntimeAttitudeRate = 0x50;

    /// <summary>Runtime 的时间加速倍率</summary>
    constexpr Offset OffsetRuntimeTimeScale = 0x78;
}
