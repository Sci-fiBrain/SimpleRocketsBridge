#pragma once

/// <summary>
/// 可选的万能头, 一次引入全部公开头文件
///
/// mod 也可以按需逐个 include; include 本文件等于把整套头都编一遍
/// </summary>

#include "SimpleRockets/Types.h"
#include "SimpleRockets/SDK/EventHandler.h"
#include "SimpleRockets/SDK/Version.h"

#include "SimpleRockets/SDK/Log/ILogger.h"
#include "SimpleRockets/SDK/Log/LogRecord.h"

#include "SimpleRockets/SDK/Environment.h"

#include "SimpleRockets/SDK/Game/Layout/Offsets.h"
#include "SimpleRockets/SDK/Game/Layout/Rvas.h"
#include "SimpleRockets/SDK/Game/Layout/Sizes.h"
#include "SimpleRockets/SDK/Game/Layout/VtableSlots.h"

#include "SimpleRockets/SDK/Game/Memory.h"
#include "SimpleRockets/SDK/Game/Ref.h"
#include "SimpleRockets/SDK/Game/Types.h"
#include "SimpleRockets/SDK/Game/PartObjectKind.h"

#include "SimpleRockets/SDK/Game/LayerRef.h"
#include "SimpleRockets/SDK/Game/ButtonRef.h"
#include "SimpleRockets/SDK/Game/ImageButtonRef.h"
#include "SimpleRockets/SDK/Game/StateButtonRef.h"
#include "SimpleRockets/SDK/Game/NavCircleRef.h"
#include "SimpleRockets/SDK/Game/RuntimeLayerRef.h"
#include "SimpleRockets/SDK/Game/ShipViewRef.h"
#include "SimpleRockets/SDK/Game/RuntimeRef.h"

#include "SimpleRockets/SDK/Game/OrbitNodeRef.h"
#include "SimpleRockets/SDK/Game/PlanetRef.h"
#include "SimpleRockets/SDK/Game/ShipOrbitNodeRef.h"
#include "SimpleRockets/SDK/Game/ShipRef.h"
#include "SimpleRockets/SDK/Game/OrbitRef.h"

#include "SimpleRockets/SDK/Game/PartTreeRef.h"
#include "SimpleRockets/SDK/Game/PartObjectRef.h"
#include "SimpleRockets/SDK/Game/PodObjectRef.h"
#include "SimpleRockets/SDK/Game/TankObjectRef.h"

#include "SimpleRockets/SDK/Ui/LayerBase.h"
#include "SimpleRockets/SDK/Ui/StateButton.h"

#include "SimpleRockets/SDK/Mod/RuntimeHookBase.h"
