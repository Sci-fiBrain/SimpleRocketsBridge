#include "PanelLayer.h"

#include <format>
#include <new>
#include <string>

#include "SelectionLayer.h"

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

        /// <summary>开关按钮到面板下沿的距离</summary>
        constexpr Int32 ToggleButtonMargin = 10;

        /// <summary>开关按钮的两种文字</summary>
        constexpr const char* OnText = "On";
        constexpr const char* OffText = "Off";

        /// <summary>点开关时翻转选中零件的启停, 带展开态的零件按类型展开或收回</summary>
        void OnTogglePartClicked([[maybe_unused]] ButtonRef sender,
            [[maybe_unused]] const ClickEventArgs& args) noexcept
        {
            const PartObjectRef part = SelectionLayer::GetSelectedPart();
            if (part.IsNull() || part.IsType(PartObjectKind::TankObject))
            {
                return;
            }

            const Boolean enabled = !part.GetActivated();

            if (part.IsType(PartObjectKind::ParachuteObject))
            {
                ParachuteObjectRef{part.pointer}.SetEnabled(enabled);
                return;
            }

            if (part.IsType(PartObjectKind::LanderObject))
            {
                LanderObjectRef{part.pointer}.SetEnabled(enabled);
                return;
            }

            if (part.IsType(PartObjectKind::SolarPanelObject))
            {
                SolarPanelObjectRef{part.pointer}.SetEnabled(enabled);
                return;
            }

            if (part.IsType(PartObjectKind::EngineObject))
            {
                EngineObjectRef{part.pointer}.SetEnabled(enabled);
                return;
            }

            part.SetEnabled(enabled);
        }
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

    void PanelLayer::OnUpdate() noexcept
    {
        RefreshContent();
    }

    void PanelLayer::BuildContent() noexcept
    {
        ILogger& logger = Environment::GetLogger();
        const LayerRef panelRef{this};
        const Int32 contentWidth = PanelWidth - ContentInset * 2;
        const Int32 titleY = PanelHeight - TitleTopMargin - TitleHeight;

        titleLabel = CreateLabel(panelRef, "Part", contentWidth, TitleHeight);
        if (titleLabel.IsNull())
        {
            if (logger.IsEnabled(LogSeverity::Error))
            {
                logger.Log(LogSeverity::Error, "标题创建失败");
            }
            return;
        }

        titleLabel.SetTextColor(TitleTextColor);
        titleLabel.SetTextAlign(LabelTextAlign::Center);
        titleLabel.SetPosition(ContentInset, titleY);

        nameLabel = CreateLabel(panelRef, "", contentWidth);
        if (nameLabel.IsNull())
        {
            if (logger.IsEnabled(LogSeverity::Error))
            {
                logger.Log(LogSeverity::Error, "名字行创建失败");
            }
            return;
        }

        nameLabel.SetTextAlign(LabelTextAlign::Center);
        nameLabel.SetPosition(ContentInset, titleY - DefaultFontLineHeight);

        infoLabel = CreateLabel(panelRef, "", contentWidth);
        if (infoLabel.IsNull())
        {
            if (logger.IsEnabled(LogSeverity::Error))
            {
                logger.Log(LogSeverity::Error, "信息行创建失败");
            }
            return;
        }

        infoLabel.SetTextAlign(LabelTextAlign::Center);
        infoLabel.SetTextColor(InfoTextColor);
        infoLabel.SetPosition(ContentInset, titleY - DefaultFontLineHeight * 2);

        toggleButton = CreateUnattachedStateButton(OnText);
        if (toggleButton.IsNull())
        {
            if (logger.IsEnabled(LogSeverity::Error))
            {
                logger.Log(LogSeverity::Error, "开关按钮创建失败");
            }
            return;
        }

        toggleButton.SetPosition((PanelWidth - toggleButton.GetWidth()) / 2, ToggleButtonMargin);
        toggleButton.SetVisible(false);
        toggleButton.SetClickHandler<OnTogglePartClicked>(nullptr);
        AttachToParent(panelRef, toggleButton.pointer);
    }

    void PanelLayer::RefreshContent() noexcept
    {
        const PartObjectRef part = SelectionLayer::GetSelectedPart();

        if (part.pointer != shownPart.pointer)
        {
            shownPart = part;

            const std::string name = part.IsNull() ? std::string{ } : part.GetDisplayName();
            nameLabel.SetText(name.c_str());

            const Boolean isTank = !part.IsNull() && part.IsType(PartObjectKind::TankObject);

            toggleButton.SetVisible(!part.IsNull() && !isTank);
            infoLabel.SetText("");
            shownInfo.clear();
            shownActive = -1;
        }

        if (part.IsNull())
        {
            return;
        }

        if (part.IsType(PartObjectKind::TankObject))
        {
            const TankObjectRef tank{part.pointer};
            const std::string info = std::format("Fuel: {:.1f} / {:.1f}", tank.GetFuel(), tank.GetCapacity());

            if (info != shownInfo)
            {
                shownInfo = info;
                infoLabel.SetText(info.c_str());
                infoLabel.SetTextColor(InfoTextColor);
            }

            return;
        }

        const Boolean active = part.GetActivated();
        if (static_cast<Int32>(active) != shownActive)
        {
            shownActive = static_cast<Int32>(active);

            const UInt32 stateColor = active ? ActiveTextColor : InactiveTextColor;

            toggleButton.SetSelected(active);
            toggleButton.GetLabel().SetText(active ? OffText : OnText);
            toggleButton.GetLabel().SetTextColor(active ? InactiveTextColor : ActiveTextColor);

            infoLabel.SetText(active ? "Status: On" : "Status: Off");
            infoLabel.SetTextColor(stateColor);
        }
    }
}
