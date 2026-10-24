#pragma once

#include "SimpleRockets/SDK/Game/Layout/Offsets.h"
#include "SimpleRockets/SDK/Game/Layout/VtableSlots.h"
#include "SimpleRockets/SDK/Game/Memory.h"
#include "SimpleRockets/SDK/Game/Ref.h"
#include "SimpleRockets/SDK/Game/Types.h"
#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>
    /// 游戏 UI 的 cfw::Layer 视图
    ///
    /// 所有 UI 元素的基类, 提供几何与子层树
    /// </summary>
    struct LayerRef : Ref
    {
        explicit LayerRef(void* pointer) noexcept : Ref{pointer} { }

        /// <summary>是否参与绘制, 为 false 时自身与子层都不画</summary>
        [[nodiscard]] Boolean GetVisible() const noexcept
        {
            return Read<Boolean>(pointer, Layout::OffsetLayerVisible);
        }

        /// <summary>设置可见性, 隐藏时一并清除指向本层的悬停缓存</summary>
        void SetVisible(Boolean visible) const noexcept
        {
            using SetVisibleFunction = void(__fastcall*)(void* self, void* edx, Int32 visible);
            reinterpret_cast<SetVisibleFunction>(GetSlotFunction(Layout::VtableSlotLayerSetVisible))(
                pointer, nullptr, visible);
        }

        /// <summary>层左上角在父层坐标系中的横坐标, 单位像素</summary>
        [[nodiscard]] Int32 GetX() const noexcept
        {
            return Read<Int32>(pointer, Layout::OffsetLayerX);
        }

        /// <summary>层左上角在父层坐标系中的纵坐标, 单位像素, 向上为正</summary>
        [[nodiscard]] Int32 GetY() const noexcept
        {
            return Read<Int32>(pointer, Layout::OffsetLayerY);
        }

        /// <summary>层宽度, 单位像素</summary>
        [[nodiscard]] Int32 GetWidth() const noexcept
        {
            return Read<Int32>(pointer, Layout::OffsetLayerWidth);
        }

        /// <summary>层高度, 单位像素</summary>
        [[nodiscard]] Int32 GetHeight() const noexcept
        {
            return Read<Int32>(pointer, Layout::OffsetLayerHeight);
        }

        /// <summary>依次访问子层, 不要在回调里增删子层</summary>
        template <typename TVisitor> void ForEachChild(TVisitor&& visit) const
        {
            const cfw::List<void*>* children = reinterpret_cast<const cfw::List<void*>*>(
                static_cast<const Byte*>(pointer) + Layout::OffsetLayerChildren);

            for (void** item = children->begin; item != children->end; ++item)
            {
                visit(LayerRef{*item});
            }
        }

        /// <summary>设置位置, 坐标相对父层, y 向上为正</summary>
        void SetPosition(Int32 x, Int32 y) const noexcept
        {
            using SetPositionFunction = void(__fastcall*)(void* self, void* edx, Int32 x, Int32 y);
            reinterpret_cast<SetPositionFunction>(GetSlotFunction(Layout::VtableSlotLayerSetPosition))(pointer, nullptr, x, y);
        }

        /// <summary>设置尺寸, 单位像素; 派生类可能附带重排内部子层</summary>
        void SetSize(Int32 width, Int32 height) const noexcept
        {
            using SetSizeFunction = void(__fastcall*)(void* self, void* edx, Int32 width, Int32 height);
            reinterpret_cast<SetSizeFunction>(GetSlotFunction(Layout::VtableSlotLayerSetSize))(pointer, nullptr, width, height);
        }

        /// <summary>按索引插入子层, 索引越大绘制越靠上</summary>
        /// <param name="index">0 是底层, 等于子层数量时追加到顶层</param>
        void InsertChild(LayerRef child, Int32 index) const noexcept
        {
            using InsertChildFunction = void(__fastcall*)(void* self, void* edx, void* child, Int32 index);
            reinterpret_cast<InsertChildFunction>(GetSlotFunction(Layout::VtableSlotLayerInsertChild))(
                pointer, nullptr, child.pointer, index);
        }
    };
}
