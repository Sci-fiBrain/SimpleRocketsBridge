#pragma once

#include <algorithm>

#include "SimpleRockets/SDK/Game/Memory.h"
#include "SimpleRockets/SDK/Game/Layout/Offsets.h"
#include "SimpleRockets/SDK/Game/PartObjectRef.h"
#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>
    /// 指挥舱, 每艘船最多一个
    ///
    /// 分级索引与油门由它持有
    /// </summary>
    struct PodObjectRef : PartObjectRef
    {
        explicit PodObjectRef(void* pointer) noexcept : PartObjectRef{pointer} { }

        /// <summary>油门, 取值 0..1</summary>
        Double GetThrottle() const noexcept
        {
            return Read<Double>(pointer, Layout::OffsetPodObjectThrottle);
        }

        /// <summary>设置油门, 取值 0..1, 超出范围取值会被夹到边界</summary>
        void SetThrottle(Double value) noexcept
        {
            Write<Double>(pointer, Layout::OffsetPodObjectThrottle, std::clamp(value, 0.0, 1.0));
        }

        /// <summary>当前分级索引</summary>
        Int32 GetStageIndex() const noexcept
        {
            return Read<Int32>(pointer, Layout::OffsetPodObjectStageIndex);
        }
    };
}
