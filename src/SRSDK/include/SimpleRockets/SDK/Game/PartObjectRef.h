#pragma once

#include "SimpleRockets/SDK/Game/Memory.h"
#include "SimpleRockets/SDK/Game/Layout/Offsets.h"
#include "SimpleRockets/SDK/Game/Layout/Rvas.h"
#include "SimpleRockets/SDK/Game/PartObjectKind.h"
#include "SimpleRockets/SDK/Game/Ref.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>
    /// 飞船零件的公共视图
    ///
    /// 具体实现类随所属零件树而定, 用 IsType 区分油箱与引擎等
    /// </summary>
    struct PartObjectRef : Ref
    {
        explicit PartObjectRef(void* pointer) noexcept : Ref{pointer} { }

        /// <summary>是否为指定的零件实现类</summary>
        Boolean IsType(PartObjectKind kind) const noexcept
        {
            const void* tree = Read<void*>(pointer, Layout::OffsetPartObjectPartTree);
            if (tree == nullptr)
            {
                return false;
            }

            // 两者都是模块内绝对地址, 相减后模块基址相消, 只剩虚表 Rva 之差
            const Rva vtable = Read<Rva>(pointer, 0);
            const Rva treeVtable = Read<Rva>(tree, 0);
            return vtable - treeVtable == static_cast<Rva>(kind) - Layout::RvaVtablePartTree;
        }
    };
}
