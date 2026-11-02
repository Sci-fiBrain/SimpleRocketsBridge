#include "Application/CommandLine.h"

#include <filesystem>
#include <system_error>

#include "Infrastructure/PathUtil.h"

namespace SimpleRockets::Launcher::Application
{
    namespace
    {
        /// <summary>未指定参数时使用的游戏可执行文件名, 与启动器同目录</summary>
        constexpr const wchar_t* DefaultExecutableName = L"SimpleRockets.exe";
    }

    Boolean TryParseCommandLine(int argumentCount, wchar_t* arguments[], CommandLineOptions& target)
    {
        std::filesystem::path requested;
        if (argumentCount >= 2 && arguments[1] != nullptr && arguments[1][0] != L'\0')
        {
            requested = arguments[1];
        }
        else
        {
            const std::filesystem::path directory = Infrastructure::ModuleDirectory();
            if (directory.empty())
            {
                return false;
            }

            requested = directory / DefaultExecutableName;
        }

        std::error_code error;
        const std::filesystem::path absolutePath = std::filesystem::absolute(requested, error);
        if (error)
        {
            return false;
        }

        if (std::filesystem::is_regular_file(absolutePath, error) == false || error)
        {
            return false;
        }

        target.executablePath = absolutePath;
        return true;
    }
}
