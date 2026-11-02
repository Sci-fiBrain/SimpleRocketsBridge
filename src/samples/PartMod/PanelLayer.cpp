#include "PanelLayer.h"

#include <format>
#include <string>

#include "SelectionLayer.h"

using namespace SimpleRockets;
using namespace SimpleRockets::SDK;
using namespace SimpleRockets::SDK::Game;
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

        /// <summary>点开关时翻转选中零件的启停, 带展开态的零件按类型展开或收回</summary>
        void OnTogglePartClicked(
            [[maybe_unused]] ButtonRef sender, [[maybe_unused]] const ClickEventArgs& args) noexcept
        {
            const PartObjectRef part = SelectionLayer::GetSelectedPart();
            if (part.IsNull() || part.IsType(PartObjectKind::TankObject))
            {
                return;
            }
            const Boolean enabled = !part.GetActivated();
            if (part.IsType(PartObjectKind::ParachuteObject))
            {
                part.As<ParachuteObjectRef>().SetEnabled(enabled);
            }
            else if (part.IsType(PartObjectKind::LanderObject))
            {
                part.As<LanderObjectRef>().SetEnabled(enabled);
            }
            else if (part.IsType(PartObjectKind::SolarPanelObject))
            {
                part.As<SolarPanelObjectRef>().SetEnabled(enabled);
            }
            else if (part.IsType(PartObjectKind::EngineObject))
            {
                part.As<EngineObjectRef>().SetEnabled(enabled);
            }
            else if (part.IsType(PartObjectKind::DetacherObject))
            {
                part.SetEnabled(true);
                SelectionLayer::ClearSelection();
            }
            else
            {
                part.SetEnabled(enabled);
            }
        }
    }

    PanelLayer::PanelLayer(Int32 subscriptionMask)
        : LayerBase(subscriptionMask)
    {
        SetSize(PanelWidth, PanelHeight);

        const LayerRef panelRef{this};
        const Int32 contentWidth = PanelWidth - ContentInset * 2;
        const Int32 titleY = PanelHeight - TitleTopMargin - TitleHeight;

        // 创建标题对象
        titleLabel = CreateLabel(panelRef, contentWidth, TitleHeight);

        // 设置标题对象属性
        titleLabel.SetText("Part");
        titleLabel.SetTextColor(TitleTextColor);
        titleLabel.SetTextAlign(LabelTextAlign::Center);
        titleLabel.SetPosition(ContentInset, titleY);

        // 创建名字行对象
        nameLabel = CreateLabel(panelRef, contentWidth);

        // 设置名字行对象属性
        nameLabel.SetTextAlign(LabelTextAlign::Center);
        nameLabel.SetPosition(ContentInset, titleY - DefaultFontLineHeight);

        // 创建信息行对象
        infoLabel = CreateLabel(panelRef, contentWidth);

        // 设置信息行对象属性
        infoLabel.SetTextAlign(LabelTextAlign::Center);
        infoLabel.SetTextColor(InfoTextColor);
        infoLabel.SetText("Select a part");
        infoLabel.SetPosition(ContentInset, titleY - DefaultFontLineHeight * 2);

        // 创建开关按钮对象
        toggleButton = CreateStateButton(panelRef, "");

        // 设置开关按钮对象属性
        toggleButton.SetPosition((PanelWidth - toggleButton.GetWidth()) / 2, ToggleButtonMargin);
        toggleButton.SetVisible(false);
        toggleButton.SetClickHandler<OnTogglePartClicked>(nullptr);

        // 创建背景对象
        background = new Rectangle();
        background->SetBounds(0, 0, PanelWidth, PanelHeight);
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
        const PartObjectRef part = SelectionLayer::GetSelectedPart();
        if (part.IsNull() && shownPart.IsNull())
        { // 无选中
            return;
        }
        else if (part.IsNull() && !shownPart.IsNull())
        { // 清除选中
            shownPart = part;
            nameLabel.SetText("");
            infoLabel.SetText("Select a part");
            infoLabel.SetTextColor(InfoTextColor);
            toggleButton.SetVisible(false);
        }
        else
        { // 更新UI状态
            if (part != shownPart)
            { // 修改选中
                shownPart = part;
                nameLabel.SetText(part.GetDisplayName());
            }

            if (part.IsType(PartObjectKind::TankObject))
            { // 油箱状态显示
                const TankObjectRef tank = part.As<TankObjectRef>();
                const std::string info = std::format("Fuel: {:.1f} / {:.1f}", tank.GetFuel(), tank.GetCapacity());
                toggleButton.SetVisible(false);
                infoLabel.SetText(info);
                infoLabel.SetTextColor(InfoTextColor);
            }
            else
            { // 其他组件状态
                const Boolean active = part.GetActivated();
                toggleButton.SetVisible(true);
                toggleButton.GetLabel().SetText(active ? "Off" : "On");
                toggleButton.GetLabel().SetTextColor(active ? InactiveTextColor : ActiveTextColor);
                infoLabel.SetText(active ? "Status: On" : "Status: Off");
                infoLabel.SetTextColor(active ? ActiveTextColor : InactiveTextColor);
            }
        }
    }
}
