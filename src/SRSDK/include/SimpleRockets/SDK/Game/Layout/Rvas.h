#pragma once

#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Game::Layout
{
    /// <summary>Runtime::~Runtime, 相对游戏模块基址</summary>
    constexpr Rva RvaHookRuntimeDestructor = 0x3F090;

    /// <summary>Runtime::Initialize, 相对游戏模块基址</summary>
    constexpr Rva RvaHookInitialize = 0x3F230;

    /// <summary>Runtime::HandleUpdate, 相对游戏模块基址</summary>
    constexpr Rva RvaHookHandleUpdate = 0x3F7C0;

    /// <summary>Runtime::SaveXml, 相对游戏模块基址</summary>
    constexpr Rva RvaHookSaveXml = 0x3DF10;

    /// <summary>PodObject 推进分级的入口, 参数 (this)</summary>
    constexpr Rva RvaHookActivateNextStage = 0x5A870;

    /// <summary>RuntimeLayer 构造器, 建完层内全部 HUD 元素后返回 this</summary>
    constexpr Rva RvaHookLayerLoaded = 0x34DA0;

    /// <summary>StateButton 构造器, 参数 (this, const std::string& name), 返回 this</summary>
    constexpr Rva RvaStateButtonCtor = 0x4E2A0;

    /// <summary>cfw::Layer::AddChild, 参数 (this, child), 插入后触发一次布局</summary>
    constexpr Rva RvaLayerAddChild = 0xC18E0;

    /// <summary>cfw::Button 的点击分发, 位于虚表槽 33; 也用于反解模块基址</summary>
    constexpr Rva RvaButtonDispatchClick = 0x1E690;

    constexpr Rva RvaVtableRuntime = 0x1248D8;
    constexpr Rva RvaVtableRuntimeLayer = 0x124198;
    constexpr Rva RvaVtableLayer = 0x120ED4;
    constexpr Rva RvaVtableStateButton = 0x125B60;
    constexpr Rva RvaVtableCallback = 0x12421C;
    constexpr Rva RvaVtableNavCircle = 0x1256C4;

    constexpr Rva RvaVtableOrbitNode = 0x126AC4;
    constexpr Rva RvaVtableShipOrbitNode = 0x125AC4;
    constexpr Rva RvaVtablePlanet = 0x1268BC;
    constexpr Rva RvaVtableShip = 0x124670;

    constexpr Rva RvaVtablePartTree = 0x1260C0;
    constexpr Rva RvaVtablePartConnection = 0x126BA0;
    constexpr Rva RvaVtablePartObject = 0x125F10;
    constexpr Rva RvaVtablePodObject = 0x126390;
    constexpr Rva RvaVtableTankObject = 0x126820;
    constexpr Rva RvaVtableEngineObject = 0x12679C;
    constexpr Rva RvaVtableFixtureObject = 0x12BC8C;
    constexpr Rva RvaVtableRcsObject = 0x12BF9C;
    constexpr Rva RvaVtableSolarPanelObject = 0x12BE54;
    constexpr Rva RvaVtableParachuteObject = 0x12BF28;
    constexpr Rva RvaVtableWheelObject = 0x12BDC4;
    constexpr Rva RvaVtableDockPortObject = 0x12BD4C;
    constexpr Rva RvaVtableDockConnectorObject = 0x12BB30;
    constexpr Rva RvaVtableDetacherObject = 0x12BA88;
    constexpr Rva RvaVtableLanderObject = 0x12BC18;
}
