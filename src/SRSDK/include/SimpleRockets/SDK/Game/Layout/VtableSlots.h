#pragma once

#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Game::Layout
{
    /// <summary>cfw::Layer 声明的 InsertChild, 参数 (子层, 索引)</summary>
    constexpr VtableSlot VtableSlotLayerInsertChild = 5;

    /// <summary>cfw::Layer 声明的 SetVisible, 参数 (是否可见)</summary>
    constexpr VtableSlot VtableSlotLayerSetVisible = 9;

    /// <summary>cfw::Layer 声明、StateButton 覆写的 SetSize</summary>
    constexpr VtableSlot VtableSlotLayerSetSize = 13;

    /// <summary>cfw::Layer 声明的 SetPosition</summary>
    constexpr VtableSlot VtableSlotLayerSetPosition = 16;

    /// <summary>cfw::Button 声明的点击分发</summary>
    constexpr VtableSlot VtableSlotButtonDispatchClick = 33;

    /// <summary>cfw::ImageButton 声明、StateButton 覆写的 SetSelected</summary>
    constexpr VtableSlot VtableSlotImageButtonSetSelected = 34;
}
