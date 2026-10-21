#pragma once

#include "SimpleRockets/SDK/Game/Memory.h"
#include "SimpleRockets/SDK/Game/Offsets.h"
#include "SimpleRockets/SDK/Game/PartTreeRef.h"
#include "SimpleRockets/SDK/Game/PodObjectRef.h"
#include "SimpleRockets/SDK/Game/Ref.h"
#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>
    /// 注释待完善
    /// </summary>
    struct ShipRef : Ref
    {
        explicit ShipRef(void* pointer) noexcept : Ref{pointer} { }

        /// <summary>根零件树</summary>
        PartTreeRef GetPartTree() const noexcept
        {
            return PartTreeRef{Read<void*>(pointer, OffsetShipPartTree)};
        }

        /// <summary>指挥舱, 可能为空</summary>
        PodObjectRef GetPodObject() const noexcept
        {
            return PodObjectRef{Read<void*>(pointer, OffsetShipPodObject)};
        }

        /// <summary>燃料比例, 0..1; 只统计给活跃发动机供油的油箱</summary>
        Double GetFuelRatio() const noexcept
        {
            return Read<Double>(pointer, OffsetShipFuelRatio);
        }

        /// <summary>依次访问本船的全部零件</summary>
        template <typename TVisitor> void ForEachPart(TVisitor&& visit) const
        {
            ForEachPartTree(PartTreeRef{Read<void*>(pointer, OffsetShipPartTree)}, visit);
        }

    private:
        template <typename TVisitor> static void ForEachPartTree(PartTreeRef tree, TVisitor& visit)
        {
            if (tree.pointer == nullptr)
            {
                return;
            }

            visit(tree.GetPart());

            tree.ForEachChildConnection([&visit](PartConnectionRef connection)
                { ForEachPartTree(connection.GetChildTree(), visit); });
        }
    };
}
