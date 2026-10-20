#pragma once

#include <filesystem>

namespace SimpleRockets::Launcher::Infrastructure
{
    /// <summary>取当前可执行文件所在目录</summary>
    [[nodiscard]] std::filesystem::path ModuleDirectory();
}
