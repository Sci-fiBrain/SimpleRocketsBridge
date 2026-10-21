#pragma once

namespace SimpleRockets::SDK::Game
{
    /// <summary>
    /// 注释待完善
    /// </summary>
    struct Ref
    {
        /// <summary>原始游戏对象指针</summary>
        void* pointer;

        explicit Ref(void* pointer) noexcept : pointer{pointer} { }
    };
}
