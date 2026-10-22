#pragma once

#include "SimpleRockets/SDK/Game/Memory.h"
#include "SimpleRockets/SDK/Game/Layout/Offsets.h"
#include "SimpleRockets/SDK/Game/OrbitNodeRef.h"
#include "SimpleRockets/SDK/Game/OrbitRef.h"
#include "SimpleRockets/SDK/Game/PlanetRef.h"
#include "SimpleRockets/SDK/Game/ShipRef.h"
#include "SimpleRockets/SDK/Game/Types.h"
#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>
    /// 飞船在轨道树上的节点
    ///
    /// 位置与速度相对行星中心, 由物理每帧写入; 轨道根数每帧重建
    /// </summary>
    struct ShipOrbitNodeRef : OrbitNodeRef
    {
        explicit ShipOrbitNodeRef(void* pointer) noexcept : OrbitNodeRef{pointer} { }

        /// <summary>节点当前所在的行星</summary>
        PlanetRef GetPlanet() const noexcept
        {
            return PlanetRef{Read<void*>(pointer, Layout::OffsetShipOrbitNodePlanet)};
        }

        /// <summary>节点对应的飞船</summary>
        ShipRef GetShip() const noexcept
        {
            return ShipRef{Read<void*>(pointer, Layout::OffsetShipOrbitNodeShip)};
        }

        /// <summary>轨道根数</summary>
        OrbitRef GetOrbit() const noexcept
        {
            return OrbitRef{Read<void*>(pointer, Layout::OffsetShipOrbitNodeOrbit)};
        }

        /// <summary>速度, 米每秒</summary>
        Vector2D GetVelocity() const noexcept
        {
            return Read<Vector2D>(pointer, Layout::OffsetShipOrbitNodeVelocity);
        }

        /// <summary>节点身份编号</summary>
        Int32 GetShipId() const noexcept
        {
            return Read<Int32>(pointer, Layout::OffsetShipOrbitNodeShipId);
        }

        /// <summary>位置, 相对行星中心, 米</summary>
        Vector2D GetPosition() const noexcept
        {
            return Read<Vector2D>(pointer, Layout::OffsetShipOrbitNodePosition);
        }

        /// <summary>轨道根数是否有效</summary>
        Boolean GetOrbitValid() const noexcept
        {
            return Read<Boolean>(pointer, Layout::OffsetShipOrbitNodeOrbitValid);
        }
    };
}
