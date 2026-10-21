#pragma once

#include "SimpleRockets/SDK/Game/Ref.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>
    /// 注释待完善
    /// </summary>
    struct RuntimeLayerRef : Ref
    {
        explicit RuntimeLayerRef(void* pointer) noexcept : Ref{pointer} { }
    };
}
