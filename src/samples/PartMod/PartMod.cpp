#include <new>

#include "SimpleRockets/SRSDK.h"

using namespace SimpleRockets;
using namespace SimpleRockets::SDK;
using namespace SimpleRockets::SDK::Game;
using namespace SimpleRockets::SDK::Log;
using namespace SimpleRockets::SDK::Mod;
using namespace SimpleRockets::SDK::Ui;

namespace
{
    /// <summary>面板宽度</summary>
    constexpr Int32 PanelWidth = 380;

    /// <summary>面板高度</summary>
    constexpr Int32 PanelHeight = 145;

    /// <summary>面板相对父层的横坐标</summary>
    constexpr Int32 PanelX = 603;

    /// <summary>面板相对父层的纵坐标</summary>
    constexpr Int32 PanelY = 736;

    /// <summary>面板层, 目前只有底板</summary>
    class PanelLayer final : public LayerBase
    {
    public:
        explicit PanelLayer(Int32 subscriptionMask) noexcept
            : LayerBase(subscriptionMask),
              background(new (std::nothrow) Rectangle())
        {
            SetSize(PanelWidth, PanelHeight);
            SetPosition(PanelX, PanelY);
        }

        ~PanelLayer() noexcept override
        {
            delete background;
        }

    protected:
        /// <summary>底板铺满整层</summary>
        void OnSizeChanged(Int32 width, Int32 height) noexcept override
        {
            if (background == nullptr || width <= 0 || height <= 0)
            {
                return;
            }

            background->SetBounds(0, 0, width, height);
        }

        /// <summary>绘制底板</summary>
        void OnDraw() noexcept override
        {
            if (background == nullptr)
            {
                return;
            }

            background->Draw();
        }

        /// <summary>面板内的触摸由面板独占, 不再冒泡到下层</summary>
        void OnTouch([[maybe_unused]] Game::LayerRef sender, TouchEventArgs& args) noexcept override
        {
            args.Handled = true;
        }

    private:
        Rectangle* background;
    };

    class PartHook final : public RuntimeHookBase
    {
    public:
        void OnLoaded(RuntimeLayerRef sender, [[maybe_unused]] const EventArgs& args) noexcept override
        {
            ILogger& logger = Environment::GetLogger();

            const LayerRef panel = CreateLayer<PanelLayer>(sender, LayerEvent::Touch);
            if (panel.IsNull())
            {
                if (logger.IsEnabled(LogSeverity::Error))
                {
                    logger.Log(LogSeverity::Error, "面板创建失败");
                }
                return;
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
