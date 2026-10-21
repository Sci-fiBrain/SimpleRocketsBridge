#pragma once

#include "SimpleRockets/SDK/Game/Ref.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>
    /// 注释待完善
    /// </summary>
    struct OrbitRef : Ref
    {
        explicit OrbitRef(void* pointer) noexcept : Ref{pointer} { }
    };
}
