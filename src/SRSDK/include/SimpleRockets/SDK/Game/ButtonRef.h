#pragma once

#include <new>
#include <type_traits>

#include "SimpleRockets/SDK/Environment.h"
#include "SimpleRockets/SDK/EventHandler.h"
#include "SimpleRockets/SDK/Game/LayerRef.h"
#include "SimpleRockets/SDK/Game/Layout/Offsets.h"
#include "SimpleRockets/SDK/Game/Layout/Rvas.h"
#include "SimpleRockets/SDK/Game/Layout/Sizes.h"
#include "SimpleRockets/SDK/Game/Layout/VtableSlots.h"
#include "SimpleRockets/SDK/Game/Memory.h"
#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Game
{
    struct ButtonRef;

    /// <summary>按钮点击事件的参数</summary>
    struct ClickEventArgs : EventArgs
    {
        /// <summary>调用 SetClickHandler 时传入的 state</summary>
        void* state;

        explicit ClickEventArgs(void* state) noexcept : state{state} { }
    };

    /// <summary>按钮点击回调签名, void(ButtonRef, const ClickEventArgs&amp;)</summary>
    using ClickHandler = EventHandler<ButtonRef, const ClickEventArgs&>;

    /// <summary>把 ClickHandler 适配成游戏要求的点击回调 ABI</summary>
    template <auto THandler>
    struct ClickThunk
    {
        static void __fastcall Invoke(void* state, [[maybe_unused]] void* edx, ButtonRef sender) noexcept
        {
            THandler(sender, ClickEventArgs{state});
        }
    };

    /// <summary>
    /// 游戏 UI 的 cfw::Button 视图
    ///
    /// 在 Layer 之上加了命中判定与三个回调槽, 外观由派生类提供
    /// </summary>
    struct ButtonRef : LayerRef
    {
        explicit ButtonRef(void* pointer) noexcept : LayerRef{pointer} { }

        /// <summary>无回调时是否也参与事件分发</summary>
        [[nodiscard]] Boolean GetHandlesEventsWithoutCallback() const noexcept
        {
            return Read<Boolean>(pointer, Layout::OffsetButtonHandlesEventsWithoutCallback);
        }

        /// <summary>是否处于按下中</summary>
        [[nodiscard]] Boolean GetIsPressed() const noexcept
        {
            return Read<Boolean>(pointer, Layout::OffsetButtonIsPressed);
        }

        /// <summary>点击回调对象, 为空表示未挂</summary>
        [[nodiscard]] void* GetClickCallback() const noexcept
        {
            return Read<void*>(pointer, Layout::OffsetButtonClickCallback);
        }

        /// <summary>按下回调对象, 为空表示未挂</summary>
        [[nodiscard]] void* GetPressCallback() const noexcept
        {
            return Read<void*>(pointer, Layout::OffsetButtonPressCallback);
        }

        /// <summary>抬起回调对象, 为空表示未挂</summary>
        [[nodiscard]] void* GetReleaseCallback() const noexcept
        {
            return Read<void*>(pointer, Layout::OffsetButtonReleaseCallback);
        }

        /// <summary>点击时是否播放音效</summary>
        [[nodiscard]] Boolean GetPlaysClickSound() const noexcept
        {
            return Read<Boolean>(pointer, Layout::OffsetButtonPlaysClickSound);
        }

        /// <summary>
        /// 挂点击回调, 必须在游戏主线程调用
        ///
        /// 已有回调会被替换并交给游戏回收; 想单纯移除用 ClearClickHandler
        /// </summary>
        /// <typeparam name="THandler">签名必须与 ClickHandler 一致</typeparam>
        /// <param name="state">原样放进 ClickEventArgs 的 state, 可空</param>
        template <auto THandler>
        void SetClickHandler(void* state) const
        {
            static_assert(std::is_same_v<decltype(THandler), ClickHandler>,
                "THandler 必须是 void(ButtonRef, const ClickEventArgs&) noexcept");

            ClearClickHandler();

            void* callback = ::operator new(Layout::SizeofCallback);

            void* callbackVtable = Environment::GetAddress<void*>(Layout::RvaVtableCallback);
            Write<void*>(callback, Layout::OffsetCallbackVtable, callbackVtable);
            Write<void*>(callback, Layout::OffsetCallbackState, state);
            Write<void*>(callback, Layout::OffsetCallbackFunction, reinterpret_cast<void*>(&ClickThunk<THandler>::Invoke));

            Write<void*>(pointer, Layout::OffsetButtonClickCallback, callback);
        }

        /// <summary>清除点击回调, 已有委托对象交给游戏回收</summary>
        void ClearClickHandler() const noexcept
        {
            void* callback = GetClickCallback();
            if (callback == nullptr)
            {
                return;
            }

            using DestroyCallbackFunction = void(__fastcall*)(void* self, void* edx, Int32 freeFlag);
            const UInt32 vtable = Read<UInt32>(callback, 0);
            reinterpret_cast<DestroyCallbackFunction>(*reinterpret_cast<const UInt32*>(vtable))(callback, nullptr, 1);

            Write<void*>(pointer, Layout::OffsetButtonClickCallback, static_cast<void*>(nullptr));
        }
    };
}
