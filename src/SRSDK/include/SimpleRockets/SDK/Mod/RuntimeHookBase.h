#pragma once

#include "SimpleRockets/SDK/EventHandler.h"
#include "SimpleRockets/SDK/Game/PodObjectRef.h"
#include "SimpleRockets/SDK/Game/RuntimeLayerRef.h"
#include "SimpleRockets/SDK/Game/RuntimeRef.h"

namespace SimpleRockets::SDK::Mod
{
    /// <summary>
    /// Runtime 场景的事件接收基类, 派生类只覆写关心的事件
    ///
    /// 全部回调都在游戏主线程同步执行, 必须快速返回
    /// </summary>
    class RuntimeHookBase
    {
    protected:
        RuntimeHookBase() = default;

    public:
        virtual ~RuntimeHookBase() = 0;

        /// <summary>Runtime 构造完成后调用, 对应 Runtime::Initialize</summary>
        virtual void OnInitialized([[maybe_unused]] Game::RuntimeRef sender,
            [[maybe_unused]] const EventArgs& args) noexcept { }

        /// <summary>UI 层构造完成时调用, sender 是已挂齐子层的新层; 晚于 OnInitialized</summary>
        virtual void OnLoaded([[maybe_unused]] Game::RuntimeLayerRef sender,
            [[maybe_unused]] const EventArgs& args) noexcept { }

        /// <summary>每帧更新前调用, 对应 Runtime::HandleUpdate; 此刻写入的字段会被本帧读取</summary>
        virtual void OnUpdating([[maybe_unused]] Game::RuntimeRef sender,
            [[maybe_unused]] const EventArgs& args) noexcept { }

        /// <summary>每帧更新完成后调用, 对应 Runtime::HandleUpdate</summary>
        virtual void OnUpdated([[maybe_unused]] Game::RuntimeRef sender,
            [[maybe_unused]] const EventArgs& args) noexcept { }

        /// <summary>世界序列化完成后调用, 对应 Runtime::SaveXml</summary>
        virtual void OnSaved([[maybe_unused]] Game::RuntimeRef sender,
            [[maybe_unused]] const EventArgs& args) noexcept { }

        /// <summary>分级推进后调用, 对应 PodObject::ActivateNextStage</summary>
        virtual void OnNextStageActivated([[maybe_unused]] Game::PodObjectRef sender,
            [[maybe_unused]] const EventArgs& args) noexcept { }

        /// <summary>Runtime 释放前调用, 对应 Runtime::~Runtime</summary>
        virtual void OnDisposing([[maybe_unused]] Game::RuntimeRef sender,
            [[maybe_unused]] const EventArgs& args) noexcept { }
    };

    inline RuntimeHookBase::~RuntimeHookBase() = default;
}
