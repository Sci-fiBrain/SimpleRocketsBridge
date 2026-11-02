#include <algorithm>
#include <cmath>

#include "SimpleRockets/SRSDK.h"

using namespace SimpleRockets;
using namespace SimpleRockets::SDK;
using namespace SimpleRockets::SDK::Game;
using namespace SimpleRockets::SDK::Log;
using namespace SimpleRockets::SDK::Mod;

namespace
{
    /// <summary>AUTO 按钮的回调 state, 同时保存高度环与倾角指令的状量</summary>
    struct AutoState
    {
        RuntimeRef runtime{nullptr};
        RuntimeLayerRef layer{nullptr};
        Boolean active = false;
        Boolean captureTarget = false;
        Double targetDistance = 0.0;
        Double lastDistance = 0.0;
        Double integral = 0.0;
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

    /// <summary>切向速度到倾角的增益, 弧度每米每秒</summary>
    constexpr Double TangentialGain = 0.03;

    /// <summary>相对径向的最大倾角, 弧度</summary>
    constexpr Double MaxTilt = 0.5236;

    /// <summary>点击 AUTO: 激活时以下一帧的距行星中心距离为目标, 再次点击交还油门与转向</summary>
    void OnAutoClick(ButtonRef sender, const ClickEventArgs& args) noexcept
    {
        AutoState* state = static_cast<AutoState*>(args.state);
        const StateButtonRef button = sender.As<StateButtonRef>();
        const NavCircleRef navCircle = state->layer.GetNavCircle();

        state->active = !state->active;
        button.SetSelected(state->active);

        if (!state->active)
        {
            navCircle.UnlockHeading();
            return;
        }

        state->captureTarget = true;
        navCircle.LockHeading();
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
            autoState.layer = sender;

            const StateButtonRef button = Ui::CreateStackedStateButton(sender, "AUTO");
            if (button.IsNull())
            {
                ILogger& logger = Environment::GetLogger();
                if (logger.IsEnabled(LogSeverity::Error))
                {
                    logger.Log(LogSeverity::Error, "AUTO button creation failed");
                }
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

            const Vector2D position = playerNode.GetPosition();
            const Double distance = std::sqrt(position.x * position.x + position.y * position.y);
            if (distance <= 0.0)
            {
                return;
            }

            const Double radialX = position.x / distance;
            const Double radialY = position.y / distance;
            const Vector2D velocity = sender.GetVelocity();
            const Double tangentialVelocity = velocity.x * -radialY + velocity.y * radialX;
            const Double tilt = std::clamp(-TangentialGain * tangentialVelocity, -MaxTilt, MaxTilt);

            autoState.layer.GetNavCircle().SetLockedHeading(std::atan2(radialY, radialX) + tilt);

            if (autoState.captureTarget)
            {
                autoState.targetDistance = distance;
                autoState.lastDistance = distance;
                autoState.integral = 0.0;
                autoState.captureTarget = false;
            }

            const Double heightError = autoState.targetDistance - distance;
            const Double desiredVelocity = std::clamp(HeightGain * heightError, -MaxClimbRate, MaxClimbRate);
            const Double verticalVelocity = (distance - autoState.lastDistance) / time;
            const Double velocityError = desiredVelocity - verticalVelocity;

            autoState.integral = std::clamp(autoState.integral + velocityError * time, -MaxIntegral, MaxIntegral);

            const Double throttle =
                std::clamp(VelocityGain * velocityError + IntegralGain * autoState.integral, 0.0, 1.0);

            pod.SetThrottle(throttle);
            autoState.lastDistance = distance;
        }

        void OnDisposing([[maybe_unused]] RuntimeRef sender, [[maybe_unused]] const EventArgs& args) noexcept override
        {
            autoState.active = false;
            autoState.layer = RuntimeLayerRef{nullptr};
            autoState.runtime = RuntimeRef{nullptr};
        }
    };

    AutoHook autoHook;
}

extern "C" __declspec(dllexport) Boolean SRModLoad(Environment* environment)
{
    Environment::Attach(environment);
    return true;
}

extern "C" __declspec(dllexport) RuntimeHookBase* SRModRuntimeHook()
{
    return &autoHook;
}
