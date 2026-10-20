#include <Windows.h>

/// <summary>
/// 模块入口, 进程注入完成后由系统调用
/// </summary>
/// <param name="module">本模块的基址, 卸载前有效</param>
/// <param name="reason">调用原因, 取值见 DLL_PROCESS_ATTACH 等常量</param>
BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(module);
        break;
    case DLL_PROCESS_DETACH:
        break;
    default:
        break;
    }
    return TRUE;
}
