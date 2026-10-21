#pragma once

#include "SimpleRockets/SDK/Game/Offsets.h"
#include "SimpleRockets/SDK/Game/Ref.h"
#include "SimpleRockets/SDK/Game/Types.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>
    /// 注释待完善
    /// </summary>
    struct OrbitNodeRef : Ref
    {
        explicit OrbitNodeRef(void* pointer) noexcept : Ref{pointer} { }

        /// <summary>依次访问子节点</summary>
        template <typename TVisitor> void ForEachChild(TVisitor&& visit) const
        {
            const cfw::List<void*>* children = reinterpret_cast<const cfw::List<void*>*>(
                static_cast<const Byte*>(pointer) + OffsetOrbitNodeChildren);
            for (void** item = children->begin; item != children->end; ++item)
            {
                visit(OrbitNodeRef{*item});
            }
        }
    };
}
