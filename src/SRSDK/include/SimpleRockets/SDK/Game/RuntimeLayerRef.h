#pragma once

#include "SimpleRockets/SDK/Game/LayerRef.h"
#include "SimpleRockets/SDK/Game/Layout/Rvas.h"
#include "SimpleRockets/SDK/Game/Memory.h"
#include "SimpleRockets/SDK/Game/NavCircleRef.h"
#include "SimpleRockets/SDK/Game/ShipViewRef.h"
#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>
    /// 运行时 HUD 的根层
    ///
    /// 由游戏在场景加载时构造, 内含相机、油门与分级控件、航向圈与六个状态按钮
    /// </summary>
    struct RuntimeLayerRef : LayerRef
    {
        explicit RuntimeLayerRef(void* pointer) noexcept : LayerRef{pointer} { }

        /// <summary>飞行视图层, 提供屏幕坐标与世界坐标的换算</summary>
        [[nodiscard]] ShipViewRef GetShipView() const noexcept
        {
            return ShipViewRef{Read<void*>(pointer, Layout::OffsetRuntimeLayerShipView)};
        }

        /// <summary>航向锁定控制器</summary>
        [[nodiscard]] NavCircleRef GetNavCircle() const noexcept
        {
            return NavCircleRef{Read<void*>(pointer, Layout::OffsetRuntimeLayerNavCircle)};
        }
    };
}
