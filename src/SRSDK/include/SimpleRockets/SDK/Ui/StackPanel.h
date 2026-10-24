#pragma once

#include <new>

#include "SimpleRockets/SDK/Environment.h"
#include "SimpleRockets/SDK/Game/LayerRef.h"
#include "SimpleRockets/SDK/Game/Layout/Rvas.h"
#include "SimpleRockets/SDK/Game/Layout/Sizes.h"
#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Ui
{
    /// <summary>创建堆叠容器并挂到父层</summary>
    /// <param name="parent">父层</param>
    /// <param name="horizontal">为 true 时横向排列, 否则纵向</param>
    /// <param name="spacing">相邻子层的间距, 单位像素</param>
    [[nodiscard]] inline Game::LayerRef CreateStackPanel(Game::LayerRef parent, Boolean horizontal,
        Int32 spacing) noexcept
    {
        void* panel = ::operator new(Game::Layout::SizeofStackPanel, std::nothrow);
        if (panel == nullptr)
        {
            return Game::LayerRef{nullptr};
        }

        using CtorFunction = void*(__fastcall*)(void* self, void* edx, Int32 orientation, Int32 spacing);
        Environment::GetAddress<CtorFunction>(Game::Layout::RvaStackPanelCtor)(panel, nullptr,
            horizontal ? 0 : 1, spacing);

        parent.AddChild(Game::LayerRef{panel});
        return Game::LayerRef{panel};
    }
}
