#pragma once

namespace SimpleRockets::SDK::Game
{
    /// <summary>
    /// PodObject 的只读视图
    ///
    /// 只持有游戏侧原始指针, 字段通过 Read 与 Offsets.h 的偏移读取
    /// </summary>
    struct PodView
    {
        /// <summary>游戏进程内的 PodObject 地址, 仅在该对象存活期间有效</summary>
        void* pod;
    };
}
