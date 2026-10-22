#pragma once

#include "SimpleRockets/SDK/Game/OrbitNodeRef.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>
    /// 天体节点, 轨道树的根与各行星都是它
    /// </summary>
    struct PlanetRef : OrbitNodeRef
    {
        explicit PlanetRef(void* pointer) noexcept : OrbitNodeRef{pointer} { }
    };
}
