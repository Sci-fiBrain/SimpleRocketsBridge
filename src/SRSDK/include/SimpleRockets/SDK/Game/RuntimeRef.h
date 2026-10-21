#pragma once

#include "SimpleRockets/SDK/Game/Memory.h"
#include "SimpleRockets/SDK/Game/Offsets.h"
#include "SimpleRockets/SDK/Game/OrbitNodeRef.h"
#include "SimpleRockets/SDK/Game/PlanetRef.h"
#include "SimpleRockets/SDK/Game/Ref.h"
#include "SimpleRockets/SDK/Game/RuntimeLayerRef.h"
#include "SimpleRockets/SDK/Game/ShipOrbitNodeRef.h"
#include "SimpleRockets/SDK/Game/Types.h"
#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>
    /// 注释待完善
    /// </summary>
    struct RuntimeRef : Ref
    {
        explicit RuntimeRef(void* pointer) noexcept : Ref{pointer} { }

        /// <summary>运行时的 UI 层</summary>
        RuntimeLayerRef GetLayer() const noexcept
        {
            return RuntimeLayerRef{Read<void*>(pointer, OffsetRuntimeLayer)};
        }

        /// <summary>玩家飞船的轨道节点</summary>
        ShipOrbitNodeRef GetPlayerNode() const noexcept
        {
            return ShipOrbitNodeRef{Read<void*>(pointer, OffsetRuntimePlayerNode)};
        }

        /// <summary>轨道树根节点</summary>
        OrbitNodeRef GetRootNode() const noexcept
        {
            return OrbitNodeRef{Read<void*>(pointer, OffsetRuntimeRootNode)};
        }

        /// <summary>玩家飞船当前所在行星的轨道节点</summary>
        PlanetRef GetPrimaryNode() const noexcept
        {
            return PlanetRef{Read<void*>(pointer, OffsetRuntimePrimaryNode)};
        }

        /// <summary>任务时间, 秒</summary>
        Double GetElapsedTime() const noexcept
        {
            return Read<Double>(pointer, OffsetRuntimeElapsedTime);
        }

        /// <summary>坐标系原点</summary>
        Vector2D GetLocalCenter() const noexcept
        {
            return Read<Vector2D>(pointer, OffsetRuntimeLocalCenter);
        }

        /// <summary>玩家飞船速度, 米/秒</summary>
        Vector2D GetVelocity() const noexcept
        {
            return Read<Vector2D>(pointer, OffsetRuntimeVelocity);
        }

        /// <summary>玩家飞船姿态角, 弧度</summary>
        Double GetAttitude() const noexcept
        {
            return Read<Double>(pointer, OffsetRuntimeAttitude);
        }

        /// <summary>速度模长, 米/秒</summary>
        Double GetSpeed() const noexcept
        {
            return Read<Double>(pointer, OffsetRuntimeSpeed);
        }

        /// <summary>角速度, 弧度/秒</summary>
        Double GetAngularRate() const noexcept
        {
            return Read<Double>(pointer, OffsetRuntimeAngularRate);
        }

        /// <summary>时间倍率</summary>
        Double GetTimeScale() const noexcept
        {
            return Read<Double>(pointer, OffsetRuntimeTimeScale);
        }

        /// <summary>时间倍率档位下标, 从 1 起</summary>
        Int32 GetTimeScaleLevel() const noexcept
        {
            return Read<Int32>(pointer, OffsetRuntimeTimeScaleLevel);
        }

        /// <summary>飞船高度, 相对行星中心, 米</summary>
        Double GetAltitude() const noexcept
        {
            return Read<Double>(pointer, OffsetRuntimeAltitude);
        }

        /// <summary>飞船离地高度, 米</summary>
        Double GetAltitudeAgl() const noexcept
        {
            return Read<Double>(pointer, OffsetRuntimeAltitudeAgl);
        }

        /// <summary>处于超级加速(warp)中</summary>
        Boolean GetInWarp() const noexcept
        {
            return Read<Boolean>(pointer, OffsetRuntimeInWarp);
        }

        /// <summary>已首次分级</summary>
        Boolean GetFirstStageActivated() const noexcept
        {
            return Read<Boolean>(pointer, OffsetRuntimeFirstStageActivated);
        }
    };
}
