#pragma once

#include "SimpleRockets/SDK/Environment.h"
#include "SimpleRockets/SDK/Game/LayerRef.h"
#include "SimpleRockets/SDK/Game/Layout/Offsets.h"
#include "SimpleRockets/SDK/Game/Layout/Rvas.h"
#include "SimpleRockets/SDK/Game/Memory.h"
#include "SimpleRockets/SDK/Game/Types.h"
#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>
    /// 飞行视图层, 取自 RuntimeLayerRef::GetShipView
    ///
    /// 全屏覆盖 HUD, 每帧跟着玩家的舱体移动相机, 屏幕坐标与世界坐标的换算也由它提供
    /// </summary>
    struct ShipViewRef : LayerRef
    {
        explicit ShipViewRef(void* pointer) noexcept : LayerRef{pointer} { }

        /// <summary>把屏幕像素换算成世界坐标</summary>
        /// <param name="x">屏幕像素横坐标</param>
        /// <param name="y">屏幕像素纵坐标</param>
        /// <returns>相对星球原点的世界坐标, 与零件拾取用的是同一个坐标系</returns>
        [[nodiscard]] Vector2D ScreenToWorld(Int32 x, Int32 y) const noexcept
        {
            using ScreenToWorldFunction = void(__fastcall*)(void* self, void* edx, Vector2D* world, Int32 x, Int32 y);

            Vector2D world{};
            Environment::GetAddress<ScreenToWorldFunction>(Layout::RvaShipViewScreenToWorld)(pointer, nullptr, &world,
                x, y);
            return world;
        }
    };
}
