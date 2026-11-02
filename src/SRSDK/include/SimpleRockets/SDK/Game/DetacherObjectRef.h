#pragma once

#include "SimpleRockets/SDK/Game/PartObjectRef.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>分离器零件, 基类 SetEnabled 即触发分离</summary>
    struct DetacherObjectRef final : PartObjectRef
    {
        explicit DetacherObjectRef(void* pointer) noexcept : PartObjectRef{pointer} { }
    };
}
