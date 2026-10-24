#pragma once

#include "SimpleRockets/SDK/Environment.h"
#include "SimpleRockets/SDK/Game/Memory.h"
#include "SimpleRockets/SDK/Game/Layout/Offsets.h"
#include "SimpleRockets/SDK/Game/Layout/Rvas.h"
#include "SimpleRockets/SDK/Game/PartObjectRef.h"
#include "SimpleRockets/SDK/Game/Ref.h"
#include "SimpleRockets/SDK/Game/Types.h"
#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Game
{
    struct PartConnectionRef;

    /// <summary>
    /// 零件树的节点, 每个节点挂一个零件
    ///
    /// 连接边同时存放在两端节点的列表里, 遍历时按父端过滤
    /// </summary>
    struct PartTreeRef : Ref
    {
        explicit PartTreeRef(void* pointer) noexcept : Ref{pointer} { }

        /// <summary>本树的零件</summary>
        PartObjectRef GetPart() const noexcept
        {
            return PartObjectRef{Read<void*>(pointer, Layout::OffsetPartTreePart)};
        }

        /// <summary>依次访问以本树为父的连接边</summary>
        template <typename TVisitor> void ForEachChildConnection(TVisitor&& visit) const;

        /// <summary>找出包含该世界坐标的最深零件树</summary>
        /// <param name="worldPoint">世界坐标, 与 ShipViewRef::ScreenToWorld 的输出同一坐标系</param>
        /// <param name="useTreeBounds">true 时用整棵树的包围盒判定, false 时用零件自身范围判定</param>
        /// <returns>命中的树; 没有命中时返回空视图</returns>
        [[nodiscard]] PartTreeRef FindAtPoint(const Vector2D& worldPoint, Boolean useTreeBounds) const noexcept
        {
            using FindAtPointFunction = void*(__fastcall*)(void* self, void* edx, const Vector2D* worldPoint,
                Int32 useTreeBounds);
            return PartTreeRef{Environment::GetAddress<FindAtPointFunction>(Layout::RvaPartTreeFindAtPoint)(
                pointer, nullptr, &worldPoint, useTreeBounds)};
        }
    };

    /// <summary>
    /// 两个零件树之间的连接边, 同时存放在父端与子端的列表里
    /// </summary>
    struct PartConnectionRef : Ref
    {
        explicit PartConnectionRef(void* pointer) noexcept : Ref{pointer} { }

        /// <summary>连接的父零件树</summary>
        PartTreeRef GetParentTree() const noexcept
        {
            return PartTreeRef{Read<void*>(pointer, Layout::OffsetPartConnectionParentTree)};
        }

        /// <summary>连接的子零件树</summary>
        PartTreeRef GetChildTree() const noexcept
        {
            return PartTreeRef{Read<void*>(pointer, Layout::OffsetPartConnectionChildTree)};
        }
    };

    template <typename TVisitor> void PartTreeRef::ForEachChildConnection(TVisitor&& visit) const
    {
        const void* self = pointer;
        const cfw::List<void*>* connections = reinterpret_cast<const cfw::List<void*>*>(
            static_cast<const Byte*>(pointer) + Layout::OffsetPartTreeConnections);
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
