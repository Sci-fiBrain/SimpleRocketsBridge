#include "SimpleRockets/SRSDK.h"

using namespace SimpleRockets;
using namespace SimpleRockets::SDK;
using namespace SimpleRockets::SDK::Game;
using namespace SimpleRockets::SDK::Log;
using namespace SimpleRockets::SDK::Mod;

namespace
{
    class PartHook final : public RuntimeHookBase
    {
    public:
        void OnLoaded([[maybe_unused]] RuntimeLayerRef sender, [[maybe_unused]] const EventArgs& args) noexcept override
        {
            ILogger& logger = Environment::GetLogger();
            if (logger.IsEnabled(LogSeverity::Info))
            {
                logger.Log(LogSeverity::Info, "PartMod loaded");
            }
        }
    };

    PartHook partHook;
}

extern "C" __declspec(dllexport) Boolean SRModLoad(Environment* environment)
{
    Environment::Attach(environment);
    return true;
}

extern "C" __declspec(dllexport) RuntimeHookBase* SRModRuntimeHook()
{
    return &partHook;
}
