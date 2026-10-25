#pragma once

#include <string>

#include "SimpleRockets/SRSDK.h"

namespace PartMod
{
    /// <summary>面板宽度</summary>
    constexpr SimpleRockets::Int32 PanelWidth = 380;

    /// <summary>面板高度</summary>
    constexpr SimpleRockets::Int32 PanelHeight = 175;

    /// <summary>已激活的文字颜色, 0xAABBGGRR</summary>
    constexpr SimpleRockets::UInt32 ActiveTextColor = 0xFF00E600;

    /// <summary>未激活的文字颜色, 0xAABBGGRR</summary>
    constexpr SimpleRockets::UInt32 InactiveTextColor = 0xFF5252FF;

    /// <summary>信息行的普通颜色</summary>
    constexpr SimpleRockets::UInt32 InfoTextColor = 0xFFFFFFFF;

    /// <summary>面板层, 显示选中零件的信息与启停开关</summary>
    class PanelLayer final : public SimpleRockets::SDK::Ui::LayerBase
    {
    public:
        explicit PanelLayer(SimpleRockets::Int32 subscriptionMask) noexcept;

        ~PanelLayer() noexcept override;

    protected:
        /// <summary>底板铺满整层</summary>
        void OnSizeChanged(SimpleRockets::Int32 width, SimpleRockets::Int32 height) noexcept override;

        /// <summary>绘制底板</summary>
        void OnDraw() noexcept override;

        /// <summary>面板内的触摸由面板独占, 不再冒泡到下层</summary>
        void OnTouch([[maybe_unused]] SimpleRockets::SDK::Game::LayerRef sender,
            SimpleRockets::SDK::Ui::TouchEventArgs& args) noexcept override;

        /// <summary>每帧跟随选中零件刷新名字与开关状态</summary>
        void OnUpdate() noexcept override;

    private:
        /// <summary>建标题、名字行、信息行与开关按钮</summary>
        void BuildContent() noexcept;

        /// <summary>每帧同步名字、信息行与开关按钮</summary>
        void RefreshContent() noexcept;

        SimpleRockets::SDK::Ui::Rectangle* background;
        SimpleRockets::SDK::Game::LabelRef titleLabel{nullptr};
        SimpleRockets::SDK::Game::LabelRef nameLabel{nullptr};
        SimpleRockets::SDK::Game::LabelRef infoLabel{nullptr};
        SimpleRockets::SDK::Game::StateButtonRef toggleButton{nullptr};
        SimpleRockets::SDK::Game::PartObjectRef shownPart{nullptr};
        std::string shownInfo{};
        SimpleRockets::Int32 shownActive = -1;
    };
}
