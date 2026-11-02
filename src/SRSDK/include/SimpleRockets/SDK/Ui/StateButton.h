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

    /// <summary>创建按钮并挂到父层, 位置与可见性由调用方设置</summary>
    /// <param name="parent">父层, 取自 RuntimeRef::GetLayer</param>
    /// <param name="text">按钮文字</param>
    /// <returns>父层为空时 pointer 为空</returns>
    [[nodiscard]] inline Game::StateButtonRef CreateStateButton(Game::LayerRef parent, const std::string& text)
    {
        if (parent.IsNull())
        {
            return Game::StateButtonRef{nullptr};
        }

        void* button = ::operator new(Game::Layout::SizeofStateButton);

        using CtorFunction = void*(__fastcall*)(void* self, void* edx, const std::string& name);

        Environment::GetAddress<CtorFunction>(Game::Layout::RvaStateButtonCtor)(button, nullptr, text);

        const Game::StateButtonRef result{button};

        parent.AddChild(result);

        return result;
    }

    /// <summary>
    /// 创建按钮并自动排到两侧栏位, 不与已有 StateButton 重叠
    ///
    /// 栏位位于游戏按钮右侧一个身位, 自游戏 MAP 按钮所在行向下排; 左右两列取更靠上的一侧
    /// </summary>
    /// <param name="parent">父层, 取自 RuntimeRef::GetLayer</param>
    /// <param name="text">按钮文字</param>
    /// <returns>父层为空时 pointer 为空</returns>
    [[nodiscard]] inline Game::StateButtonRef CreateStackedStateButton(Game::RuntimeLayerRef parent,
        const std::string& text)
    {
        if (parent.IsNull())
        {
            return Game::StateButtonRef{nullptr};
        }

        void* button = ::operator new(Game::Layout::SizeofStateButton);

        using CtorFunction = void*(__fastcall*)(void* self, void* edx, const std::string& name);

        Environment::GetAddress<CtorFunction>(Game::Layout::RvaStateButtonCtor)(button, nullptr, text);

        const Game::StateButtonRef result{button};

        const Int32 width = result.GetWidth();
        const Int32 height = result.GetHeight();
        const Int32 layerWidth = parent.GetWidth();
        const Int32 top = parent.GetContentTop() - height;

        // 同一列内从 top 向下找不与其他 StateButton 重叠的纵坐标
        struct ColumnYCalculator
        {
            Game::RuntimeLayerRef parent;
            Int32 width;
            Int32 height;
            Int32 top;

            [[nodiscard]] Int32 operator()(Int32 x) const noexcept
            {
                Int32 y = top;
                Boolean settled = false;

                while (!settled && y >= Game::RuntimeLayerRef::ContentMargin)
                {
                    settled = true;

                    parent.ForEachChild(
                        [&](Game::LayerRef child)
                        {
                            if (Game::Read<UInt32>(child.pointer, 0) !=
                                Environment::GetAddress<UInt32>(Game::Layout::RvaVtableStateButton))
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
        };

        const Int32 leftX = Game::RuntimeLayerRef::ContentMargin + width + StateButtonStackGap;
        const Int32 rightX =
            layerWidth - Game::RuntimeLayerRef::ContentMargin - width - StateButtonStackGap - width;

        const ColumnYCalculator columnY{parent, width, height, top};
        const Int32 leftY = columnY(leftX);
        const Int32 rightY = columnY(rightX);

        Int32 x = leftX;
        Int32 y = leftY;

        if (rightY > leftY)
        {
            x = rightX;
            y = rightY;
        }

        result.SetPosition(x, y);

        // 插到首个游戏 StateButton 之前, 没有则追加到末位
        Int32 index = 0;
        Boolean found = false;
        Int32 childCount = 0;

        parent.ForEachChild(
            [&](Game::LayerRef child)
            {
                if (!found && Game::Read<UInt32>(child.pointer, 0) ==
                    Environment::GetAddress<UInt32>(Game::Layout::RvaVtableStateButton))
                {
                    index = childCount;
                    found = true;
                }

                ++childCount;
            });

        parent.InsertChild(result, found ? index : childCount);

        return result;
    }
}
