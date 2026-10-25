#include "PanelLayer.h"
#include "SelectionLayer.h"

using namespace SimpleRockets;
using namespace SimpleRockets::SDK;
using namespace SimpleRockets::SDK::Game;
using namespace SimpleRockets::SDK::Log;
using namespace SimpleRockets::SDK::Mod;
using namespace SimpleRockets::SDK::Ui;

namespace PartMod
{
    namespace
    {
        /// <summary>HUD 开关按钮的文字</summary>
        constexpr const char* ButtonText = "Part";

        LayerRef panel{nullptr};
        LayerRef selectionLayer{nullptr};
        StateButtonRef toggleButton{nullptr};

        /// <summary>点击 HUD 按钮时切换面板</summary>
        void OnToggleClicked([[maybe_unused]] ButtonRef sender, [[maybe_unused]] const ClickEventArgs& args) noexcept
        {
            const Boolean visible = !panel.GetVisible();

            panel.SetVisible(visible);
            selectionLayer.SetVisible(visible);
            toggleButton.SetSelected(visible);

            if (!visible)
            {
                SelectionLayer::ClearSelection();
            }
        }

        class PartHook final : public RuntimeHookBase
        {
        public:
            void OnLoaded(RuntimeLayerRef sender, [[maybe_unused]] const EventArgs& args) noexcept override
            {
                ILogger& logger = Environment::GetLogger();

                // 创建选中层
                const LayerRef selectionLayerRef = CreateLayer<SelectionLayer>(sender, LayerEvent::Touch);
                if (selectionLayerRef.IsNull())
                {
                    if (logger.IsEnabled(LogSeverity::Error))
                    {
                        logger.Log(LogSeverity::Error, "选中层创建失败");
                    }
                    return;
                }

                // 配置选中层
                selectionLayerRef.SetSize(sender.GetWidth(), sender.GetHeight());
                selectionLayerRef.SetPosition(0, 0);
                selectionLayerRef.SetVisible(false);

                // 创建面板
                const LayerRef panelRef =
                    CreateLayer<PanelLayer>(sender, LayerEvent::Touch | LayerEvent::Update);
                if (panelRef.IsNull())
                {
                    if (logger.IsEnabled(LogSeverity::Error))
                    {
                        logger.Log(LogSeverity::Error, "面板创建失败");
                    }
                    return;
                }

                // 配置面板
                panelRef.SetPosition((sender.GetWidth() - PanelWidth) / 2,
                    sender.GetContentTop() - PanelHeight);
                panelRef.SetVisible(false);

                // 创建面板开关按钮
                const StateButtonRef toggleButtonRef = CreateStackedStateButton(sender, ButtonText, true);
                if (toggleButtonRef.IsNull())
                {
                    if (logger.IsEnabled(LogSeverity::Error))
                    {
                        logger.Log(LogSeverity::Error, "开关按钮创建失败");
                    }
                    return;
                }

                selectionLayer = selectionLayerRef;
                panel = panelRef;
                toggleButton = toggleButtonRef;
                toggleButton.SetClickHandler<OnToggleClicked>(nullptr);
            }

            /// <summary>场景初始化后登记 Runtime, 供选中层拾取时取用</summary>
            void OnInitialized(RuntimeRef sender, [[maybe_unused]] const EventArgs& args) noexcept override
            {
                SelectionLayer::SetRuntime(sender);
            }

            /// <summary>场景释放前清掉 Runtime 缓存与已经失效的句柄</summary>
            void OnDisposing([[maybe_unused]] RuntimeRef sender, [[maybe_unused]] const EventArgs& args) noexcept override
            {
                SelectionLayer::ClearSelection();
                SelectionLayer::SetRuntime(RuntimeRef{nullptr});

                selectionLayer = LayerRef{nullptr};
                panel = LayerRef{nullptr};
                toggleButton = StateButtonRef{nullptr};
            }
        };

        PartHook partHook;
    }
}

extern "C" __declspec(dllexport) Boolean SRModLoad(Environment* environment)
{
    Environment::Attach(environment);
    return true;
}

extern "C" __declspec(dllexport) RuntimeHookBase* SRModRuntimeHook()
{
    return &PartMod::partHook;
}
