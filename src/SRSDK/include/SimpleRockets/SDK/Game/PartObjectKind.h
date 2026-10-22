#pragma once

#include "SimpleRockets/SDK/Game/Layout/Rvas.h"
#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>
    /// 零件实现类标识, 取值即对应虚表 Rva
    ///
    /// 只列游戏里实际存在的派生类; 不含 PartObject 基类与抽象的 DockingObject
    /// </summary>
    enum class PartObjectKind : Rva
    {
        PodObject = Layout::RvaVtablePodObject,
        TankObject = Layout::RvaVtableTankObject,
        EngineObject = Layout::RvaVtableEngineObject,
        FixtureObject = Layout::RvaVtableFixtureObject,
        RcsObject = Layout::RvaVtableRcsObject,
        SolarPanelObject = Layout::RvaVtableSolarPanelObject,
        ParachuteObject = Layout::RvaVtableParachuteObject,
        WheelObject = Layout::RvaVtableWheelObject,
        DockPortObject = Layout::RvaVtableDockPortObject,
        DockConnectorObject = Layout::RvaVtableDockConnectorObject,
        DetacherObject = Layout::RvaVtableDetacherObject,
        LanderObject = Layout::RvaVtableLanderObject,
    };
}
