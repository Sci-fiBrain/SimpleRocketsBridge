#pragma once

#include "SimpleRockets/SRSDK.h"

namespace PartMod
{
    /// <summary>全屏透明层, 把点击转为零件选中</summary>
    class SelectionLayer final : public SimpleRockets::SDK::Ui::LayerBase
    {
    public:
        explicit SelectionLayer(SimpleRockets::Int32 subscriptionMask) noexcept
            : LayerBase(subscriptionMask)
        {
        }

        /// <summary>记录当前 Runtime, 拾取时用</summary>
        static void SetRuntime(SimpleRockets::SDK::Game::RuntimeRef runtime) noexcept;

        /// <summary>取消当前零件的高亮</summary>
        static void ClearSelection() noexcept;

    protected:
        /// <summary>按下时拾取零件, 不拦截事件</summary>
        void OnTouch([[maybe_unused]] SimpleRockets::SDK::Game::LayerRef sender,
            SimpleRockets::SDK::Ui::TouchEventArgs& args) noexcept override;
    };
}
