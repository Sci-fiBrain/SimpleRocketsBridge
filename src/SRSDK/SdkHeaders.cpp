/// <summary>
/// SDK 头文件的编译期检查目标
///
/// SRSDK 是纯头库, 本文件让头文件语法与 static_assert 参与构建
/// </summary>

#include <type_traits>

#include "SimpleRockets/SDK/Log/ILogger.h"
#include "SimpleRockets/SDK/Log/LogRecord.h"
#include "SimpleRockets/SDK/Version.h"
#include "SimpleRockets/Types.h"

#include "SimpleRockets/SDK/Game/Memory.h"
#include "SimpleRockets/SDK/Game/Layout/Offsets.h"
#include "SimpleRockets/SDK/Game/ButtonRef.h"
#include "SimpleRockets/SDK/Game/ImageButtonRef.h"
#include "SimpleRockets/SDK/Game/LayerRef.h"
#include "SimpleRockets/SDK/Game/OrbitNodeRef.h"
#include "SimpleRockets/SDK/Game/OrbitRef.h"
#include "SimpleRockets/SDK/Game/PartObjectKind.h"
#include "SimpleRockets/SDK/Game/PartObjectRef.h"
#include "SimpleRockets/SDK/Game/PartTreeRef.h"
#include "SimpleRockets/SDK/Game/PlanetRef.h"
#include "SimpleRockets/SDK/Game/PodObjectRef.h"
#include "SimpleRockets/SDK/Game/Ref.h"
#include "SimpleRockets/SDK/Game/RuntimeLayerRef.h"
#include "SimpleRockets/SDK/Game/RuntimeRef.h"
#include "SimpleRockets/SDK/Game/ShipOrbitNodeRef.h"
#include "SimpleRockets/SDK/Game/ShipRef.h"
#include "SimpleRockets/SDK/Game/Layout/Sizes.h"
#include "SimpleRockets/SDK/Game/StateButtonRef.h"
#include "SimpleRockets/SDK/Game/TankObjectRef.h"
#include "SimpleRockets/SDK/Game/Types.h"
#include "SimpleRockets/SDK/Game/Layout/VtableSlots.h"
#include "SimpleRockets/SDK/Mod/RuntimeHookBase.h"

namespace
{
    class NullLogger final : public SimpleRockets::SDK::Log::ILogger
    {
    public:
        [[nodiscard]] SimpleRockets::Boolean IsEnabled(SimpleRockets::SDK::Log::LogSeverity) const noexcept override
        {
            return false;
        }

        void Log(SimpleRockets::SDK::Log::LogSeverity, std::string_view) noexcept override { }
    };

    static_assert(std::is_constructible_v<NullLogger>, "ILogger derived types must stay default constructible");

    [[maybe_unused]] void ExerciseOrbitNodeRef(SimpleRockets::SDK::Game::OrbitNodeRef node)
    {
        node.ForEachChild([](SimpleRockets::SDK::Game::OrbitNodeRef child) { ExerciseOrbitNodeRef(child); });
    }

    [[maybe_unused]] void ExercisePartTreeRef(SimpleRockets::SDK::Game::PartTreeRef tree)
    {
        tree.ForEachChildConnection([](SimpleRockets::SDK::Game::PartConnectionRef connection)
            { ExercisePartTreeRef(connection.GetChildTree()); });
    }

    [[maybe_unused]] void ExerciseShipRef(SimpleRockets::SDK::Game::ShipRef ship)
    {
        ship.ForEachPart([](SimpleRockets::SDK::Game::PartObjectRef part) { (void)part; });
    }

    [[maybe_unused]] void ExerciseStateButtonRef(SimpleRockets::SDK::Game::RuntimeLayerRef layer)
    {
        const SimpleRockets::SDK::Game::StateButtonRef button =
            SimpleRockets::SDK::Game::StateButtonRef::Create(layer, "FULL", 0, 0);

        button.SetPosition(0, 0);
        button.SetSize(1, 1);
        button.SetSelected(true);
        button.SetClickHandler(nullptr, nullptr);
        button.ClearClickHandler();

        (void)button.GetWidth();
        (void)button.GetHeight();
        (void)button.GetSelected();
        (void)button.GetIsPressed();
    }

