#pragma once

#include "SimpleRockets/SDK/Game/Memory.h"
#include "SimpleRockets/SDK/Game/Layout/Offsets.h"
#include "SimpleRockets/SDK/Game/Layout/Rvas.h"
#include "SimpleRockets/SDK/Game/Layout/VtableSlots.h"
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

        /// <summary>是否被高亮, 游戏自身的点选会写这个字节</summary>
        [[nodiscard]] Boolean GetHighlighted() const noexcept
        {
            return Read<Boolean>(pointer, Layout::OffsetPartObjectHighlighted);
        }

        /// <summary>设置高亮, 零件在场景中的描边随之切换</summary>
        void SetHighlighted(Boolean highlighted) const noexcept
        {
            Write<Boolean>(pointer, Layout::OffsetPartObjectHighlighted, highlighted);
        }

        /// <summary>激活状态, 引擎据此决定是否产生推力</summary>
        [[nodiscard]] Boolean GetActivated() const noexcept
        {
            return Read<Boolean>(pointer, Layout::OffsetPartObjectActivated);
        }

        /// <summary>设置激活状态, 状态未变化时不做任何事</summary>
        /// <remarks>会触发派生类的激活或停用回调, 起落架的收起与放下只由停用回调驱动</remarks>
        void SetActivated(Boolean activated) const noexcept
        {
            if (GetActivated() == activated)
            {
                return;
            }

            if (activated != 0)
            {
                using ActivateFunction = void(__fastcall*)(void* self, void* edx);

                Write<Boolean>(pointer, Layout::OffsetPartObjectActivated, 1);
                reinterpret_cast<ActivateFunction>(GetSlotFunction(Layout::VtableSlotPartObjectActivate))(
                    pointer, nullptr);
                return;
            }

            using DeactivateFunction = void(__fastcall*)(void* self, void* edx);

            Write<Boolean>(pointer, Layout::OffsetPartObjectActivated, 0);
            reinterpret_cast<DeactivateFunction>(GetSlotFunction(Layout::VtableSlotPartObjectDeactivate))(
                pointer, nullptr);
        }

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
