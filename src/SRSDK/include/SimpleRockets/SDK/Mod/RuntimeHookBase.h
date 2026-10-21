#pragma once

#include "SimpleRockets/SDK/Game/PodObjectRef.h"
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

        /// <summary>
        /// Runtime 构造完成后调用
        /// Runtime::Initialize
        /// </summary>
        /// <param name="runtime">新建的 Runtime, 尚无有效世界数据</param>
        virtual void OnInitialized([[maybe_unused]] Game::RuntimeRef& runtime) noexcept { }

        /// <summary>
        /// 每帧更新前调用
        /// Runtime::HandleUpdate
        /// </summary>
        /// <param name="runtime">当前 Runtime, 此处写入的字段会被本帧游戏逻辑读取</param>
        virtual void OnUpdating([[maybe_unused]] Game::RuntimeRef& runtime) noexcept { }

        /// <summary>
        /// 每帧更新完成后调用
        /// Runtime::HandleUpdate
        /// </summary>
        /// <param name="runtime">当前 Runtime, 读到的字段为本帧计算结果</param>
        virtual void OnUpdated([[maybe_unused]] Game::RuntimeRef& runtime) noexcept { }

        /// <summary>
        /// 世界序列化完成后调用
        /// Runtime::SaveXml
        /// </summary>
        /// <param name="runtime">当前 Runtime, 状态已写入文档</param>
        virtual void OnSaved([[maybe_unused]] Game::RuntimeRef& runtime) noexcept { }

        /// <summary>
        /// 分级成功后调用
        /// ActivateNextStage
        /// </summary>
        /// <param name="pod">执行分级的 PodObject, 分级后其分级索引已递增</param>
        virtual void OnStageActivated([[maybe_unused]] Game::PodObjectRef& pod) noexcept { }

        /// <summary>
        /// Runtime 析构前调用
        /// Runtime::~Runtime
        /// </summary>
        /// <param name="runtime">即将销毁的 Runtime, 此时字段仍可读</param>
        virtual void OnDestroying([[maybe_unused]] Game::RuntimeRef& runtime) noexcept { }
    };

    inline RuntimeHookBase::~RuntimeHookBase() = default;
}
