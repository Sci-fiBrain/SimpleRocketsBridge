#include <format>
#include <string>

#include "SimpleRockets/SDK/Game/Memory.h"
#include "SimpleRockets/SDK/Game/PodView.h"
#include "SimpleRockets/SDK/Game/RuntimeView.h"
#include "SimpleRockets/SDK/Game/ShipOrbitNodeView.h"
#include "SimpleRockets/SDK/Game/ShipView.h"
#include "SimpleRockets/SDK/Log/ILogger.h"
#include "SimpleRockets/SDK/Mod/RuntimeHookBase.h"
#include "SimpleRockets/Types.h"

using namespace SimpleRockets;
using namespace SimpleRockets::SDK::Game;
using namespace SimpleRockets::SDK::Log;
using namespace SimpleRockets::SDK::Mod;

namespace
{
    ILogger* logger = nullptr;

    void Log(const std::string& message)
    {
        if (logger != nullptr)
        {
            logger->Log(LogSeverity::Info, message);
        }
    }

    class TestHook final : public RuntimeHookBase
    {
    public:
        void OnInitialized(RuntimeView& runtime) noexcept override
        {
            Log(std::format("TestMod OnInitialized: 0x{:08X}", reinterpret_cast<UInt32>(runtime.runtime)));
        }

        void OnUpdated(RuntimeView& runtime) noexcept override
        {
            updateCount += 1;
            if (updateCount % ReportInterval != 0)
            {
                return;
            }

            void* pod = PlayerPod(runtime);
            const Double throttle = pod != nullptr ? Read<Double>(pod, OffsetPodThrottle) : -1.0;
            const Double elapsedTime = Read<Double>(runtime.runtime, OffsetRuntimeElapsedTime);

            Log(std::format(
                "TestMod OnUpdated: 累计 {} 帧, 油门 {:.3f}, 时间 {:.3f}s", updateCount, throttle, elapsedTime));
        }

        void OnSaved([[maybe_unused]] RuntimeView& runtime) noexcept override
        {
            Log("TestMod OnSaved");
        }

        void OnStageActivated(PodView& pod, Int32 stageIndex) noexcept override
        {
            Log(std::format(
                "TestMod OnStageActivated: pod=0x{:08X}, 阶段 {}", reinterpret_cast<UInt32>(pod.pod), stageIndex));
        }

        void OnDestroying([[maybe_unused]] RuntimeView& runtime) noexcept override
        {
            Log(std::format("TestMod OnDestroying: 累计 {} 帧", updateCount));
        }

    private:
        /// <summary>沿 Runtime → ShipOrbitNode → Ship → Pod 取玩家 Pod, 任一跳为空时返回空</summary>
        static void* PlayerPod(RuntimeView& runtime) noexcept
        {
            const void* node = Read<void*>(runtime.runtime, OffsetRuntimePlayerNode);
            if (node == nullptr)
            {
                return nullptr;
            }

            const void* ship = Read<void*>(node, OffsetNodeShip);
            if (ship == nullptr)
            {
                return nullptr;
            }

            return Read<void*>(ship, OffsetShipPod);
        }

        /// <summary>每多少帧报告一次, 约 110 帧每秒</summary>
        static constexpr UInt32 ReportInterval = 300;

        UInt32 updateCount = 0;
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
