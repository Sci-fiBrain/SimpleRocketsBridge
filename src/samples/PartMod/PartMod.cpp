#include <new>

#include "SimpleRockets/SRSDK.h"

using namespace SimpleRockets;
using namespace SimpleRockets::SDK;
using namespace SimpleRockets::SDK::Game;
using namespace SimpleRockets::SDK::Log;
using namespace SimpleRockets::SDK::Mod;
using namespace SimpleRockets::SDK::Ui;

namespace
{
    /// <summary>面板宽度</summary>
    constexpr Int32 PanelWidth = 380;

    /// <summary>面板高度</summary>
    constexpr Int32 PanelHeight = 145;

    /// <summary>面板上沿到父层顶部的距离</summary>
    constexpr Int32 PanelTopMargin = 120;

    /// <summary>面板内标题到上沿的距离</summary>
    constexpr Int32 TitleTopMargin = 10;

    /// <summary>标题高度</summary>
    constexpr Int32 TitleHeight = 22;

    /// <summary>内容区左右内缩</summary>
    constexpr Int32 ContentInset = 10;

    /// <summary>相邻数据行的间距</summary>
    constexpr Int32 RowSpacing = 2;

    /// <summary>数据行数量</summary>
    constexpr Int32 RowCount = 2;

    /// <summary>面板层, 目前只有底板</summary>
    class PanelLayer final : public LayerBase
    {
    public:
        explicit PanelLayer(Int32 subscriptionMask) noexcept
            : LayerBase(subscriptionMask),
              background(new (std::nothrow) Rectangle())
        {
            SetSize(PanelWidth, PanelHeight);
        }

        ~PanelLayer() noexcept override
        {
            delete background;
        }

    protected:
        /// <summary>底板铺满整层</summary>
        void OnSizeChanged(Int32 width, Int32 height) noexcept override
        {
            if (background == nullptr || width <= 0 || height <= 0)
            {
                return;
            }

            background->SetBounds(0, 0, width, height);
        }

        /// <summary>绘制底板</summary>
        void OnDraw() noexcept override
        {
            if (background == nullptr)
            {
                return;
            }

            background->Draw();
        }

        /// <summary>面板内的触摸由面板独占, 不再冒泡到下层</summary>
        void OnTouch([[maybe_unused]] Game::LayerRef sender, TouchEventArgs& args) noexcept override
        {
            args.Handled = true;
        }

    private:
        Rectangle* background;
    };

    class PartHook final : public RuntimeHookBase
    {
    public:
        void OnLoaded(RuntimeLayerRef sender, [[maybe_unused]] const EventArgs& args) noexcept override
        {
            ILogger& logger = Environment::GetLogger();

            const LayerRef panel = CreateLayer<PanelLayer>(sender, LayerEvent::Touch);
            if (panel.IsNull())
            {
                if (logger.IsEnabled(LogSeverity::Error))
                {
                    logger.Log(LogSeverity::Error, "面板创建失败");
                }
                return;
            }

            panel.SetPosition((sender.GetWidth() - PanelWidth) / 2,
                sender.GetHeight() - PanelHeight - PanelTopMargin);

            const Int32 contentWidth = PanelWidth - ContentInset * 2;
            const Int32 titleY = PanelHeight - TitleTopMargin - TitleHeight;
            const Int32 rowsHeight =
                RowCount * DefaultFontLineHeight + (RowCount - 1) * RowSpacing;

            const LabelRef title = CreateLabel(panel, "Fuel Transfer Mode", contentWidth, TitleHeight);
            title.SetTextColor(TitleTextColor);
            title.SetTextAlign(LabelTextAlign::Center);
            if (!title.IsNull())
            {
                title.SetPosition(ContentInset, titleY);
            }

            const LayerRef stack = CreateStackPanel(panel, false, RowSpacing);
            if (stack.IsNull())
            {
                logger.Log(LogSeverity::Error, "StackPanel 创建失败");
                return;
            }

            stack.SetPosition(ContentInset, titleY - rowsHeight);
            stack.SetSize(contentWidth, rowsHeight);

            [[maybe_unused]] const LabelRef first = CreateLabel(stack, "5978L / 6000L", contentWidth);
            first.SetTextAlign(LabelTextAlign::Center);

            [[maybe_unused]] const LabelRef second = CreateLabel(stack, "1200L / 3000L", contentWidth);
            second.SetTextAlign(LabelTextAlign::Center);
        }
    };

    PartHook partHook;
}

extern "C" __declspec(dllexport) Boolean SRModLoad(Environment* environment)
{
    Environment::Attach(environment);
    return true;
}

extern "C" __declspec(dllexport) RuntimeHookBase* SRModRuntimeHook()
{
    return &partHook;
}
