#pragma once

#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>游戏侧二维向量</summary>
    struct Vector2D
    {
        Double x;
        Double y;
    };

    static_assert(sizeof(Vector2D) == 16, "Vector2D 必须与游戏内布局同宽 (2 x Double)");

    /// <summary>游戏内 cfw::List 的布局</summary>
    namespace cfw
    {
        template <typename T> struct List
        {
            void* vtable;
            T* begin;
            T* end;
            T* capacity;
        };
    }

    static_assert(sizeof(cfw::List<void*>) == 16, "cfw::List 必须与游戏内布局同宽 (vptr + 3 x 指针)");
}
