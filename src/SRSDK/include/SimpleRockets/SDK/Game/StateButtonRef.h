#pragma once

#include "SimpleRockets/SDK/Game/ImageButtonRef.h"
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

    };
}
