#pragma once

#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Game::Layout
{
    constexpr Offset OffsetRuntimeLayer = 0x008;
    constexpr Offset OffsetRuntimePlayerNode = 0x00C;
    constexpr Offset OffsetRuntimeElapsedTime = 0x010;
    constexpr Offset OffsetRuntimeRootNode = 0x018;
    constexpr Offset OffsetRuntimeLocalCenter = 0x020;
    constexpr Offset OffsetRuntimeVelocity = 0x030;
    constexpr Offset OffsetRuntimeAttitude = 0x040;
    constexpr Offset OffsetRuntimeSpeed = 0x048;
    constexpr Offset OffsetRuntimeAngularRate = 0x050;
    constexpr Offset OffsetRuntimeTimeScale = 0x078;
    constexpr Offset OffsetRuntimeTimeScaleLevel = 0x080;
    constexpr Offset OffsetRuntimeAltitude = 0x088;
    constexpr Offset OffsetRuntimeAltitudeAgl = 0x090;
    constexpr Offset OffsetRuntimePrimaryNode = 0x0B4;
    constexpr Offset OffsetRuntimeInWarp = 0x0C0;
    constexpr Offset OffsetRuntimeFirstStageActivated = 0x0D9;

    constexpr Offset OffsetOrbitNodeChildren = 0x104;

    constexpr Offset OffsetShipOrbitNodePlanet = 0x0FC;
    constexpr Offset OffsetShipOrbitNodeVelocity = 0x140;
    constexpr Offset OffsetShipOrbitNodeShipId = 0x15C;
    constexpr Offset OffsetShipOrbitNodeShip = 0x160;
    constexpr Offset OffsetShipOrbitNodeOrbit = 0x168;
    constexpr Offset OffsetShipOrbitNodePosition = 0x170;
    constexpr Offset OffsetShipOrbitNodeOrbitValid = 0x184;

    constexpr Offset OffsetShipPartTree = 0x008;
    constexpr Offset OffsetShipPodObject = 0x024;
    constexpr Offset OffsetShipFuelRatio = 0x050;

    constexpr Offset OffsetPodObjectThrottle = 0x200;
    constexpr Offset OffsetPodObjectStageIndex = 0x234;

    constexpr Offset OffsetPartObjectPartTree = 0x00C;

    constexpr Offset OffsetPartTreeConnections = 0x01C;
    constexpr Offset OffsetPartTreePart = 0x1F0;

    constexpr Offset OffsetPartConnectionParentTree = 0x014;
    constexpr Offset OffsetPartConnectionChildTree = 0x01C;

    constexpr Offset OffsetTankObjectFuel = 0x200;
    constexpr Offset OffsetTankObjectCapacity = 0x208;
    constexpr Offset OffsetTankObjectFullMass = 0x210;
    constexpr Offset OffsetTankObjectDryMass = 0x218;

    constexpr Offset OffsetLayerVisible = 0x004;
    constexpr Offset OffsetLayerX = 0x010;
    constexpr Offset OffsetLayerY = 0x014;
    constexpr Offset OffsetLayerWidth = 0x018;
    constexpr Offset OffsetLayerHeight = 0x01C;
    constexpr Offset OffsetLayerChildren = 0x050;

    constexpr Offset OffsetRuntimeLayerRcsButton = 0x0B8;
    constexpr Offset OffsetRuntimeLayerSolarButton = 0x0BC;
    constexpr Offset OffsetRuntimeLayerFuelButton = 0x0C0;
    constexpr Offset OffsetRuntimeLayerUndockButton = 0x0C4;
    constexpr Offset OffsetRuntimeLayerMapButton = 0x0C8;
    constexpr Offset OffsetRuntimeLayerSplitButton = 0x0CC;

    constexpr Offset OffsetButtonHandlesEventsWithoutCallback = 0x060;
    constexpr Offset OffsetButtonIsPressed = 0x061;
    constexpr Offset OffsetButtonClickCallback = 0x064;
    constexpr Offset OffsetButtonPressCallback = 0x068;
    constexpr Offset OffsetButtonReleaseCallback = 0x06C;
    constexpr Offset OffsetButtonPlaysClickSound = 0x070;

    constexpr Offset OffsetCallbackVtable = 0x000;
    constexpr Offset OffsetCallbackState = 0x004;
    constexpr Offset OffsetCallbackFunction = 0x008;

    constexpr Offset OffsetImageButtonSelected = 0x098;
}
