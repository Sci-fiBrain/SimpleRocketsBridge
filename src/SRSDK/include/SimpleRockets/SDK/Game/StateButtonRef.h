#pragma once

#include "SimpleRockets/SDK/Game/ImageButtonRef.h"
#include "SimpleRockets/SDK/Game/LabelRef.h"
#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>
    /// 游戏 UI 的 StateButton 视图
    ///
    /// 在 ImageButton 之上多一个文字标签, 自带 ActionButton.png 外观, 构造时尺寸取自贴图
    /// </summary>
    struct StateButtonRef : ImageButtonRef
    {
        explicit StateButtonRef(void* pointer) noexcept : ImageButtonRef{pointer} { }

        /// <summary>按钮内部的文字标签</summary>
        /// <remarks>SetSelected 会改这个标签的颜色, 需要固定颜色时在它之后写</remarks>
        [[nodiscard]] LabelRef GetLabel() const noexcept
        {
            return LabelRef{Read<void*>(pointer, Layout::OffsetStateButtonLabel)};
        }
    };
}
