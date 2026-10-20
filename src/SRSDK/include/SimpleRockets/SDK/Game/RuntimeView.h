#pragma once

namespace SimpleRockets::SDK::Game
{
    /// <summary>
    /// Runtime 的只读视图
    ///
    /// 只持有游戏侧原始指针, 字段通过 Read 与 Offsets.h 的偏移读取
    /// </summary>
    struct RuntimeView
    {
        /// <summary>游戏进程内的 Runtime 对象地址, 仅在该 Runtime 存活期间有效</summary>
        void* runtime;
    };
}
