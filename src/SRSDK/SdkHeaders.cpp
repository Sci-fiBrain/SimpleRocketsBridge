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
#include "SimpleRockets/SDK/Game/RuntimeView.h"
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

    static_assert(std::is_abstract_v<SimpleRockets::SDK::Mod::RuntimeHookBase>, "RuntimeHookBase must stay abstract");
    static_assert(
        sizeof(SimpleRockets::SDK::Game::RuntimeView) == sizeof(void*), "RuntimeView must carry only the raw pointer");
}
