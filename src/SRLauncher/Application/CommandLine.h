#pragma once

#include <filesystem>

#include "SimpleRockets/Types.h"

namespace SimpleRockets::Launcher::Application
{
    /// <summary>命令行参数</summary>
    struct CommandLineOptions
    {
        /// <summary>目标游戏 exe 的绝对路径</summary>
        std::filesystem::path executablePath;
    };

    /// <summary>解析命令行, 参数缺失或路径无效时返回 0</summary>
    [[nodiscard]] Boolean TryParseCommandLine(int argumentCount, wchar_t* arguments[], CommandLineOptions& target);
}
