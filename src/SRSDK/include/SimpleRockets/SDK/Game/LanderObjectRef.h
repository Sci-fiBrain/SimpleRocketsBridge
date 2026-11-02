#pragma once

#include "SimpleRockets/SDK/Game/PartObjectRef.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>起落架零件, 展开与收起各有一个状态位</summary>
    struct LanderObjectRef final : PartObjectRef
    {
        explicit LanderObjectRef(void* pointer) noexcept : PartObjectRef{pointer} { }

        /// <summary>启用或停用起落架; 展开由激活触发, 收回走状态机</summary>
        /// <param name="enabled">true 为展开, false 为收起</param>
        void SetEnabled(Boolean enabled) const noexcept override
        {
            if (enabled)
            {
                Write<Boolean>(pointer, Layout::OffsetPartObjectRaise, 0);
                PartObjectRef::SetEnabled(true);
            }
            else
            {
                Write<Boolean>(pointer, Layout::OffsetPartObjectLanderStageDone, 0);
                Write<Boolean>(pointer, Layout::OffsetPartObjectLower, 0);
                Write<Boolean>(pointer, Layout::OffsetPartObjectRaise, 1);
                Write<Boolean>(pointer, Layout::OffsetPartObjectActivated, 0);
            }
        }
    };
}
