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

        ClearHighlight();

        if (tree.IsNull())
        {
            return;
        }

        const PartObjectRef part = tree.GetPart();
        if (part.IsNull())
        {
            return;
        }

        part.SetHighlighted(true);
        selectedPart = part;
    }
}
