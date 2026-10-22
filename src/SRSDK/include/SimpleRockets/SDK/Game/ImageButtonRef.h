#pragma once

#include "SimpleRockets/SDK/Game/ButtonRef.h"
#include "SimpleRockets/SDK/Game/Layout/Offsets.h"
#include "SimpleRockets/SDK/Game/Layout/VtableSlots.h"
#include "SimpleRockets/SDK/Game/Memory.h"
#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>
    /// 游戏 UI 的 ImageButton 视图
    ///
    /// 带常态 / 按下 / 选中三张帧图, 选中态决定绘制时用哪一张
    /// </summary>
    struct ImageButtonRef : ButtonRef
    {
        explicit ImageButtonRef(void* pointer) noexcept : ButtonRef{pointer} { }

        /// <summary>选中态标志, 决定绘制时用 ActionButtonSelected.png 还是 ActionButton.png</summary>
        [[nodiscard]] Boolean GetSelected() const noexcept
        {
            return Read<Boolean>(pointer, Layout::OffsetImageButtonSelected);
        }

        /// <summary>设置选中态, 影响绘制用的帧图; StateButton 覆写后还会改文字颜色</summary>
        void SetSelected(Boolean selected) const noexcept
        {
            using SetSelectedFunction = void(__fastcall*)(void* self, void* edx, Boolean selected);
            reinterpret_cast<SetSelectedFunction>(GetSlotFunction(Layout::VtableSlotImageButtonSetSelected))(pointer, nullptr, selected);
        }
    };
}
