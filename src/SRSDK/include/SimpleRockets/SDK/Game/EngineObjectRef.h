#pragma once

#include "SimpleRockets/SDK/Game/PartObjectRef.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>引擎零件, 尾焰是两个矩形, 停用后不再由游戏更新</summary>
    struct EngineObjectRef final : PartObjectRef
    {
        explicit EngineObjectRef(void* pointer) noexcept : PartObjectRef{pointer} { }

        /// <summary>启用或停用引擎, 停用时清掉尾焰目标尺寸与矩形本身</summary>
        void SetEnabled(Boolean enabled) const noexcept override
        {
            if (enabled)
            {
                PartObjectRef::SetEnabled(true);
            }
            else
            {
                PartObjectRef::SetEnabled(false);

                Write<Single>(pointer, Layout::OffsetPartObjectEngineFlameWidth, 0.0F);
                Write<Single>(pointer, Layout::OffsetPartObjectEngineFlameHeight, 0.0F);

                ClearFlame(pointer, Layout::OffsetPartObjectEngineFlame);
                ClearFlame(pointer, Layout::OffsetPartObjectEngineFlameBackup);
            }
        }

    private:
        /// <summary>把尾焰矩形缩到零并置为全透明</summary>
        static void ClearFlame(void* part, Offset rectOffset) noexcept
        {
            void* rect = Read<void*>(part, rectOffset);
            if (rect == nullptr)
            {
                return;
            }

            Write<Single>(rect, Layout::OffsetRectangleWidth, 0.0F);
            Write<Single>(rect, Layout::OffsetRectangleHeight, 0.0F);
            Write<Byte>(rect, Layout::OffsetRectangleInnerColorAlpha, 0);
        }
    };
}
