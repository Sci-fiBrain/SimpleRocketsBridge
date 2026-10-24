#pragma once

#include <string>

#include "SimpleRockets/SDK/Environment.h"
#include "SimpleRockets/SDK/Game/ButtonRef.h"
#include "SimpleRockets/SDK/Game/Layout/Offsets.h"
#include "SimpleRockets/SDK/Game/Layout/Rvas.h"
#include "SimpleRockets/SDK/Game/Memory.h"
#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>文字在标签宽度内的排列方式</summary>
    enum class LabelTextAlign : Int32
    {
        /// <summary>从标签左边缘起画</summary>
        Start = 0,

        /// <summary>按文字自身宽度在标签内居中</summary>
        Center = 1,
    };

    /// <summary>文本标签, 继承 cfw::Button, 自身不处理点击</summary>
    struct LabelRef : ButtonRef
    {
        explicit LabelRef(void* pointer) noexcept : ButtonRef{pointer} { }

        /// <summary>按当前文本重算行布局, 行偏移在这一步算出</summary>
        void Relayout() const noexcept
        {
            using RelayoutFunction = void(__fastcall*)(void* self, void* edx);
            Environment::GetAddress<RelayoutFunction>(Layout::RvaLabelRelayout)(pointer, nullptr);
        }

        /// <summary>设置显示的文字, 并立即重排</summary>
        /// <param name="text">新文本</param>
        void SetText(const char* text) const noexcept
        {
            if (IsNull())
            {
                return;
            }

            Write<std::string>(pointer, Layout::OffsetLabelText, std::string{text});

            Relayout();
        }

        /// <summary>设置文字颜色</summary>
        /// <param name="color">ARGB 颜色</param>
        void SetTextColor(UInt32 color) const noexcept
        {
            if (IsNull())
            {
                return;
            }

            Write<UInt32>(pointer, Layout::OffsetLabelTextColor, color);
        }

        /// <summary>设置文字排列方式, 并立即重排</summary>
        /// <param name="align">排列方式</param>
        void SetTextAlign(LabelTextAlign align) const noexcept
        {
            if (IsNull())
            {
                return;
            }

            const Int32 flag = static_cast<Int32>(align);
            Write<Int32>(pointer, Layout::OffsetLabelCenterByText, flag);
            Write<Int32>(pointer, Layout::OffsetLabelCenterByTextShadow, flag);

            Relayout();
        }
    };
}
