#pragma once

#include <cstdint>
#include <limits>

/// <summary>
/// 基础别名与常量
///
/// 本命名空间只允许出现类型别名与 constexpr 常量; 任何类、函数、接口一律进 SimpleRockets::SDK
/// 下按功能域划分的子命名空间
/// </summary>
namespace SimpleRockets
{
    using Int8 = std::int8_t;
    using UInt8 = std::uint8_t;
    using Byte = std::uint8_t;
    using Int16 = std::int16_t;
    using UInt16 = std::uint16_t;
    using Int32 = std::int32_t;
    using UInt32 = std::uint32_t;
    using Int64 = std::int64_t;
    using UInt64 = std::uint64_t;
    using Boolean = UInt8;
    using Single = float;
    using Double = double;

    /// <summary>相对模块加载基址的偏移</summary>
    using Rva = UInt32;

    /// <summary>相对对象起始地址的字段偏移</summary>
    using Offset = UInt32;

    static_assert(sizeof(Boolean) == 1, "Boolean 必须与游戏内 bool 同宽 (1 字节)");
    static_assert(std::numeric_limits<Single>::is_iec559 && sizeof(Single) == 4, "Single 必须为 IEEE 754 binary32");
    static_assert(std::numeric_limits<Double>::is_iec559 && sizeof(Double) == 8, "Double 必须为 IEEE 754 binary64");

}
