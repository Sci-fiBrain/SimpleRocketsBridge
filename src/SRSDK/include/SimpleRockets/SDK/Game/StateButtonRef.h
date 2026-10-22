#pragma once

#include <algorithm>
#include <new>
#include <string>

#include "SimpleRockets/SDK/Game/ImageButtonRef.h"
#include "SimpleRockets/SDK/Game/Layout/Rvas.h"
#include "SimpleRockets/SDK/Game/Layout/Sizes.h"
#include "SimpleRockets/SDK/Game/Memory.h"
#include "SimpleRockets/SDK/Game/RuntimeLayerRef.h"
#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>
    /// 游戏 UI 的 StateButton 视图
    ///
    /// 在 ImageButton 之上多一个文字标签, 自带 ActionButton.png 外观, 构造时尺寸取自贴图
    /// </summary>
    struct StateButtonRef : ImageButtonRef
    {
        explicit StateButtonRef(void* pointer) noexcept : ImageButtonRef{pointer} { }

        /// <summary>
        /// 创建按钮, 设好位置并挂到父层
        /// </summary>
        /// <param name="parent">父层, 取自 RuntimeRef::GetLayer</param>
        /// <param name="text">按钮文字</param>
        /// <param name="visible">是否显示</param>
        /// <param name="x">父层坐标系下的横坐标</param>
        /// <param name="y">父层坐标系下的纵坐标, 向上为正</param>
        /// <returns>创建失败时 pointer 为空</returns>
        [[nodiscard]] static StateButtonRef Create(RuntimeLayerRef parent, const char* text, Boolean visible, Int32 x,
            Int32 y) noexcept
        {
            if (parent.IsNull() || text == nullptr)
            {
                return StateButtonRef{nullptr};
            }

            const void* moduleBase = parent.GetModuleBase();
            const StateButtonRef result = CreateUnattached(moduleBase, text);
            if (result.IsNull())
            {
                return result;
            }

            result.SetPosition(x, y);
            result.SetVisible(visible);
            AttachToParent(moduleBase, parent, result.pointer);

            return result;
        }

        /// <summary>
        /// 创建按钮并自动排到两侧栏位, 不与已有 StateButton 重叠
        ///
        /// 栏位位于游戏按钮右侧一个身位, 自游戏 MAP 按钮所在行向下排; 左右两列取更靠上的一侧
        /// </summary>
        /// <param name="parent">父层, 取自 RuntimeRef::GetLayer</param>
        /// <param name="text">按钮文字</param>
        /// <param name="visible">默认显示状态</param>
        /// <returns>创建失败时 pointer 为空</returns>
        [[nodiscard]] static StateButtonRef CreateStacked(RuntimeLayerRef parent, const char* text, Boolean visible) noexcept
        {
            if (parent.IsNull() || text == nullptr)
            {
                return StateButtonRef{nullptr};
            }

            const void* moduleBase = parent.GetModuleBase();
            const StateButtonRef result = CreateUnattached(moduleBase, text);
            if (result.IsNull())
            {
                return result;
            }

            const UInt32 baseValue = reinterpret_cast<UInt32>(moduleBase);
            const Int32 width = result.GetWidth();
            const Int32 height = result.GetHeight();
            const Int32 layerWidth = parent.GetWidth();
            const Int32 layerHeight = parent.GetHeight();
            const Int32 top = GetColumnTop(parent, layerHeight, height, baseValue);

            const Int32 leftX = StackMargin + width + StackGap;
            const Int32 rightX = layerWidth - StackMargin - width - StackGap - width;
            const Int32 leftY = GetColumnY(parent, baseValue, leftX, width, height, top);
            const Int32 rightY = GetColumnY(parent, baseValue, rightX, width, height, top);

            Int32 x = leftX;
            Int32 y = leftY;

            if (rightY > leftY)
            {
                x = rightX;
                y = rightY;
            }

            result.SetPosition(x, y);
            result.SetVisible(visible);
            parent.InsertChild(LayerRef{result.pointer}, GetInsertIndex(parent, baseValue));

            return result;
        }

    private:
        /// <summary>与屏幕边缘的间距, 与游戏 HUD 一致</summary>
        static constexpr Int32 StackMargin = 2;

        /// <summary>按钮之间的垂直间距, 与游戏 HUD 一致</summary>
        static constexpr Int32 StackGap = 3;

        /// <summary>分配并构造按钮, 尚未设置位置与挂载</summary>
        [[nodiscard]] static StateButtonRef CreateUnattached(const void* moduleBase, const char* text) noexcept
        {
            void* button = ::operator new(Layout::SizeofStateButton, std::nothrow);
            if (button == nullptr)
            {
                return StateButtonRef{nullptr};
            }

            using CtorFunction = void*(__fastcall*)(void* self, void* edx, const std::string& name);

            const std::string name{text};
            reinterpret_cast<CtorFunction>(static_cast<const Byte*>(moduleBase) + Layout::RvaStateButtonCtor)(
                button, nullptr, name);

            return StateButtonRef{button};
        }

        /// <summary>挂到父层, 由父层触发一次布局</summary>
        static void AttachToParent(const void* moduleBase, RuntimeLayerRef parent, void* button) noexcept
        {
            using AddChildFunction = void(__fastcall*)(void* self, void* edx, void* child);
            reinterpret_cast<AddChildFunction>(static_cast<const Byte*>(moduleBase) + Layout::RvaLayerAddChild)(
                parent.pointer, nullptr, button);
        }

        /// <summary>子层是否为 StateButton</summary>
        [[nodiscard]] static Boolean IsStateButton(LayerRef child, UInt32 moduleBase) noexcept
        {
            return Read<UInt32>(child.pointer, 0) - moduleBase == Layout::RvaVtableStateButton;
        }

        /// <summary>在指定栏位自 top 向下找不与其他 StateButton 重叠的纵坐标, 放不下时返回小于边距的值</summary>
        [[nodiscard]] static Int32 GetColumnY(RuntimeLayerRef parent, UInt32 moduleBase, Int32 x, Int32 width,
            Int32 height, Int32 top) noexcept
        {
            Int32 y = top;
            Boolean settled = false;

            while (!settled && y >= StackMargin)
            {
                settled = true;

                parent.ForEachChild(
                    [&](LayerRef child)
                    {
                        if (!IsStateButton(child, moduleBase))
                        {
                            return;
                        }

                        if (child.GetX() >= x + width || x >= child.GetX() + child.GetWidth())
                        {
                            return;
                        }

                        if (y >= child.GetY() + child.GetHeight() || y + height <= child.GetY())
                        {
                            return;
                        }

                        y = child.GetY() - height - StackGap;
                        settled = false;
                    });
            }

            return y;
        }

        /// <summary>子层插入位置: 首个游戏 StateButton 之前, 没有则追加到末位</summary>
        [[nodiscard]] static Int32 GetInsertIndex(RuntimeLayerRef parent, UInt32 moduleBase) noexcept
        {
            Int32 index = 0;
            Boolean found = false;
            Int32 childCount = 0;

            parent.ForEachChild(
                [&](LayerRef child)
                {
                    if (!found && IsStateButton(child, moduleBase))
                    {
                        index = childCount;
                        found = true;
                    }

                    ++childCount;
                });

            return found ? index : childCount;
        }

        /// <summary>栏位起始纵坐标, 取游戏 MAP 按钮所在行; 该字段缺失时退回层顶端</summary>
        [[nodiscard]] static Int32 GetColumnTop(RuntimeLayerRef parent, Int32 layerHeight, Int32 height,
            UInt32 moduleBase) noexcept
        {
            const Int32 layerTop = layerHeight - StackMargin - height;
            void* mapButton = Read<void*>(parent.pointer, Layout::OffsetRuntimeLayerMapButton);

            if (mapButton == nullptr || !IsStateButton(LayerRef{mapButton}, moduleBase))
            {
                return layerTop;
            }

            return std::min(layerTop, StateButtonRef{mapButton}.GetY());
        }

        /// <summary>游戏模块基址, 由虚表反解</summary>
        [[nodiscard]] void* GetModuleBase() const noexcept
        {
            return reinterpret_cast<void*>(Read<UInt32>(pointer, 0) - Layout::RvaVtableStateButton);
        }
    };
}
