#pragma once

#include "SimpleRockets/SDK/Game/PartObjectRef.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>降落伞零件</summary>
    struct ParachuteObjectRef final : PartObjectRef
    {
        explicit ParachuteObjectRef(void* pointer) noexcept : PartObjectRef{pointer} { }

        /// <summary>启用或停用降落伞, 停用时清掉部署缓存</summary>
        void SetEnabled(Boolean enabled) const noexcept override
        {
            if (enabled)
            {
                PartObjectRef::SetEnabled(true);
            }
            else
            {
                Write<Boolean>(pointer, Layout::OffsetPartObjectActivated, 0);
                Write<Boolean>(pointer, Layout::OffsetPartObjectDeployed, 0);
                Write<Boolean>(pointer, Layout::OffsetPartObjectInflate, 0);
                Write<Boolean>(pointer, Layout::OffsetPartObjectRope, 0);
                Write<Int32>(pointer, Layout::OffsetPartObjectChuteData, 0);
                Write<Int32>(pointer, Layout::OffsetPartObjectChute, 0);
            }
        }
    };
}
