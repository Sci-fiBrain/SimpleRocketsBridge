#pragma once

#include <span>

#include "SimpleRockets/SDK/Log/ILogger.h"
#include "SimpleRockets/Types.h"

namespace SimpleRockets::Bridge::Infrastructure::Hook::Detours
{
    // detour 签名约定
    //   游戏函数都是 __thiscall: this 在 ECX, 其余参数在栈上、由 callee 清栈
    //   __fastcall 前两个参数正好落在 ECX 与 EDX, 因此第二参固定为 UnusedEdx 占位
    //   其余参数按原顺序跟在后面

    /// <summary>__thiscall 的 EDX 槽位, 值无定义且 detour 不读取, 仅用于让 __fastcall 与之一致</summary>
    using UnusedEdx = void*;

    /// <summary>
    /// 调用 Runtime::Initialize(RuntimeParams*)
    /// </summary>
    using InitializeFn = void(__thiscall*)(void* self, void* parameters);

    /// <summary>
    /// 调用 Runtime::HandleUpdate(float)
    /// </summary>
    using HandleUpdateFn = void(__thiscall*)(void* self, float deltaSeconds);

    /// <summary>
    /// 调用 Runtime::~Runtime()
    /// </summary>
    using RuntimeDestructorFn = void(__thiscall*)(void* self);

    /// <summary>
    /// 调用 Runtime::SaveXml(XMLDocument&)
    /// </summary>
    using SaveXmlFn = void(__thiscall*)(void* self, void* document);

    /// <summary>
    /// 调用 ActivateNextStage()
    /// </summary>
    using ActivateStageFn = void(__thiscall*)(void* self);

    /// <summary>原函数入口, 由 MinHook 在创建 hook 时填入</summary>
    extern InitializeFn initializeOriginal;
    extern HandleUpdateFn handleUpdateOriginal;
    extern RuntimeDestructorFn runtimeDestructorOriginal;
    extern SaveXmlFn saveXmlOriginal;
    extern ActivateStageFn activateStageOriginal;

    /// <summary>注入日志接口, 空表示 detour 不再写日志</summary>
    void SetLogger(SDK::Log::ILogger* logger);

    /// <summary>清空 detour 持有的全部状态</summary>
    void Reset();

    /// <summary>
    /// 拦截 Runtime 初始化
    /// Runtime::Initialize
    /// </summary>
    /// <param name="self">新建的 Runtime</param>
    /// <param name="parameters">运行时构造参数</param>
    void __fastcall DetourInitialize(void* self, UnusedEdx, void* parameters);

    /// <summary>
    /// 拦截每帧更新
    /// Runtime::HandleUpdate
    /// </summary>
    /// <param name="self">Runtime 对象</param>
    /// <param name="deltaSeconds">本帧时间增量</param>
    void __fastcall DetourHandleUpdate(void* self, UnusedEdx, float deltaSeconds);

    /// <summary>
    /// 拦截 Runtime 析构
    /// Runtime::~Runtime
    /// </summary>
    /// <param name="self">Runtime 对象, 此时尚未销毁</param>
    void __fastcall DetourRuntimeDestructor(void* self, UnusedEdx);

    /// <summary>
    /// 拦截世界序列化
    /// Runtime::SaveXml
    /// </summary>
    /// <param name="self">Runtime 对象</param>
    /// <param name="document">接收序列化结果的 XML 文档</param>
    void __fastcall DetourSaveXml(void* self, UnusedEdx, void* document);

    /// <summary>
    /// 拦截分级成功路径
    /// ActivateNextStage
    /// </summary>
    /// <param name="self">玩家飞船的 PodObject, 分级后其阶段索引已递增</param>
    void __fastcall DetourActivateStage(void* self, UnusedEdx);

    /// <summary>一条 hook 记录, 目标地址由模块基址加 RVA 得到</summary>
    struct HookEntry
    {
        /// <summary>目标函数相对游戏模块基址的偏移</summary>
        Rva rva;

        /// <summary>detour 函数地址, 实际类型为对应的 xxxFn 等价签名</summary>
        void* detour;

        /// <summary>原函数入口变量的地址, 由 MinHook 回填, 实际类型为 xxxFn*</summary>
        void** original;
    };

    /// <summary>全部待安装的 hook, 顺序即安装顺序</summary>
    [[nodiscard]] std::span<const HookEntry> GetEntries();
}
