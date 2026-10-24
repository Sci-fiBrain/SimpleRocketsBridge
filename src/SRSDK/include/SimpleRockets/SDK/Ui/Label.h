#pragma once

#include <new>
#include <string>

#include "SimpleRockets/SDK/Environment.h"
#include "SimpleRockets/SDK/Game/LabelRef.h"
#include "SimpleRockets/SDK/Game/Layout/Offsets.h"
#include "SimpleRockets/SDK/Game/Layout/Rvas.h"
#include "SimpleRockets/SDK/Game/Layout/Sizes.h"
#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Ui
{
    /// <summary>UI 小号字体, 与游戏 HUD 按钮一致</summary>
    constexpr const char* DefaultFontName = "FontSmall";

    /// <summary>默认字体的行高</summary>
    constexpr Int32 DefaultFontLineHeight = 30;

    /// <summary>正文文字颜色</summary>
    constexpr UInt32 DefaultTextColor = 0xFFFFFFFF;

    /// <summary>面板标题文字颜色</summary>
    constexpr UInt32 TitleTextColor = 0xFF00D7A6;

    /// <summary>创建标签并挂到父层</summary>
    /// <param name="parent">父层</param>
    /// <param name="text">显示的文字</param>
    /// <param name="width">标签宽度, 单位像素</param>
    /// <param name="height">标签高度, 单位像素, 默认等于字体行高</param>
    /// <param name="fontName">字体名, 默认与 HUD 一致</param>
    [[nodiscard]] inline Game::LabelRef CreateLabel(Game::LayerRef parent, const char* text, Int32 width,
        Int32 height = DefaultFontLineHeight, const char* fontName = DefaultFontName) noexcept
    {
        void* label = ::operator new(Game::Layout::SizeofLabel, std::nothrow);
        if (label == nullptr)
        {
            return Game::LabelRef{nullptr};
        }

        using CtorFunction =
            void*(__fastcall*)(void* self, void* edx, Int32 width, Int32 height, std::string fontName);
        Environment::GetAddress<CtorFunction>(Game::Layout::RvaLabelCtor)(label, nullptr, width, height,
            std::string{fontName});

        const Game::LabelRef result{label};
        result.SetTextColor(DefaultTextColor);
        result.SetText(text);
        parent.AddChild(Game::LayerRef{label});
        return result;
    }
}
