#pragma once

#include "SimpleRockets/SDK/Game/PartObjectRef.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>太阳能板零件, 展开程度由自身的浮点字段决定</summary>
    struct SolarPanelObjectRef final : PartObjectRef
    {
        explicit SolarPanelObjectRef(void* pointer) noexcept : PartObjectRef{pointer} { }

        /// <summary>启用或停用太阳能板, 停用时清零展开量</summary>
        void SetEnabled(Boolean enabled) const noexcept override
        {
            if (enabled)
            {
                PartObjectRef::SetEnabled(true);
            }
            else
            {
                PartObjectRef::SetEnabled(false);
                Write<Single>(pointer, Layout::OffsetPartObjectSolarExtension, 0.0F);
            }
        }
    };
}
