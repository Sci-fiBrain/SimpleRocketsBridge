#pragma once

#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Game::Layout
{
    /// <summary>StateButton, 取自删除析构 0x448C0 的释放尺寸</summary>
    constexpr ObjectSize SizeofStateButton = 0xB8;

    /// <summary>按钮回调委托对象, 取自删除析构 0x26980 的释放尺寸</summary>
    constexpr ObjectSize SizeofCallback = 0x0C;
}
