#pragma once

#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>Runtime::~Runtime, 相对游戏模块基址</summary>
    constexpr Rva RvaHookRuntimeDestructor = 0x3F090;

    /// <summary>Runtime::Initialize, 相对游戏模块基址</summary>
    constexpr Rva RvaHookInitialize = 0x3F230;

    /// <summary>Runtime::HandleUpdate, 相对游戏模块基址</summary>
    constexpr Rva RvaHookHandleUpdate = 0x3F7C0;

    /// <summary>Runtime::SaveXml, 相对游戏模块基址</summary>
    constexpr Rva RvaHookSaveXml = 0x3DF10;

    /// <summary>分级成功路径, 相对游戏模块基址</summary>
    constexpr Rva RvaHookActivateStage = 0x5A870;

    constexpr Rva RvaVtableRuntime = 0x1248D8;
    constexpr Rva RvaVtableRuntimeLayer = 0x124198;
    constexpr Rva RvaVtableLayer = 0x120ED4;
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

    /// <summary>
    /// 零件实现类标识, 取值即对应虚表 Rva
    ///
    /// 只列游戏里实际存在的派生类; 不含 PartObject 基类与抽象的 DockingObject
    /// </summary>
    enum class PartObjectKind : Rva
    {
        PodObject = RvaVtablePodObject,
        TankObject = RvaVtableTankObject,
        EngineObject = RvaVtableEngineObject,
        FixtureObject = RvaVtableFixtureObject,
        RcsObject = RvaVtableRcsObject,
        SolarPanelObject = RvaVtableSolarPanelObject,
        ParachuteObject = RvaVtableParachuteObject,
        WheelObject = RvaVtableWheelObject,
        DockPortObject = RvaVtableDockPortObject,
        DockConnectorObject = RvaVtableDockConnectorObject,
        DetacherObject = RvaVtableDetacherObject,
        LanderObject = RvaVtableLanderObject,
    };
}
