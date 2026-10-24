#include "PanelLayer.h"

#include <iterator>
#include <new>

using namespace SimpleRockets;
using namespace SimpleRockets::SDK;
using namespace SimpleRockets::SDK::Game;
using namespace SimpleRockets::SDK::Log;
using namespace SimpleRockets::SDK::Ui;

namespace PartMod
{
    namespace
    {
        /// <summary>标题到面板上沿的距离</summary>
        constexpr Int32 TitleTopMargin = 10;

        /// <summary>标题高度</summary>
        constexpr Int32 TitleHeight = 22;

        /// <summary>内容区左右内缩</summary>
        constexpr Int32 ContentInset = 10;

        /// <summary>相邻数据行的间距</summary>
        constexpr Int32 RowSpacing = 2;

        /// <summary>面板标题</summary>
        constexpr const char* PanelTitle = "Fuel Transfer Mode";

        /// <summary>数据行文字</summary>
        constexpr const char* RowTexts[] = {"5978L / 6000L", "1200L / 3000L"};

        /// <summary>数据行数量</summary>
        constexpr Int32 RowCount = static_cast<Int32>(std::size(RowTexts));
    }

    PanelLayer::PanelLayer(Int32 subscriptionMask) noexcept
        : LayerBase(subscriptionMask),
          background(new (std::nothrow) Rectangle())
    {
        SetSize(PanelWidth, PanelHeight);
        BuildContent();
    }

    PanelLayer::~PanelLayer() noexcept
    {
        delete background;
    }

    void PanelLayer::OnSizeChanged(Int32 width, Int32 height) noexcept
    {
        if (background == nullptr || width <= 0 || height <= 0)
        {
            return;
        }

        background->SetBounds(0, 0, width, height);
    }

    void PanelLayer::OnDraw() noexcept
    {
        if (background == nullptr)
        {
            return;
        }

        background->Draw();
    }

    void PanelLayer::OnTouch([[maybe_unused]] LayerRef sender, TouchEventArgs& args) noexcept
    {
        args.Handled = true;
    }

    void PanelLayer::BuildContent() noexcept
    {
        ILogger& logger = Environment::GetLogger();
        const LayerRef panelRef{this};
        const Int32 contentWidth = PanelWidth - ContentInset * 2;
        const Int32 titleY = PanelHeight - TitleTopMargin - TitleHeight;
        const Int32 rowsHeight = RowCount * DefaultFontLineHeight + (RowCount - 1) * RowSpacing;

        const LabelRef title = CreateLabel(panelRef, PanelTitle, contentWidth, TitleHeight);
        if (title.IsNull())
        {
            if (logger.IsEnabled(LogSeverity::Error))
            {
                logger.Log(LogSeverity::Error, "标题创建失败");
            }
            return;
        }

        title.SetTextColor(TitleTextColor);
        title.SetTextAlign(LabelTextAlign::Center);
        title.SetPosition(ContentInset, titleY);

        const LayerRef stack = CreateStackPanel(panelRef, false, RowSpacing);
        if (stack.IsNull())
        {
            if (logger.IsEnabled(LogSeverity::Error))
            {
                logger.Log(LogSeverity::Error, "StackPanel 创建失败");
            }
            return;
        }

        stack.SetPosition(ContentInset, titleY - rowsHeight);
        stack.SetSize(contentWidth, rowsHeight);

        for (const char* text : RowTexts)
        {
            const LabelRef row = CreateLabel(stack, text, contentWidth);
            row.SetTextAlign(LabelTextAlign::Center);
        }
    }
}
