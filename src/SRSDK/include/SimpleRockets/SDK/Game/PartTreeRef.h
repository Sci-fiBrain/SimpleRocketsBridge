#pragma once

#include "SimpleRockets/SDK/Game/Memory.h"
#include "SimpleRockets/SDK/Game/Offsets.h"
#include "SimpleRockets/SDK/Game/PartObjectRef.h"
#include "SimpleRockets/SDK/Game/Ref.h"
#include "SimpleRockets/SDK/Game/Types.h"
#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Game
{
    struct PartConnectionRef;

    /// <summary>
    /// 注释待完善
    /// </summary>
    struct PartTreeRef : Ref
    {
        explicit PartTreeRef(void* pointer) noexcept : Ref{pointer} { }

        /// <summary>本树的零件</summary>
        PartObjectRef GetPart() const noexcept
        {
            return PartObjectRef{Read<void*>(pointer, OffsetPartTreePart)};
        }

        /// <summary>依次访问以本树为父的连接边</summary>
        template <typename TVisitor> void ForEachChildConnection(TVisitor&& visit) const;
    };

    /// <summary>
    /// 注释待完善
    /// </summary>
    struct PartConnectionRef : Ref
    {
        explicit PartConnectionRef(void* pointer) noexcept : Ref{pointer} { }

        /// <summary>连接的父零件树</summary>
        PartTreeRef GetParentTree() const noexcept
        {
            return PartTreeRef{Read<void*>(pointer, OffsetPartConnectionParentTree)};
        }

        /// <summary>连接的子零件树</summary>
        PartTreeRef GetChildTree() const noexcept
        {
            return PartTreeRef{Read<void*>(pointer, OffsetPartConnectionChildTree)};
        }
    };

    template <typename TVisitor> void PartTreeRef::ForEachChildConnection(TVisitor&& visit) const
    {
        const void* self = pointer;
        const cfw::List<void*>* connections = reinterpret_cast<const cfw::List<void*>*>(
            static_cast<const Byte*>(pointer) + OffsetPartTreeConnections);
        for (void** item = connections->begin; item != connections->end; ++item)
        {
            PartConnectionRef connection{*item};
            if (connection.GetParentTree().pointer != self)
            {
                continue;
            }

            visit(connection);
        }
    }
}
