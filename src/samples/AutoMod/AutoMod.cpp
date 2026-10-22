#include <algorithm>

#include "SimpleRockets/SRSDK.h"

using namespace SimpleRockets;
using namespace SimpleRockets::SDK;
using namespace SimpleRockets::SDK::Game;
using namespace SimpleRockets::SDK::Log;
using namespace SimpleRockets::SDK::Mod;

namespace
{
    ILogger* logger = nullptr;

    /// <summary>AUTO 按钮的回调 state, 同时保存高度环的全部状态</summary>
    struct AutoState
    {
        RuntimeRef runtime{nullptr};
        Boolean active = false;
        Double targetAltitude = 0.0;
        Double lastAltitude = 0.0;
        Double integral = 0.0;
        Int32 logCountdown = 0;
    };

    AutoState autoState;

    /// <summary>外环增益, 单位 1/秒</summary>
    constexpr Double HeightGain = 0.5;

    /// <summary>外环输出的垂速上限, 米/秒</summary>
    constexpr Double MaxClimbRate = 10.0;

    /// <summary>内环比例增益</summary>
    constexpr Double VelocityGain = 0.25;

    /// <summary>内环积分增益</summary>
    constexpr Double IntegralGain = 0.05;

    /// <summary>积分项限幅</summary>
    constexpr Double MaxIntegral = 20.0;

    /// <summary>单帧步长上限, 秒</summary>
    constexpr Double MaxDeltaSeconds = 0.1;

    /// <summary>日志间隔, 帧</summary>
    constexpr Int32 LogIntervalFrames = 60;

    /// <summary>点击 AUTO: 激活时以当前离地高度为目标, 再次点击交还油门</summary>
    void OnAutoClick(ButtonRef sender, const ClickEventArgs& args) noexcept
    {
        AutoState* state = static_cast<AutoState*>(args.state);
        const StateButtonRef button = sender.As<StateButtonRef>();

        state->active = !state->active;
        button.SetSelected(state->active);

        if (!state->active)
        {
            return;
        }

        state->targetAltitude = state->runtime.GetAltitudeAgl();
        state->lastAltitude = state->targetAltitude;
        state->integral = 0.0;
        state->logCountdown = 0;
    }

    class AutoHook final : public RuntimeHookBase
    {
    public:
        void OnInitialized(RuntimeRef sender, [[maybe_unused]] const EventArgs& args) noexcept override
        {
            autoState.runtime = sender;
        }

        void OnLoaded(RuntimeLayerRef sender, [[maybe_unused]] const EventArgs& args) noexcept override
        {
            const StateButtonRef button = StateButtonRef::CreateStacked(sender, "AUTO", true);
            if (button.IsNull())
            {
                LogMessage(logger, LogSeverity::Error, "AUTO button creation failed");
                return;
            }

            button.SetClickHandler<&OnAutoClick>(&autoState);
        }

        void OnUpdating(RuntimeRef sender, const FrameEventArgs& args) noexcept override
        {
            if (!autoState.active)
            {
                return;
            }

            const ShipOrbitNodeRef playerNode = sender.GetPlayerNode();
            if (playerNode.IsNull())
            {
                return;
            }

            const ShipRef ship = playerNode.GetShip();
            if (ship.IsNull())
            {
                return;
            }

            PodObjectRef pod = ship.GetPodObject();
            if (pod.IsNull())
            {
                return;
            }

            const Double deltaSeconds = std::clamp(static_cast<Double>(args.DeltaSeconds), 0.0, MaxDeltaSeconds);
            const Double time = deltaSeconds * sender.GetTimeScale();
            if (time <= 0.0)
            {
                return;
            }

            const Double altitude = sender.GetAltitudeAgl();
            const Double heightError = autoState.targetAltitude - altitude;
            const Double desiredVelocity = std::clamp(HeightGain * heightError, -MaxClimbRate, MaxClimbRate);
            const Double verticalVelocity = (altitude - autoState.lastAltitude) / time;
            const Double velocityError = desiredVelocity - verticalVelocity;

            autoState.integral = std::clamp(autoState.integral + velocityError * time, -MaxIntegral, MaxIntegral);

            const Double throttle =
                std::clamp(VelocityGain * velocityError + IntegralGain * autoState.integral, 0.0, 1.0);

            pod.SetThrottle(throttle);
            autoState.lastAltitude = altitude;

            if (autoState.logCountdown <= 0)
            {
                LogFormat(logger, LogSeverity::Info, "AUTO target={:.1f} agl={:.1f} v={:.2f} u={:.3f}",
                    autoState.targetAltitude, altitude, verticalVelocity, throttle);
                autoState.logCountdown = LogIntervalFrames;
            }

            --autoState.logCountdown;
        }

        void OnDisposing([[maybe_unused]] RuntimeRef sender, [[maybe_unused]] const EventArgs& args) noexcept override
        {
            autoState.active = false;
            autoState.runtime = RuntimeRef{nullptr};
        }
    };

    AutoHook autoHook;
}

extern "C" __declspec(dllexport) Boolean SRModLoad(ILogger* target)
{
    logger = target;
    return true;
}

extern "C" __declspec(dllexport) RuntimeHookBase* SRModRuntimeHook()
{
    return &autoHook;
}
