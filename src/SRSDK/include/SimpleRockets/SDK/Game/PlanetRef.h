#pragma once

#include "SimpleRockets/SDK/Game/OrbitNodeRef.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>
    /// 注释待完善
    /// </summary>
    struct PlanetRef : OrbitNodeRef
    {
        explicit PlanetRef(void* pointer) noexcept : OrbitNodeRef{pointer} { }
    };
}
