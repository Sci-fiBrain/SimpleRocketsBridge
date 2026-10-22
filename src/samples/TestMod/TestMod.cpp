#include "SimpleRockets/SRSDK.h"

using namespace SimpleRockets;
using namespace SimpleRockets::SDK;
using namespace SimpleRockets::SDK::Game;
using namespace SimpleRockets::SDK::Log;
using namespace SimpleRockets::SDK::Mod;

namespace
{
    ILogger* logger = nullptr;

    /// <summary>按钮回调的 state, 持有所在场景以便点击时定位飞船</summary>
    struct FullButton
    {
        RuntimeRef runtime{nullptr};
    };

    FullButton fullButton;

    /// <summary>补满玩家船的全部油箱</summary>
    void RefillTanks(RuntimeRef runtime) noexcept
    {
        const ShipOrbitNodeRef playerNode = runtime.GetPlayerNode();
        if (playerNode.IsNull()) return;

        const ShipRef ship = playerNode.GetShip();
        if (ship.IsNull()) return;

        ship.ForEachPart(
            [](PartObjectRef part)
            {
                if (!part.IsType(PartObjectKind::TankObject))
                {
                    return;
                }

                TankObjectRef tank = part.As<TankObjectRef>();
                tank.SetFuel(tank.GetCapacity());
            });
    }

    void OnFullClick(ButtonRef sender, const ClickEventArgs& args) noexcept
    {
        const FullButton* state = static_cast<const FullButton*>(args.state);
        RefillTanks(state->runtime);

        const StateButtonRef button = sender.As<StateButtonRef>();
        // button.SetSelected(button.GetSelected() == 0);

        if (logger != nullptr)
        {
            logger->Log(LogSeverity::Info, "FULL clicked");
        }
    }

    class TestHook final : public RuntimeHookBase
    {
    public:
        void OnInitialized(RuntimeRef sender, [[maybe_unused]] const EventArgs& args) noexcept override
        {
            fullButton.runtime = sender;
        }

        void OnLoaded(RuntimeLayerRef sender, [[maybe_unused]] const EventArgs& args) noexcept override
        {
            const StateButtonRef button = StateButtonRef::CreateStacked(sender, "FULL", true);
            if (button.IsNull())
            {
                if (logger != nullptr)
                {
                    logger->Log(LogSeverity::Error, "FULL button creation failed");
                }

                return;
            }

            button.SetClickHandler<&OnFullClick>(&fullButton);
        }

        void OnDisposing([[maybe_unused]] RuntimeRef sender, [[maybe_unused]] const EventArgs& args) noexcept override
        {
            fullButton.runtime = RuntimeRef{nullptr};
        }
    };

    TestHook testHook;
}

extern "C" __declspec(dllexport) Boolean SRModLoad(ILogger* target)
{
    logger = target;
    return true;
}

extern "C" __declspec(dllexport) RuntimeHookBase* SRModRuntimeHook()
{
    return &testHook;
}
