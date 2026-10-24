#pragma once

#include <Windows.h>

#include <filesystem>
#include <span>
#include <vector>

#include "SimpleRockets/SDK/Mod/RuntimeHookBase.h"
#include "SimpleRockets/Types.h"

namespace SimpleRockets::Bridge::Infrastructure::Mod
{
    /// <summary>扫描目录下的 MOD 并收集 Runtime 事件接收者</summary>
    class ModLoader
    {
    public:
        ModLoader() = default;
        ~ModLoader();

        ModLoader(const ModLoader&) = delete;
        ModLoader& operator=(const ModLoader&) = delete;

        /// <summary>加载目录下全部 dll, 单个失败只记录并跳过</summary>
        /// <param name="directory">MOD 目录, 不存在时直接返回</param>
        void LoadAll(const std::filesystem::path& directory);

        /// <summary>已注册的 Runtime 事件接收者, 元素存活期由所属 MOD 模块保证</summary>
        [[nodiscard]] std::span<SDK::Mod::RuntimeHookBase* const> RuntimeHooks() const;

    private:
        void LoadOne(const std::filesystem::path& path);

        std::vector<SDK::Mod::RuntimeHookBase*> runtimeHooks;
        std::vector<HMODULE> modules;
    };
}
