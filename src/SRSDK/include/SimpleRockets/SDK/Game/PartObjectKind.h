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

    /// <summary>全部零件实现类, 供逐个判定用</summary>
    constexpr PartObjectKind PartObjectKinds[] =
    {
        PartObjectKind::PodObject,
        PartObjectKind::TankObject,
        PartObjectKind::EngineObject,
        PartObjectKind::FixtureObject,
        PartObjectKind::RcsObject,
        PartObjectKind::SolarPanelObject,
        PartObjectKind::ParachuteObject,
        PartObjectKind::WheelObject,
        PartObjectKind::DockPortObject,
        PartObjectKind::DockConnectorObject,
        PartObjectKind::DetacherObject,
        PartObjectKind::LanderObject,
    };

    /// <summary>零件实现类的显示名</summary>
    [[nodiscard]] constexpr const char* GetPartObjectKindName(PartObjectKind kind) noexcept
    {
        switch (kind)
        {
        case PartObjectKind::PodObject: return "Pod";
        case PartObjectKind::TankObject: return "Tank";
        case PartObjectKind::EngineObject: return "Engine";
        case PartObjectKind::FixtureObject: return "Fixture";
        case PartObjectKind::RcsObject: return "RCS";
        case PartObjectKind::SolarPanelObject: return "Solar Panel";
        case PartObjectKind::ParachuteObject: return "Parachute";
        case PartObjectKind::WheelObject: return "Wheel";
        case PartObjectKind::DockPortObject: return "Dock Port";
        case PartObjectKind::DockConnectorObject: return "Dock Connector";
        case PartObjectKind::DetacherObject: return "Detacher";
        case PartObjectKind::LanderObject: return "Lander";
        }

        return "Part";
    }
}
