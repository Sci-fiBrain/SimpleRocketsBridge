#pragma once

#include <string>

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

        virtual ~PartObjectRef() = default;

        /// <summary>零件显示名, 即建造界面里给玩家看的名字</summary>
        [[nodiscard]] std::string GetDisplayName() const noexcept
        {
            return Read<std::string>(pointer, Layout::OffsetPartObjectDisplayName);
        }

        /// <summary>是否被高亮, 游戏自身的点选会写这个字节</summary>
        [[nodiscard]] Boolean GetHighlighted() const noexcept
        {
            return Read<Boolean>(pointer, Layout::OffsetPartObjectHighlighted);
        }

        /// <summary>设置高亮</summary>
        void SetHighlighted(Boolean highlighted) const noexcept
        {
            Write<Boolean>(pointer, Layout::OffsetPartObjectHighlighted, highlighted);
        }

        /// <summary>激活状态</summary>
        [[nodiscard]] Boolean GetActivated() const noexcept
        {
            return Read<Boolean>(pointer, Layout::OffsetPartObjectActivated);
        }

        /// <summary>设置激活状态</summary>
        void SetActivated(Boolean activated) const noexcept
        {
            if (GetActivated() == activated)
            {
                return;
            }

            if (activated)
            {
                using SetActivatedFunction = void(__fastcall*)(void* self, void* edx);

                reinterpret_cast<SetActivatedFunction>(GetSlotFunction(Layout::VtableSlotPartObjectSetActivated))(
                    pointer, nullptr);
            }
            else
            {
                using DeactivateFunction = void(__fastcall*)(void* self, void* edx);

                Write<Boolean>(pointer, Layout::OffsetPartObjectActivated, 0);
                reinterpret_cast<DeactivateFunction>(GetSlotFunction(Layout::VtableSlotPartObjectDeactivate))(
                    pointer, nullptr);
            }
        }

        /// <summary>启用或停用零件</summary>
        virtual void SetEnabled(Boolean enabled) const noexcept
        {
            SetActivated(enabled);
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
