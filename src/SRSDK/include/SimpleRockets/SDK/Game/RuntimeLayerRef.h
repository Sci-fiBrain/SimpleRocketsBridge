#pragma once

#include <algorithm>

#include "SimpleRockets/SDK/Game/LayerRef.h"
#include "SimpleRockets/SDK/Game/Layout/Rvas.h"
#include "SimpleRockets/SDK/Game/Memory.h"
#include "SimpleRockets/SDK/Game/NavCircleRef.h"
#include "SimpleRockets/SDK/Game/ShipViewRef.h"
#include "SimpleRockets/SDK/Game/StateButtonRef.h"
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
        /// <summary>HUD 内容区离层边缘的间距</summary>
        static constexpr Int32 ContentMargin = 2;

        explicit RuntimeLayerRef(void* pointer) noexcept : LayerRef{pointer} { }

        /// <summary>HUD 内容区的上沿纵坐标</summary>
        [[nodiscard]] Int32 GetContentTop() const noexcept
        {
            const Int32 layerRowTop = GetHeight() - ContentMargin;
            const StateButtonRef map{Read<void*>(pointer, Layout::OffsetRuntimeLayerMapButton)};

            if (map.IsNull())
            {
                return layerRowTop;
            }

            return std::min(layerRowTop, map.GetY() + map.GetHeight());
        }

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
