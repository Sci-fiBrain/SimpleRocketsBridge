#include "SelectionLayer.h"

using namespace SimpleRockets;
using namespace SimpleRockets::SDK;
using namespace SimpleRockets::SDK::Game;
using namespace SimpleRockets::SDK::Ui;

namespace PartMod
{
    namespace
    {
        RuntimeRef currentRuntime{nullptr};
        PartObjectRef selectedPart{nullptr};

        /// <summary>可被选中的零件实现类</summary>
        constexpr PartObjectKind SelectableKinds[] =
        {
            PartObjectKind::TankObject,
            PartObjectKind::EngineObject,
            PartObjectKind::ParachuteObject,
            PartObjectKind::LanderObject,
            PartObjectKind::SolarPanelObject,
        };

        /// <summary>零件是否属于可选中类型</summary>
        [[nodiscard]] Boolean IsSelectable(const PartObjectRef& part) noexcept
        {
            for (const PartObjectKind kind : SelectableKinds)
            {
                if (part.IsType(kind))
                {
                    return true;
                }
            }

            return false;
        }

        /// <summary>取消当前零件的高亮</summary>
        void ClearHighlight() noexcept
        {
            if (selectedPart.IsNull())
            {
                return;
            }

            selectedPart.SetHighlighted(false);
            selectedPart = PartObjectRef{nullptr};
        }
    }

    void SelectionLayer::SetRuntime(RuntimeRef runtime) noexcept
    {
        currentRuntime = runtime;
    }

    PartObjectRef SelectionLayer::GetSelectedPart() noexcept
    {
        return selectedPart;
    }

    void SelectionLayer::ClearSelection() noexcept
    {
        ClearHighlight();
    }

    void SelectionLayer::OnTouch([[maybe_unused]] LayerRef sender, TouchEventArgs& args) noexcept
    {
        if (args.Phase != TouchPhase::Down || currentRuntime.IsNull())
        {
            return;
        }

        const ShipViewRef shipView = currentRuntime.GetLayer().GetShipView();
        const ShipRef ship = currentRuntime.GetPlayerNode().GetShip();
        const Vector2D world = shipView.ScreenToWorld(args.ScreenX, args.ScreenY);
        const PartTreeRef tree = ship.GetPartTree().FindAtPoint(world, false);

        if (tree.IsNull())
        {
            return;
        }

        const PartObjectRef part = tree.GetPart();
        if (part.IsNull() || !IsSelectable(part) || part == selectedPart)
        {
            return;
        }

        ClearHighlight();
        part.SetHighlighted(true);
        selectedPart = part;
    }
}
