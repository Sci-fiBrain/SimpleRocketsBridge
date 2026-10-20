#include <format>
#include <string>

#include "SimpleRockets/SDK/Game/Memory.h"
#include "SimpleRockets/SDK/Game/Offsets.h"
#include "SimpleRockets/SDK/Game/PodView.h"
#include "SimpleRockets/SDK/Game/RuntimeView.h"
#include "SimpleRockets/SDK/Log/ILogger.h"
#include "SimpleRockets/SDK/Mod/RuntimeHookBase.h"
#include "SimpleRockets/Types.h"

namespace
{
    using SimpleRockets::Boolean;
    using SimpleRockets::Double;
    using SimpleRockets::Int32;

    SimpleRockets::SDK::Log::ILogger* logger = nullptr;

    void Log(const std::string& message)
    {
        if (logger != nullptr)
        {
            logger->Log(SimpleRockets::SDK::Log::LogSeverity::Info, message);
        }
    }

    class TestHook final : public SimpleRockets::SDK::Mod::RuntimeHookBase
    {
    public:
        void OnInitialized(SimpleRockets::SDK::Game::RuntimeView& runtime) noexcept override
        {
            Log(std::format(
                "TestMod OnInitialized: 0x{:08X}", reinterpret_cast<SimpleRockets::UInt32>(runtime.runtime)));
        }

        void OnUpdated(SimpleRockets::SDK::Game::RuntimeView& runtime) noexcept override
        {
            updateCount += 1;
            if (updateCount % ReportInterval != 0)
            {
                return;
            }

            const Double elapsedTime = SimpleRockets::SDK::Game::Read<Double>(
                runtime.runtime, SimpleRockets::SDK::Game::OffsetRuntimeElapsedTime);
            const Double timeScale = SimpleRockets::SDK::Game::Read<Double>(
                runtime.runtime, SimpleRockets::SDK::Game::OffsetRuntimeTimeScale);

            Log(std::format(
                "TestMod OnUpdated: 累计 {} 帧, 时间 {:.3f}s, 倍率 {}", updateCount, elapsedTime, timeScale));
        }

        void OnSaved([[maybe_unused]] SimpleRockets::SDK::Game::RuntimeView& runtime) noexcept override
        {
            Log("TestMod OnSaved");
        }

        void OnStageActivated(SimpleRockets::SDK::Game::PodView& pod, Int32 stageIndex) noexcept override
        {
            Log(std::format("TestMod OnStageActivated: pod=0x{:08X}, 阶段 {}",
                reinterpret_cast<SimpleRockets::UInt32>(pod.pod), stageIndex));
        }

        void OnDestroying([[maybe_unused]] SimpleRockets::SDK::Game::RuntimeView& runtime) noexcept override
        {
            Log(std::format("TestMod OnDestroying: 累计 {} 帧", updateCount));
        }

    private:
        /// <summary>每多少帧报告一次, 约 110 帧每秒</summary>
        static constexpr SimpleRockets::UInt32 ReportInterval = 300;

        SimpleRockets::UInt32 updateCount = 0;
    };

    TestHook testHook;
}

extern "C" __declspec(dllexport) Boolean SRModLoad(SimpleRockets::SDK::Log::ILogger* target)
{
    logger = target;
    return true;
}

extern "C" __declspec(dllexport) SimpleRockets::SDK::Mod::RuntimeHookBase* SRModRuntimeHook()
{
    return &testHook;
}
