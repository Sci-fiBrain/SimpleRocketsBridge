#pragma once

#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>
    /// PodObject 的只读视图
    ///
    /// 只持有游戏侧原始指针, 字段通过 Read 与下方偏移读取
    /// </summary>
    struct PodView
    {
        /// <summary>游戏进程内的 PodObject 地址, 仅在该对象存活期间有效</summary>
        void* pod;
    };

    /// <summary>PodObject 的实时油门, 取值 0..1</summary>
    constexpr Offset OffsetPodThrottle = 0x200;

    /// <summary>PodObject 的当前阶段索引</summary>
    constexpr Offset OffsetPodStageIndex = 0x234;
}
