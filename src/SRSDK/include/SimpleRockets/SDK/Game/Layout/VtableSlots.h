#pragma once

#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Game::Layout
{
    /// <summary>cfw::Layer 声明的 InsertChild, 参数 (子层, 索引)</summary>
    constexpr VtableSlot VtableSlotLayerInsertChild = 5;

    /// <summary>cfw::Layer 声明、StackPanel 覆写的 AddChild, 参数 (子层)</summary>
    constexpr VtableSlot VtableSlotLayerAddChild = 6;

    /// <summary>cfw::Layer 声明的 SetVisible, 参数 (是否可见)</summary>
    constexpr VtableSlot VtableSlotLayerSetVisible = 9;

    /// <summary>cfw::Layer 声明的 HandleTouch, 参数 (触摸事件)</summary>
    constexpr VtableSlot VtableSlotLayerHandleTouch = 24;

    /// <summary>cfw::Layer 声明、StateButton 覆写的 SetSize</summary>
    constexpr VtableSlot VtableSlotLayerSetSize = 13;

    /// <summary>cfw::Layer 声明的 SetPosition</summary>
    constexpr VtableSlot VtableSlotLayerSetPosition = 16;

    /// <summary>cfw::Button 声明的点击分发</summary>
    constexpr VtableSlot VtableSlotButtonDispatchClick = 33;

    /// <summary>cfw::ImageButton 声明、StateButton 覆写的 SetSelected</summary>
    constexpr VtableSlot VtableSlotImageButtonSetSelected = 34;

    /// <summary>PartObject 的激活回调, 派生类可覆写</summary>
    constexpr VtableSlot VtableSlotPartObjectActivate = 23;

    /// <summary>PartObject 的停用回调, 只有 LanderObject 覆写</summary>
    constexpr VtableSlot VtableSlotPartObjectDeactivate = 24;

    /// <summary>PartObject 的置激活入口, 游戏点火走这里而不是槽 23</summary>
    constexpr VtableSlot VtableSlotPartObjectSetActivated = 17;
}
