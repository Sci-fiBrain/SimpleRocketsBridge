#pragma once

#include <cstddef>
#include <new>

#include "SimpleRockets/SDK/Environment.h"
#include "SimpleRockets/SDK/EventHandler.h"
#include "SimpleRockets/SDK/Game/LayerRef.h"
#include "SimpleRockets/SDK/Game/Layout/Offsets.h"
#include "SimpleRockets/SDK/Game/Layout/Rvas.h"
#include "SimpleRockets/SDK/Game/Memory.h"
#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Ui
{
    // 以下事件类型名取自 so 符号, 内部字段未查清, 因此只做不完整声明;
    // 需要读数据时用 Game::Layout 里已实证的偏移
    struct BaseEvent;
    struct GestureEvent;
    struct MultiTouchEvent;
    struct UpdateEvent;
    struct ScrollEvent;
    struct KeyEvent;
    struct Bounds;

    /// <summary>触摸事件, 只声明已实证的字段</summary>
    struct TouchEvent
    {
        /// <summary>+0x00, 虚表指针, 见 cfw::TouchEvent 构造器</summary>
        void* Vtable;

        /// <summary>+0x04, 事件种类, 为 1 时是触摸</summary>
        Int32 Kind;

        /// <summary>+0x08, 按设备尺寸换算后的横坐标, 控件命中测试用</summary>
        Int32 HitTestX;

        /// <summary>+0x0C, 按设备尺寸换算后的纵坐标</summary>
        Int32 HitTestY;

        /// <summary>+0x10, 屏幕像素横坐标</summary>
        Int32 ScreenX;

        /// <summary>+0x14, 屏幕像素纵坐标</summary>
        Int32 ScreenY;

        /// <summary>+0x18, 触摸阶段</summary>
        Int32 Phase;
    };

    static_assert(offsetof(TouchEvent, Kind) == 0x04, "Kind 必须在 +0x04");
    static_assert(offsetof(TouchEvent, HitTestX) == 0x08, "HitTestX 必须在 +0x08");
    static_assert(offsetof(TouchEvent, HitTestY) == 0x0C, "HitTestY 必须在 +0x0C");
    static_assert(offsetof(TouchEvent, ScreenX) == 0x10, "ScreenX 必须在 +0x10");
    static_assert(offsetof(TouchEvent, ScreenY) == 0x14, "ScreenY 必须在 +0x14");
    static_assert(offsetof(TouchEvent, Phase) == 0x18, "Phase 必须在 +0x18");

    /// <summary>触摸阶段</summary>
    enum class TouchPhase : Int32
    {
        /// <summary>按下</summary>
        Down = 0,

        /// <summary>按住移动</summary>
        Move = 1,

        /// <summary>抬起</summary>
        Up = 2,
    };

    /// <summary>层可订阅的事件种类, 值即掩码中的位</summary>
    enum class LayerEvent : Int32
    {
        /// <summary>触摸, 命中测试通过后送达</summary>
        Touch = 1,

        /// <summary>多点触控</summary>
        MultiTouch = 2,

        /// <summary>每帧更新</summary>
        Update = 4,

        /// <summary>返回键, 掩码里设不设都会送达</summary>
        BackButton = 8,

        /// <summary>滚动</summary>
        Scroll = 0x10,

        /// <summary>手势</summary>
        Gesture = 0x20,

        /// <summary>按键</summary>
        Key = 0x40,
    };

    /// <summary>把两个事件合成订阅掩码</summary>
    [[nodiscard]] constexpr Int32 operator|(LayerEvent left, LayerEvent right) noexcept
    {
        return static_cast<Int32>(left) | static_cast<Int32>(right);
    }

    /// <summary>在已有掩码上追加一个事件</summary>
    [[nodiscard]] constexpr Int32 operator|(Int32 mask, LayerEvent event) noexcept
    {
        return mask | static_cast<Int32>(event);
    }

    /// <summary>触摸事件的参数</summary>
    struct TouchEventArgs : EventArgs
    {
        /// <summary>屏幕像素横坐标, 与 ShipViewRef::ScreenToWorld 的入参同一空间, 从屏幕左边向右计</summary>
        Int32 ScreenX;

        /// <summary>屏幕像素纵坐标, 从屏幕顶部向下计, 与层坐标的 y 轴方向相反</summary>
        Int32 ScreenY;

        /// <summary>命中测试用横坐标, 已按设备尺寸换算</summary>
        Int32 HitTestX;

        /// <summary>命中测试用纵坐标, 已按设备尺寸换算</summary>
        Int32 HitTestY;

        /// <summary>本次触摸所处的阶段</summary>
        TouchPhase Phase;

        /// <summary>置 true 表示本层吃掉事件, 不再传给其它层</summary>
        Boolean Handled;
    };

    /// <summary>创建层, 返回其视图; 对象由 SDK 分配、游戏持有</summary>
    /// <param name="parent">父层</param>
    /// <param name="subscriptionMask">事件订阅掩码, bit n 对应事件种类 2^n</param>
    template <typename TLayer>
    [[nodiscard]] Game::LayerRef CreateLayer(Game::LayerRef parent, Int32 subscriptionMask);

    /// <summary>
    /// 可继承的 UI 层基类, 对应游戏的 cfw::Layer
    ///
    /// 32 个虚槽与游戏虚表逐槽对齐, 默认实现全部转发给游戏自己的 cfw::Layer;
    /// 需要行为时覆写下方的事件钩子, 由本类把游戏事件翻译成标准参数后调用
    /// </summary>
    class LayerBase
    {
        template <typename TLayer>
        friend Game::LayerRef CreateLayer(Game::LayerRef parent, Int32 subscriptionMask);

    public:
        /// <summary>槽 0, 删除析构, 转发给 cfw::Layer 的析构</summary>
        /// <remarks>派生类的析构总会覆盖此槽, 由它先跑再转到这里</remarks>
        virtual ~LayerBase()
        {
            using Function = void(__fastcall*)(void* self, void* edx);
            const Function function = Environment::GetAddress<Function>(Game::Layout::RvaLayerDestructor);
            function(static_cast<void*>(this), nullptr);
        }

        /// <summary>槽 1, 事件自上而下透传给子层</summary>
        virtual Boolean TunnelEvent(BaseEvent* event) final
        {
            using Function = Boolean(__fastcall*)(void* self, void* edx, BaseEvent* event);
            const Function function = Environment::GetAddress<Function>(Game::Layout::RvaLayerTunnelEvent);
            return function(static_cast<void*>(this), nullptr, event);
        }

        /// <summary>槽 2, 按事件种类分发到对应的处理槽</summary>
        virtual Boolean HandleEvent(BaseEvent* event) final
        {
            using Function = Boolean(__fastcall*)(void* self, void* edx, BaseEvent* event);
            const Function function = Environment::GetAddress<Function>(Game::Layout::RvaLayerHandleEvent);
            return function(static_cast<void*>(this), nullptr, event);
        }

        /// <summary>槽 3, 事件自下而上冒泡</summary>
        virtual Boolean BubbleEvent(BaseEvent* event) final
        {
            using Function = Boolean(__fastcall*)(void* self, void* edx, BaseEvent* event);
            const Function function = Environment::GetAddress<Function>(Game::Layout::RvaLayerBubbleEvent);
            return function(static_cast<void*>(this), nullptr, event);
        }

        /// <summary>槽 4, 挂到父层时调用</summary>
        virtual void AddedToParent(LayerBase* parent) final
        {
            using Function = void(__fastcall*)(void* self, void* edx, LayerBase* parent);
            const Function function = Environment::GetAddress<Function>(Game::Layout::RvaLayerAddedToParent);
            function(static_cast<void*>(this), nullptr, parent);
        }

        /// <summary>槽 5, 按索引插入子层</summary>
        virtual void InsertChild(LayerBase* child, Int32 index) final
        {
            using Function = void(__fastcall*)(void* self, void* edx, LayerBase* child, Int32 index);
            const Function function = Environment::GetAddress<Function>(Game::Layout::RvaLayerInsertChild);
            function(static_cast<void*>(this), nullptr, child, index);
        }

        /// <summary>槽 6, 追加子层</summary>
        virtual void AddChild(LayerBase* child) final
        {
            using Function = void(__fastcall*)(void* self, void* edx, LayerBase* child);
            const Function function = Environment::GetAddress<Function>(Game::Layout::RvaLayerAddChild);
            function(static_cast<void*>(this), nullptr, child);
        }

        /// <summary>槽 7, 移除子层</summary>
        virtual void RemoveChild(LayerBase* child) final
        {
            using Function = void(__fastcall*)(void* self, void* edx, LayerBase* child);
            const Function function = Environment::GetAddress<Function>(Game::Layout::RvaLayerRemoveChild);
            function(static_cast<void*>(this), nullptr, child);
        }

        /// <summary>槽 8, 重算布局</summary>
        virtual void UpdateLayout() final
        {
            using Function = void(__fastcall*)(void* self, void* edx);
            const Function function = Environment::GetAddress<Function>(Game::Layout::RvaLayerUpdateLayout);
            function(static_cast<void*>(this), nullptr);
        }

        /// <summary>槽 9, 设置可见性</summary>
        virtual void SetVisible(Boolean visible) final
        {
            using Function = void(__fastcall*)(void* self, void* edx, Int32 visible);
            const Function function = Environment::GetAddress<Function>(Game::Layout::RvaLayerSetVisible);
            function(static_cast<void*>(this), nullptr, visible);
        }

        /// <summary>槽 10, 设置内边距</summary>
        virtual void SetPadding(const Bounds& padding) final
        {
            using Function = void(__fastcall*)(void* self, void* edx, const Bounds* padding);
            const Function function = Environment::GetAddress<Function>(Game::Layout::RvaLayerSetPadding);
            function(static_cast<void*>(this), nullptr, &padding);
        }

        /// <summary>槽 11, 宽</summary>
        [[nodiscard]] virtual Int32 GetWidth() const final
        {
            using Function = Int32(__fastcall*)(const void* self, void* edx);
            const Function function = Environment::GetAddress<Function>(Game::Layout::RvaLayerWidth);
            return function(this, nullptr);
        }

        /// <summary>槽 12, 高</summary>
        [[nodiscard]] virtual Int32 GetHeight() const final
        {
            using Function = Int32(__fastcall*)(const void* self, void* edx);
            const Function function = Environment::GetAddress<Function>(Game::Layout::RvaLayerHeight);
            return function(this, nullptr);
        }

        /// <summary>槽 13, 设置尺寸</summary>
        virtual void SetSize(Int32 width, Int32 height) final
        {
            using Function = void(__fastcall*)(void* self, void* edx, Int32 width, Int32 height);
            const Function function = Environment::GetAddress<Function>(Game::Layout::RvaLayerSetSize);
            function(static_cast<void*>(this), nullptr, width, height);
            OnSizeChanged(GetWidth(), GetHeight());
        }

        /// <summary>槽 14, 相对父层的横坐标</summary>
        [[nodiscard]] virtual Int32 GetX() const final
        {
            using Function = Int32(__fastcall*)(const void* self, void* edx);
            const Function function = Environment::GetAddress<Function>(Game::Layout::RvaLayerX);
            return function(this, nullptr);
        }

        /// <summary>槽 15, 相对父层的纵坐标</summary>
        [[nodiscard]] virtual Int32 GetY() const final
        {
            using Function = Int32(__fastcall*)(const void* self, void* edx);
            const Function function = Environment::GetAddress<Function>(Game::Layout::RvaLayerY);
            return function(this, nullptr);
        }

        /// <summary>槽 16, 设置位置</summary>
        virtual void SetPosition(Int32 x, Int32 y) final
        {
            using Function = void(__fastcall*)(void* self, void* edx, Int32 x, Int32 y);
            const Function function = Environment::GetAddress<Function>(Game::Layout::RvaLayerSetPosition);
            function(static_cast<void*>(this), nullptr, x, y);
        }

        /// <summary>槽 17, 在指定层内居中</summary>
        virtual void CenterInLayer(LayerBase* layer) final
        {
            using Function = void(__fastcall*)(void* self, void* edx, LayerBase* layer);
            const Function function = Environment::GetAddress<Function>(Game::Layout::RvaLayerCenterInLayer);
            function(static_cast<void*>(this), nullptr, layer);
        }

        /// <summary>槽 18, 屏幕横坐标</summary>
        [[nodiscard]] virtual Int32 GetScreenX() const final
        {
            using Function = Int32(__fastcall*)(const void* self, void* edx);
            const Function function = Environment::GetAddress<Function>(Game::Layout::RvaLayerScreenX);
            return function(this, nullptr);
        }

        /// <summary>槽 19, 屏幕纵坐标</summary>
        [[nodiscard]] virtual Int32 GetScreenY() const final
        {
            using Function = Int32(__fastcall*)(const void* self, void* edx);
            const Function function = Environment::GetAddress<Function>(Game::Layout::RvaLayerScreenY);
            return function(this, nullptr);
        }

        /// <summary>槽 20, 点是否落在本层内</summary>
        [[nodiscard]] virtual Boolean IsPointInLayer(Int32 x, Int32 y) const final
        {
            using Function = Boolean(__fastcall*)(const void* self, void* edx, Int32 x, Int32 y);
            const Function function = Environment::GetAddress<Function>(Game::Layout::RvaLayerIsPointInLayer);
            return function(this, nullptr, x, y);
        }

        /// <summary>槽 21, 递归绘制子树</summary>
        virtual void DrawHierarchy() final
        {
            using Function = void(__fastcall*)(void* self, void* edx);
            const Function function = Environment::GetAddress<Function>(Game::Layout::RvaLayerDrawHierarchy);
            function(static_cast<void*>(this), nullptr);
        }

        /// <summary>槽 22, 绘制自身; 转发给 OnDraw</summary>
        virtual void Draw() final
        {
            OnDraw();
        }

        /// <summary>槽 23, 绘制后回调</summary>
        virtual void PostDraw() final
        {
            using Function = void(__fastcall*)(void* self, void* edx);
            const Function function = Environment::GetAddress<Function>(Game::Layout::RvaLayerPostDraw);
            function(static_cast<void*>(this), nullptr);
        }

        /// <summary>槽 24, 触摸处理; 先过游戏的命中测试, 命中的才交给 OnTouch</summary>
        virtual Boolean HandleTouch(TouchEvent* event) final
        {
            using Function = Boolean(__fastcall*)(void* self, void* edx, TouchEvent* event);
            const Function function = Environment::GetAddress<Function>(Game::Layout::RvaLayerHandleTouch);
            const Boolean hit = function(static_cast<void*>(this), nullptr, event);
            if (!hit)
            {
                return false;
            }

            const TouchEvent& touch = *reinterpret_cast<const TouchEvent*>(event);

            TouchEventArgs args{};
            args.ScreenX = touch.ScreenX;
            args.ScreenY = touch.ScreenY;
            args.HitTestX = touch.HitTestX;
            args.HitTestY = touch.HitTestY;
            args.Phase = static_cast<TouchPhase>(touch.Phase);

            OnTouch(Game::LayerRef{static_cast<void*>(this)}, args);
            return args.Handled;
        }

        /// <summary>槽 25, 手势处理</summary>
        virtual Boolean HandleGesture(GestureEvent* event) final
        {
            using Function = Boolean(__fastcall*)(void* self, void* edx, GestureEvent* event);
            const Function function = Environment::GetAddress<Function>(Game::Layout::RvaLayerHandleGesture);
            return function(static_cast<void*>(this), nullptr, event);
        }

        /// <summary>槽 26, 多点触控处理</summary>
        virtual Boolean HandleMultiTouch(MultiTouchEvent* event) final
        {
            using Function = Boolean(__fastcall*)(void* self, void* edx, MultiTouchEvent* event);
            const Function function = Environment::GetAddress<Function>(Game::Layout::RvaLayerHandleMultiTouch);
            return function(static_cast<void*>(this), nullptr, event);
        }

        /// <summary>槽 27, 每帧更新</summary>
        virtual Boolean HandleUpdate(UpdateEvent* event) final
        {
            using Function = Boolean(__fastcall*)(void* self, void* edx, UpdateEvent* event);
            const Function function = Environment::GetAddress<Function>(Game::Layout::RvaLayerHandleUpdate);
            const Boolean handled = function(static_cast<void*>(this), nullptr, event);
            OnUpdate();
            return handled;
        }

        /// <summary>槽 28, 滚动处理</summary>
        virtual Boolean HandleScroll(ScrollEvent* event) final
        {
            using Function = Boolean(__fastcall*)(void* self, void* edx, ScrollEvent* event);
            const Function function = Environment::GetAddress<Function>(Game::Layout::RvaLayerHandleScroll);
            return function(static_cast<void*>(this), nullptr, event);
        }

        /// <summary>槽 29, 返回键</summary>
        virtual Boolean HandleBackButton() final
        {
            using Function = Boolean(__fastcall*)(void* self, void* edx);
            const Function function = Environment::GetAddress<Function>(Game::Layout::RvaLayerHandleBackButton);
            return function(static_cast<void*>(this), nullptr);
        }

        /// <summary>槽 30, 按键处理</summary>
        virtual Boolean HandleKey(KeyEvent* event) final
        {
            using Function = Boolean(__fastcall*)(void* self, void* edx, KeyEvent* event);
            const Function function = Environment::GetAddress<Function>(Game::Layout::RvaLayerHandleKey);
            return function(static_cast<void*>(this), nullptr, event);
        }

        /// <summary>槽 31, 是否把事件继续发给子层</summary>
        virtual Boolean CanTunnelEventToChildren(BaseEvent* event) final
        {
            using Function = Boolean(__fastcall*)(void* self, void* edx, BaseEvent* event);
            const Function function = Environment::GetAddress<Function>(Game::Layout::RvaLayerCanTunnelEventToChildren);
            return function(static_cast<void*>(this), nullptr, event);
        }

    protected:
        /// <summary>只能由派生类构造</summary>
        /// <param name="subscriptionMask">事件订阅掩码, 取 LayerEvent 的位或</param>
        explicit LayerBase(Int32 subscriptionMask)
        {
            using CtorFunction = void(__fastcall*)(void* self, void* edx, Int32 subscriptionMask);
            Environment::GetAddress<CtorFunction>(Game::Layout::RvaLayerCtor)(this, nullptr,
                subscriptionMask);
        }

        /// <summary>对象代表游戏内的一个层, 身份不可复制</summary>
        LayerBase(const LayerBase&) = delete;
        LayerBase& operator=(const LayerBase&) = delete;

        /// <summary>触摸事件, 只会在点落在本层内时触发</summary>
        /// <param name="args">置 Handled 为 true 表示本层吃掉事件</param>
        virtual void OnTouch([[maybe_unused]] Game::LayerRef sender,
            [[maybe_unused]] TouchEventArgs& args) noexcept
        {
        }

        /// <summary>绘制自身, 每帧在本层坐标系里调用, 原点在层左下角, x 向右 y 向上, 单位与层几何相同</summary>
        virtual void OnDraw() noexcept
        {
        }

        /// <summary>尺寸变化后调用, 参数为实际生效的宽高</summary>
        virtual void OnSizeChanged([[maybe_unused]] Int32 width, [[maybe_unused]] Int32 height) noexcept
        {
        }

        /// <summary>每帧更新后调用, 需在订阅掩码里包含 LayerEvent::Update</summary>
        virtual void OnUpdate() noexcept
        {
        }

    private:
        /// <summary>等宽填充</summary>
        Byte storage[0x5C];
    };

    static_assert(sizeof(LayerBase) == 0x60, "LayerBase 必须与游戏 cfw::Layer 同宽");

    template <typename TLayer>
    [[nodiscard]] Game::LayerRef CreateLayer(Game::LayerRef parent, Int32 subscriptionMask)
    {
        if (parent.IsNull())
        {
            return Game::LayerRef{nullptr};
        }

        TLayer* layer = new TLayer(subscriptionMask);
        parent.AddChild(Game::LayerRef{static_cast<void*>(layer)});
        return Game::LayerRef{static_cast<void*>(layer)};
    }

    template <typename TLayer>
    [[nodiscard]] Game::LayerRef CreateLayer(Game::LayerRef parent, LayerEvent events)
    {
        return CreateLayer<TLayer>(parent, static_cast<Int32>(events));
    }
}
