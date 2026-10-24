#pragma once

#include "SimpleRockets/SRSDK.h"

namespace PartMod
{
    /// <summary>面板宽度</summary>
    constexpr SimpleRockets::Int32 PanelWidth = 380;

    /// <summary>面板高度</summary>
    constexpr SimpleRockets::Int32 PanelHeight = 145;

    /// <summary>面板层, 自绘半透明底板并填入内容</summary>
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

    private:
        /// <summary>按版式常量填入标题与数据行</summary>
        void BuildContent() noexcept;

        SimpleRockets::SDK::Ui::Rectangle* background;
    };
}
