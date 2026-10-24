#include "Infrastructure/Mod/ModLoader.h"

#include <format>
#include <string>

#include "SimpleRockets/SDK/Environment.h"

namespace
{
    /// <summary>MOD 导出的加载入口, 接收宿主环境</summary>
    constexpr const char* ModLoadExport = "SRModLoad";

    /// <summary>MOD 导出的 Runtime 事件入口</summary>
    constexpr const char* RuntimeHookExport = "SRModRuntimeHook";

    using ModLoadFn = SimpleRockets::Boolean(__cdecl*)(SimpleRockets::SDK::Environment* environment);

    using GetRuntimeHookFn = SimpleRockets::SDK::Mod::RuntimeHookBase*(__cdecl*)();

    /// <summary>路径转 UTF-8, 供日志输出</summary>
    std::string ToUtf8(const std::filesystem::path& path)
    {
        const std::u8string text = path.u8string();
        return std::string(reinterpret_cast<const char*>(text.data()), text.size());
    }
}

namespace SimpleRockets::Bridge::Infrastructure::Mod
{
    ModLoader::~ModLoader()
    {
        for (const HMODULE module : modules)
        {
            FreeLibrary(module);
        }
    }

    void ModLoader::LoadAll(const std::filesystem::path& directory)
    {
        SDK::Log::ILogger& logger = SDK::Environment::GetLogger();

        std::error_code error;
        if (!std::filesystem::is_directory(directory, error) || error)
        {
            if (logger.IsEnabled(SDK::Log::LogSeverity::Info))
            {
                logger.Log(SDK::Log::LogSeverity::Info, std::format("MOD 目录不存在: {}", ToUtf8(directory)));
            }
            return;
        }

        std::filesystem::directory_iterator iterator(directory, error);
        if (error)
        {
            if (logger.IsEnabled(SDK::Log::LogSeverity::Error))
            {
                logger.Log(SDK::Log::LogSeverity::Error, std::format("MOD 目录读取失败: {}", ToUtf8(directory)));
            }
            return;
        }

        for (const std::filesystem::directory_entry& entry : iterator)
        {
            if (entry.path().extension() != L".dll")
            {
                continue;
            }
            LoadOne(entry.path());
        }

        if (logger.IsEnabled(SDK::Log::LogSeverity::Info))
        {
            logger.Log(SDK::Log::LogSeverity::Info, std::format("MOD 已注册 {} 个", runtimeHooks.size()));
        }
    }

    void ModLoader::LoadOne(const std::filesystem::path& path)
    {
        SDK::Log::ILogger& logger = SDK::Environment::GetLogger();

        const HMODULE module = LoadLibraryW(path.c_str());
        if (module == nullptr)
        {
            if (logger.IsEnabled(SDK::Log::LogSeverity::Error))
            {
                logger.Log(SDK::Log::LogSeverity::Error,
                    std::format("MOD 加载失败: {}", ToUtf8(path.filename())));
            }
            return;
        }

        const FARPROC loadAddress = GetProcAddress(module, ModLoadExport);
        if (loadAddress == nullptr)
        {
            if (logger.IsEnabled(SDK::Log::LogSeverity::Error))
            {
                logger.Log(SDK::Log::LogSeverity::Error,
                    std::format("MOD 缺少加载入口: {}", ToUtf8(path.filename())));
            }
            FreeLibrary(module);
            return;
        }

        const ModLoadFn modLoad = reinterpret_cast<ModLoadFn>(loadAddress);
        if (!modLoad(SDK::Environment::GetCurrent()))
        {
            if (logger.IsEnabled(SDK::Log::LogSeverity::Error))
            {
                logger.Log(SDK::Log::LogSeverity::Error,
                    std::format("MOD 拒绝加载: {}", ToUtf8(path.filename())));
            }
            FreeLibrary(module);
            return;
        }

        const FARPROC address = GetProcAddress(module, RuntimeHookExport);
        if (address == nullptr)
        {
            // 未导出入口视为不关心 Runtime 事件
            FreeLibrary(module);
            return;
        }

        SDK::Mod::RuntimeHookBase* hook = reinterpret_cast<GetRuntimeHookFn>(address)();
        if (hook == nullptr)
        {
            FreeLibrary(module);
            return;
        }

        runtimeHooks.push_back(hook);
        modules.push_back(module);
        if (logger.IsEnabled(SDK::Log::LogSeverity::Info))
        {
            logger.Log(SDK::Log::LogSeverity::Info, std::format("MOD 已加载: {}", ToUtf8(path.filename())));
        }
    }

    std::span<SDK::Mod::RuntimeHookBase* const> ModLoader::RuntimeHooks() const
    {
        return runtimeHooks;
    }
}
