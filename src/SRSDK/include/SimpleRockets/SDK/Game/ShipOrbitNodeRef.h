#pragma once

#include "SimpleRockets/SDK/Game/Memory.h"
#include "SimpleRockets/SDK/Game/Offsets.h"
#include "SimpleRockets/SDK/Game/OrbitNodeRef.h"
#include "SimpleRockets/SDK/Game/OrbitRef.h"
#include "SimpleRockets/SDK/Game/PlanetRef.h"
#include "SimpleRockets/SDK/Game/ShipRef.h"
#include "SimpleRockets/SDK/Game/Types.h"
#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>
    /// 注释待完善
    /// </summary>
    struct ShipOrbitNodeRef : OrbitNodeRef
    {
        explicit ShipOrbitNodeRef(void* pointer) noexcept : OrbitNodeRef{pointer} { }

        /// <summary>节点当前所在的行星</summary>
        PlanetRef GetPlanet() const noexcept
        {
            return PlanetRef{Read<void*>(pointer, OffsetShipOrbitNodePlanet)};
        }

        /// <summary>节点对应的飞船</summary>
        ShipRef GetShip() const noexcept
        {
            return ShipRef{Read<void*>(pointer, OffsetShipOrbitNodeShip)};
        }

        /// <summary>轨道根数</summary>
        OrbitRef GetOrbit() const noexcept
        {
            return OrbitRef{Read<void*>(pointer, OffsetShipOrbitNodeOrbit)};
        }

        /// <summary>速度, 米每秒</summary>
        Vector2D GetVelocity() const noexcept
        {
            return Read<Vector2D>(pointer, OffsetShipOrbitNodeVelocity);
        }

        /// <summary>节点身份编号</summary>
        Int32 GetShipId() const noexcept
        {
            return Read<Int32>(pointer, OffsetShipOrbitNodeShipId);
        }

        /// <summary>位置, 相对行星中心, 米</summary>
        Vector2D GetPosition() const noexcept
        {
            return Read<Vector2D>(pointer, OffsetShipOrbitNodePosition);
        }

        /// <summary>轨道根数是否有效</summary>
        Boolean GetOrbitValid() const noexcept
        {
            return Read<Boolean>(pointer, OffsetShipOrbitNodeOrbitValid);
        }
    };
}
