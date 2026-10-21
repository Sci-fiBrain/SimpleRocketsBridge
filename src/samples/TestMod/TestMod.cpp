#include <format>
#include <string>

#include "SimpleRockets/SDK/Game/Memory.h"
#include "SimpleRockets/SDK/Game/PodObjectRef.h"
#include "SimpleRockets/SDK/Game/RuntimeRef.h"
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

    class TestHook final : public RuntimeHookBase
    {
    public:
        void OnInitialized([[maybe_unused]] RuntimeRef& runtime) noexcept override { }

        void OnUpdated([[maybe_unused]] RuntimeRef& runtime) noexcept override
        {
            updateCount += 1;
            if (updateCount % ReportInterval != 0)
            {
                return;
            }
        }

    private:
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
