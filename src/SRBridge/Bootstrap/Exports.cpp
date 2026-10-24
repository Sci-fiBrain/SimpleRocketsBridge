#include <new>

#include <Windows.h>

#include <filesystem>
#include <memory>

#include "Infrastructure/Hook/Detours.h"
#include "Infrastructure/Hook/HookBridge.h"
#include "Infrastructure/Log/PipeLogger.h"
#include "Infrastructure/Mod/ModLoader.h"
#include "SimpleRockets/SDK/Environment.h"

namespace
{
    std::unique_ptr<SimpleRockets::Bridge::Infrastructure::Log::PipeLogger> pipeLogger;
    std::unique_ptr<SimpleRockets::SDK::Environment> environment;
    std::unique_ptr<SimpleRockets::Bridge::Infrastructure::Mod::ModLoader> modLoader;

    /// <summary>宿主进程主模块所在目录, 取不到时为空</summary>
    std::filesystem::path GameDirectory()
    {
        wchar_t buffer[MAX_PATH] = {};
        const DWORD length = GetModuleFileNameW(nullptr, buffer, MAX_PATH);
        if (length == 0 || length == MAX_PATH)
        {
            return std::filesystem::path();
        }

        return std::filesystem::path(buffer).parent_path();
    }
}

/// <summary>
/// 由 Launcher 远程调用, 传入日志管道的可写端句柄
///
/// 线程入口约定为 WINAPI, 返回值即远程调用的结果
/// </summary>
extern "C" DWORD WINAPI SRBridgeStart(void* pipeHandle)
{
    using namespace SimpleRockets::Bridge::Infrastructure::Hook;
    using namespace SimpleRockets::Bridge::Infrastructure::Log;
    using namespace SimpleRockets::Bridge::Infrastructure::Mod;
    using namespace SimpleRockets::SDK::Log;

    if (pipeHandle == nullptr || pipeHandle == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    pipeLogger.reset(new (std::nothrow) PipeLogger(static_cast<HANDLE>(pipeHandle), LogSeverity::Trace));
    if (pipeLogger == nullptr)
    {
        return FALSE;
    }

    ILogger& logger = *pipeLogger;
    logger.Log(LogSeverity::Info, "SRBridge 已连接");

    environment.reset(new (std::nothrow) SimpleRockets::SDK::Environment(*pipeLogger, GetModuleHandleW(nullptr)));
    if (environment == nullptr)
    {
        logger.Log(LogSeverity::Error, "运行期环境创建失败");
        return FALSE;
    }
    SimpleRockets::SDK::Environment::Attach(environment.get());

    modLoader.reset(new (std::nothrow) ModLoader());
    if (modLoader == nullptr)
    {
        logger.Log(LogSeverity::Error, "MOD 加载器创建失败");
        return FALSE;
    }

    const std::filesystem::path gameDirectory = GameDirectory();
    if (!gameDirectory.empty())
    {
        modLoader->LoadAll(gameDirectory / L"mods");
    }
    Detours::SetRuntimeHooks(modLoader->RuntimeHooks());

    if (!Install())
    {
        logger.Log(LogSeverity::Error, "hook 安装失败");
        return FALSE;
    }

    logger.Log(LogSeverity::Info, "hook 已安装");
    return TRUE;
}