    static_assert(std::is_abstract_v<SimpleRockets::SDK::Mod::RuntimeHookBase>, "RuntimeHookBase must stay abstract");
    static_assert(sizeof(SimpleRockets::SDK::Game::Ref) == sizeof(void*), "Ref must carry only the raw pointer");
    static_assert(std::is_base_of_v<SimpleRockets::SDK::Game::Ref, SimpleRockets::SDK::Game::RuntimeRef>,
        "RuntimeRef must derive from Ref");
    static_assert(std::is_base_of_v<SimpleRockets::SDK::Game::Ref, SimpleRockets::SDK::Game::RuntimeLayerRef>,
        "RuntimeLayerRef must derive from Ref");
    static_assert(
        sizeof(SimpleRockets::SDK::Game::RuntimeLayerRef) == sizeof(void*),
        "RuntimeLayerRef must carry only the raw pointer");
    static_assert(
        sizeof(SimpleRockets::SDK::Game::RuntimeRef) == sizeof(void*), "RuntimeRef must carry only the raw pointer");
    static_assert(std::is_base_of_v<SimpleRockets::SDK::Game::PartObjectRef, SimpleRockets::SDK::Game::PodObjectRef>,
        "PodObjectRef must derive from PartObjectRef");
    static_assert(
        sizeof(SimpleRockets::SDK::Game::PodObjectRef) == sizeof(void*), "PodObjectRef must carry only the raw pointer");
    static_assert(
        std::is_base_of_v<SimpleRockets::SDK::Game::OrbitNodeRef, SimpleRockets::SDK::Game::PlanetRef>,
        "PlanetRef must derive from OrbitNodeRef");
    static_assert(
        std::is_base_of_v<SimpleRockets::SDK::Game::OrbitNodeRef, SimpleRockets::SDK::Game::ShipOrbitNodeRef>,
        "ShipOrbitNodeRef must derive from OrbitNodeRef");
    static_assert(
        sizeof(SimpleRockets::SDK::Game::ShipOrbitNodeRef) == sizeof(void*), "ShipOrbitNodeRef must carry only the raw pointer");
    static_assert(
        std::is_base_of_v<SimpleRockets::SDK::Game::Ref, SimpleRockets::SDK::Game::ShipRef>, "ShipRef must derive from Ref");
    static_assert(
        sizeof(SimpleRockets::SDK::Game::ShipRef) == sizeof(void*), "ShipRef must carry only the raw pointer");
    static_assert(
        std::is_base_of_v<SimpleRockets::SDK::Game::Ref, SimpleRockets::SDK::Game::OrbitRef>, "OrbitRef must derive from Ref");
    static_assert(
        sizeof(SimpleRockets::SDK::Game::OrbitRef) == sizeof(void*), "OrbitRef must carry only the raw pointer");
    static_assert(std::is_base_of_v<SimpleRockets::SDK::Game::Ref, SimpleRockets::SDK::Game::PartTreeRef>,
        "PartTreeRef must derive from Ref");
    static_assert(
        sizeof(SimpleRockets::SDK::Game::PartTreeRef) == sizeof(void*), "PartTreeRef must carry only the raw pointer");
    static_assert(std::is_base_of_v<SimpleRockets::SDK::Game::Ref, SimpleRockets::SDK::Game::PartConnectionRef>,
        "PartConnectionRef must derive from Ref");
    static_assert(sizeof(SimpleRockets::SDK::Game::PartConnectionRef) == sizeof(void*),
        "PartConnectionRef must carry only the raw pointer");
    static_assert(std::is_base_of_v<SimpleRockets::SDK::Game::PartObjectRef, SimpleRockets::SDK::Game::TankObjectRef>,
        "TankObjectRef must derive from PartObjectRef");
    static_assert(
        sizeof(SimpleRockets::SDK::Game::TankObjectRef) == sizeof(void*), "TankObjectRef must carry only the raw pointer");
    static_assert(
        sizeof(SimpleRockets::SDK::Game::PlanetRef) == sizeof(void*), "PlanetRef must carry only the raw pointer");
    static_assert(std::is_base_of_v<SimpleRockets::SDK::Game::Ref, SimpleRockets::SDK::Game::LayerRef>,
        "LayerRef must derive from Ref");
    static_assert(
        sizeof(SimpleRockets::SDK::Game::LayerRef) == sizeof(void*), "LayerRef must carry only the raw pointer");
    static_assert(std::is_base_of_v<SimpleRockets::SDK::Game::LayerRef, SimpleRockets::SDK::Game::ButtonRef>,
        "ButtonRef must derive from LayerRef");
    static_assert(
        sizeof(SimpleRockets::SDK::Game::ButtonRef) == sizeof(void*), "ButtonRef must carry only the raw pointer");
    static_assert(std::is_base_of_v<SimpleRockets::SDK::Game::ButtonRef, SimpleRockets::SDK::Game::ImageButtonRef>,
        "ImageButtonRef must derive from ButtonRef");
    static_assert(std::is_base_of_v<SimpleRockets::SDK::Game::Ref, SimpleRockets::SDK::Game::ImageButtonRef>,
        "ImageButtonRef must derive from Ref");
    static_assert(
        sizeof(SimpleRockets::SDK::Game::ImageButtonRef) == sizeof(void*), "ImageButtonRef must carry only the raw pointer");
    static_assert(std::is_base_of_v<SimpleRockets::SDK::Game::ImageButtonRef, SimpleRockets::SDK::Game::StateButtonRef>,
        "StateButtonRef must derive from ImageButtonRef");
    static_assert(std::is_base_of_v<SimpleRockets::SDK::Game::Ref, SimpleRockets::SDK::Game::StateButtonRef>,
        "StateButtonRef must derive from Ref");
    static_assert(
        sizeof(SimpleRockets::SDK::Game::StateButtonRef) == sizeof(void*), "StateButtonRef must carry only the raw pointer");
}
