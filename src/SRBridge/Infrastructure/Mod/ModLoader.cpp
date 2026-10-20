#include "Infrastructure/Mod/ModLoader.h"

#include <format>
#include <string>

namespace
{
    /// <summary>MOD 导出的加载入口, 可选</summary>
    constexpr const char* ModLoadExport = "SRModLoad";

    /// <summary>MOD 导出的 Runtime 事件入口</summary>
    constexpr const char* RuntimeHookExport = "SRModRuntimeHook";

    using ModLoadFn = SimpleRockets::Boolean(__cdecl*)(SimpleRockets::SDK::Log::ILogger* logger);

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
    ModLoader::ModLoader(SDK::Log::ILogger& logger)
        : logger(logger)
    {
    }

    ModLoader::~ModLoader()
    {
        for (const HMODULE module : modules)
        {
            FreeLibrary(module);
        }
    }

    void ModLoader::LoadAll(const std::filesystem::path& directory)
    {
        std::error_code error;
        if (!std::filesystem::is_directory(directory, error) || error)
        {
            logger.Log(SDK::Log::LogSeverity::Info, std::format("MOD 目录不存在: {}", ToUtf8(directory)));
            return;
        }

        std::filesystem::directory_iterator iterator(directory, error);
        if (error)
        {
            logger.Log(SDK::Log::LogSeverity::Error, std::format("MOD 目录读取失败: {}", ToUtf8(directory)));
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

        logger.Log(SDK::Log::LogSeverity::Info, std::format("MOD 已注册 {} 个", runtimeHooks.size()));
    }

    void ModLoader::LoadOne(const std::filesystem::path& path)
    {
        const HMODULE module = LoadLibraryW(path.c_str());
        if (module == nullptr)
        {
            logger.Log(SDK::Log::LogSeverity::Error, std::format("MOD 加载失败: {}", ToUtf8(path.filename())));
            return;
        }

        // 导出了加载入口时先注入日志, 未导出视为不需要日志
        const FARPROC loadAddress = GetProcAddress(module, ModLoadExport);
        if (loadAddress != nullptr)
        {
            const ModLoadFn modLoad = reinterpret_cast<ModLoadFn>(loadAddress);
            if (!modLoad(&logger))
            {
                logger.Log(SDK::Log::LogSeverity::Error, std::format("MOD 拒绝加载: {}", ToUtf8(path.filename())));
                FreeLibrary(module);
                return;
            }
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
        logger.Log(SDK::Log::LogSeverity::Info, std::format("MOD 已加载: {}", ToUtf8(path.filename())));
    }

    std::span<SDK::Mod::RuntimeHookBase* const> ModLoader::RuntimeHooks() const
    {
        return runtimeHooks;
    }
}
