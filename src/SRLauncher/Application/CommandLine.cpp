#include "Application/CommandLine.h"

#include <filesystem>
#include <system_error>

namespace SimpleRockets::Launcher::Application
{
    namespace
    {
        /// <summary>未指定参数时使用的游戏可执行文件</summary>
        constexpr const wchar_t* DefaultExecutablePath =
            L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\SimpleRockets\\SimpleRockets.exe";
    }

    Boolean TryParseCommandLine(int argumentCount, wchar_t* arguments[], CommandLineOptions& target)
    {
        const wchar_t* requested = DefaultExecutablePath;
        if (argumentCount >= 2 && arguments[1] != nullptr && arguments[1][0] != L'\0')
        {
            requested = arguments[1];
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
