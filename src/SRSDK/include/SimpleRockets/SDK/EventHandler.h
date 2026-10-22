#pragma once

#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK
{
    /// <summary>事件参数的占位基类, 不含任何数据; 有参数的派生类继承它</summary>
    struct EventArgs { };

    /// <summary>每帧事件的参数</summary>
    struct FrameEventArgs : EventArgs
    {
        /// <summary>上一帧到本帧的真实间隔, 秒; 乘时间倍率得到仿真步长</summary>
        Double DeltaSeconds;

        explicit FrameEventArgs(Double deltaSeconds) noexcept : DeltaSeconds{deltaSeconds} { }
    };

    /// <summary>
    /// 事件回调签名
    ///
    /// sender 为事件源; args 的 const 性表示该事件是否可拦截, 目前全部为只读
    /// </summary>
    template <typename TSender, typename TArgs>
    using EventHandler = void (*)(TSender sender, TArgs args) noexcept;
}
