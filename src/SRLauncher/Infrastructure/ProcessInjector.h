#pragma once

#include <filesystem>

#include <Windows.h>

#include "SimpleRockets/Types.h"

namespace SimpleRockets::Launcher::Infrastructure
{
    /// <summary>
    /// 在目标进程内加载 DLL 并调用其导出函数
    /// </summary>
    class ProcessInjector final
    {
    public:
        ProcessInjector(HANDLE process, DWORD processId);

        /// <summary>注入 DLL, 返回目标进程内的模块基址, 失败为 0</summary>
        [[nodiscard]] UInt32 InjectLibrary(const std::filesystem::path& dllPath) const;

        /// <summary>在目标模块列表中查找模块基址, 未找到为 0</summary>
        [[nodiscard]] UInt32 FindModule(const wchar_t* moduleName) const;

        /// <summary>远程调用只接收一个 32 位参数的导出函数, 返回其是否返回非 0</summary>
        [[nodiscard]] Boolean CallExport(UInt32 moduleBase, UInt32 exportOffset, UInt32 argument) const;

    private:
        HANDLE process;
        DWORD processId;
    };
}
