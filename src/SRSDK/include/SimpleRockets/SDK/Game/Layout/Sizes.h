#pragma once

#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Game::Layout
{
    /// <summary>cfw::Layer 基类, 取自 cfw::Layer 构造器 0x4128B0 写的最后一个字段 +0x5C</summary>
    constexpr ObjectSize SizeofLayer = 0x60;

    /// <summary>StateButton, 取自删除析构 0x448C0 的释放尺寸</summary>
    constexpr ObjectSize SizeofStateButton = 0xB8;

    /// <summary>按钮回调委托对象, 取自删除析构 0x26980 的释放尺寸</summary>
    constexpr ObjectSize SizeofCallback = 0x0C;

    /// <summary>cfw::Rectangle, 取自 FrameLayer 构造器分配时传入的尺寸</summary>
    constexpr ObjectSize SizeofRectangle = 0x48;

    /// <summary>cfw::Frame, 取自 FrameLayer 析构释放帧时传入的尺寸</summary>
    constexpr ObjectSize SizeofFrame = 0x5C;

    /// <summary>cfw::StackPanel, 比 cfw::Layer 多两个 Int32 字段</summary>
    constexpr ObjectSize SizeofStackPanel = 0x68;

    /// <summary>cfw::Label, 继承 cfw::Button</summary>
    constexpr ObjectSize SizeofLabel = 0xD8;
}
