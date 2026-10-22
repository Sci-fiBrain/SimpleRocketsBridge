#pragma once

#include <algorithm>

#include "SimpleRockets/SDK/Game/Memory.h"
#include "SimpleRockets/SDK/Game/Layout/Offsets.h"
#include "SimpleRockets/SDK/Game/PartObjectRef.h"
#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>
    /// 油箱零件
    ///
    /// 燃料写入后由游戏在下一帧汇总到全船燃料比
    /// </summary>
    struct TankObjectRef : PartObjectRef
    {
        explicit TankObjectRef(void* pointer) noexcept : PartObjectRef{pointer} { }

        /// <summary>当前燃料</summary>
        Double GetFuel() const noexcept
        {
            return Read<Double>(pointer, Layout::OffsetTankObjectFuel);
        }

        /// <summary>设置当前燃料, 超出 0..容量 的取值会被夹到边界</summary>
        void SetFuel(Double value) noexcept
        {
            Write<Double>(pointer, Layout::OffsetTankObjectFuel, std::clamp(value, 0.0, GetCapacity()));
        }

        /// <summary>燃料容量</summary>
        Double GetCapacity() const noexcept
        {
            return Read<Double>(pointer, Layout::OffsetTankObjectCapacity);
        }

        /// <summary>满油质量</summary>
        Double GetFullMass() const noexcept
        {
            return Read<Double>(pointer, Layout::OffsetTankObjectFullMass);
        }

        /// <summary>干重</summary>
        Double GetDryMass() const noexcept
        {
            return Read<Double>(pointer, Layout::OffsetTankObjectDryMass);
        }
    };
}
