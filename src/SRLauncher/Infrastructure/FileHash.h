#pragma once

#include <filesystem>
#include <string>

#include "SimpleRockets/Types.h"

namespace SimpleRockets::Launcher::Infrastructure
{
    /// <summary>计算文件 SHA-256 并以大写十六进制写入 hexDigest; 无法读取或计算时返回 false</summary>
    [[nodiscard]] Boolean ComputeSha256(const std::filesystem::path& path, std::wstring& hexDigest);
}
