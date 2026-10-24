#pragma once

#include <algorithm>
#include <new>
#include <string>

#include "SimpleRockets/SDK/Environment.h"
#include "SimpleRockets/SDK/Game/Layout/Offsets.h"
#include "SimpleRockets/SDK/Game/Layout/Rvas.h"
#include "SimpleRockets/SDK/Game/Layout/Sizes.h"
#include "SimpleRockets/SDK/Game/Memory.h"
#include "SimpleRockets/SDK/Game/RuntimeLayerRef.h"
#include "SimpleRockets/SDK/Game/StateButtonRef.h"
#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Ui
{
    /// <summary>按钮之间的垂直间距, 与游戏 HUD 一致</summary>
    constexpr Int32 StateButtonStackGap = 3;

    /// <summary>分配并构造按钮, 尚未设置位置与挂载</summary>
    /// <param name="text">按钮文字</param>
    [[nodiscard]] inline Game::StateButtonRef CreateUnattachedStateButton(const char* text) noexcept
    {
        void* button = ::operator new(Game::Layout::SizeofStateButton, std::nothrow);
        if (button == nullptr)
        {
            return Game::StateButtonRef{nullptr};
        }

        using CtorFunction = void*(__fastcall*)(void* self, void* edx, const std::string& name);

        const std::string name{text};
        Environment::GetAddress<CtorFunction>(Game::Layout::RvaStateButtonCtor)(button, nullptr, name);

        return Game::StateButtonRef{button};
    }

    /// <summary>挂到父层</summary>
    inline void AttachToParent(Game::LayerRef parent, void* child) noexcept
    {
        parent.AddChild(Game::LayerRef{child});
    }

    /// <summary>子层是否为 StateButton</summary>
    [[nodiscard]] inline Boolean IsStateButton(Game::LayerRef child) noexcept
    {
        return Game::Read<UInt32>(child.pointer, 0) ==
            Environment::GetAddress<UInt32>(Game::Layout::RvaVtableStateButton);
    }

    /// <summary>自 top 向下找不与其他 StateButton 重叠的纵坐标, 放不下时返回小于边距的值</summary>
    [[nodiscard]] inline Int32 GetStateButtonColumnY(Game::RuntimeLayerRef parent, Int32 x, Int32 width, Int32 height,
        Int32 top) noexcept
    {
        Int32 y = top;
        Boolean settled = false;

        while (!settled && y >= Game::RuntimeLayerRef::ContentMargin)
        {
            settled = true;

            parent.ForEachChild(
                [&](Game::LayerRef child)
                {
                    if (!IsStateButton(child))
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

                    y = child.GetY() - height - StateButtonStackGap;
                    settled = false;
                });
        }

        return y;
    }

    /// <summary>子层插入位置: 首个游戏 StateButton 之前, 没有则追加到末位</summary>
    [[nodiscard]] inline Int32 GetStateButtonInsertIndex(Game::RuntimeLayerRef parent) noexcept
    {
        Int32 index = 0;
        Boolean found = false;
        Int32 childCount = 0;

        parent.ForEachChild(
            [&](Game::LayerRef child)
            {
                if (!found && IsStateButton(child))
                {
                    index = childCount;
                    found = true;
                }

                ++childCount;
            });

        return found ? index : childCount;
    }

    /// <summary>创建按钮, 设好位置并挂到父层</summary>
    /// <param name="parent">父层, 取自 RuntimeRef::GetLayer</param>
    /// <param name="text">按钮文字</param>
    /// <param name="visible">是否显示</param>
    /// <param name="x">父层坐标系下的横坐标</param>
    /// <param name="y">父层坐标系下的纵坐标, 向上为正</param>
    /// <returns>创建失败时 pointer 为空</returns>
    [[nodiscard]] inline Game::StateButtonRef CreateStateButton(Game::LayerRef parent, const char* text,
        Boolean visible, Int32 x, Int32 y) noexcept
    {
        if (parent.IsNull() || text == nullptr)
        {
            return Game::StateButtonRef{nullptr};
        }

        const Game::StateButtonRef result = CreateUnattachedStateButton(text);
        if (result.IsNull())
        {
            return result;
        }

        result.SetPosition(x, y);
        result.SetVisible(visible);
        AttachToParent(parent, result.pointer);

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
    [[nodiscard]] inline Game::StateButtonRef CreateStackedStateButton(Game::RuntimeLayerRef parent,
        const char* text, Boolean visible) noexcept
    {
        if (parent.IsNull() || text == nullptr)
        {
            return Game::StateButtonRef{nullptr};
        }

        const Game::StateButtonRef result = CreateUnattachedStateButton(text);
        if (result.IsNull())
        {
            return result;
        }

        const Int32 width = result.GetWidth();
        const Int32 height = result.GetHeight();
        const Int32 layerWidth = parent.GetWidth();
        const Int32 top = parent.GetContentTop() - height;

        const Int32 leftX = Game::RuntimeLayerRef::ContentMargin + width + StateButtonStackGap;
        const Int32 rightX =
            layerWidth - Game::RuntimeLayerRef::ContentMargin - width - StateButtonStackGap - width;
        const Int32 leftY = GetStateButtonColumnY(parent, leftX, width, height, top);
        const Int32 rightY = GetStateButtonColumnY(parent, rightX, width, height, top);

        Int32 x = leftX;
        Int32 y = leftY;

        if (rightY > leftY)
        {
            x = rightX;
            y = rightY;
        }

        result.SetPosition(x, y);
        result.SetVisible(visible);
        parent.InsertChild(Game::LayerRef{result.pointer}, GetStateButtonInsertIndex(parent));

        return result;
    }
}
