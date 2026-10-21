#pragma once

#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>
    /// 按字节偏移读取游戏对象字段
    ///
    /// 只做原始读取, 不校验地址有效性; offset 取自对应的 Ref 头
    /// </summary>
    /// <param name="base">游戏对象地址, 来自视图的 runtime 字段</param>
    /// <param name="offset">相对对象起始地址的偏移</param>
    template <typename T> [[nodiscard]] T Read(const void* base, Offset offset) noexcept
    {
        return *reinterpret_cast<const T*>(static_cast<const Byte*>(base) + offset);
    }

    /// <summary>
    /// 按字节偏移写入游戏对象字段
    ///
    /// 只在事件的前置回调中写入才有意义, 后置写入会被下一帧游戏逻辑覆盖
    /// </summary>
    /// <param name="base">游戏对象地址, 来自视图的 runtime 字段</param>
    /// <param name="offset">相对对象起始地址的偏移</param>
    /// <param name="value">写入值</param>
    template <typename T> void Write(void* base, Offset offset, T value) noexcept
    {
        *reinterpret_cast<T*>(static_cast<Byte*>(base) + offset) = value;
    }
}
