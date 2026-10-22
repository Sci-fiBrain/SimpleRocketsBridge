# SRBridge

SimpleRockets 1 (PC / Steam, 32 位) 的 mod 加载框架。启动器校验游戏版本后注入 `SRBridge.dll`，桥接层在游戏主线程上挂钩若干函数，并通过 SDK 把事件派发给 `mods/` 目录下的各个 mod。

## 组成

| 目标 | 类型 | 作用 |
|---|---|---|
| `SRLauncher` | exe | 校验目标 exe 哈希、启动游戏、注入 `SRBridge.dll`、把日志经管道回显到控制台 |
| `SRBridge` | dll | 加载 `mods/*.dll`、安装 hook、把 Runtime 事件派发给已注册的 mod |
| `SRSDK` | 纯头库 | mod 侧视图与接口：`ILogger`、`RuntimeHookBase`、各类 Ref |
| `TestMod` | dll | 样例 mod，构建后自动拷到游戏的 `mods/` 目录 |

## 构建

要求 CMake ≥ 3.28、Visual Studio 2026 (v145)、Win32 目标、C++20。

```powershell
cmake --preset win32
cmake --build --preset win32-release
```

产物落在 `bin/Release`：`SRLauncher.exe`、`SRBridge.dll`、`TestMod.dll`。

样例 mod 的部署目录由缓存变量控制，换机器时覆盖：

```powershell
cmake --preset win32 -DSR_MODS_DIR="D:/Games/SimpleRockets/mods"
```

## 运行

```powershell
bin\Release\SRLauncher.exe "C:\Program Files (x86)\Steam\steamapps\common\SimpleRockets\SimpleRockets.exe"
```

`SRLauncher.exe` 与 `SRBridge.dll` 必须放在同一目录，注入时按启动器自身目录定位 DLL。省略参数时使用内置的 Steam 默认路径。

## 写一个 mod

mod 就是一个普通 DLL，导出两个 C 函数：

```cpp
extern "C" __declspec(dllexport) Boolean SRModLoad(ILogger* logger);          // 可选
extern "C" __declspec(dllexport) RuntimeHookBase* SRModRuntimeHook();         // 必须
```

`RuntimeHookBase` 的默认实现全是空函数，按需覆写：

| 事件 | 时机 |
|---|---|
| `OnInitialized` | `Runtime::Initialize` 之后，持有 `RuntimeRef` |
| `OnLoaded` | HUD 层构造完成，sender 是已挂齐游戏按钮的 `RuntimeLayerRef` |
| `OnUpdating` / `OnUpdated` | 每帧更新前后 |
| `OnSaved` | 世界序列化之后 |
| `OnNextStageActivated` | 推进分级之后，sender 是 `PodObjectRef` |
| `OnDisposing` | `Runtime` 析构之前 |

最小例子见 `src/samples/TestMod/TestMod.cpp`：在 `OnLoaded` 里用 `StateButtonRef::CreateStacked` 往 HUD 加一个按钮，点击后补满油箱。

```cpp
const StateButtonRef button = StateButtonRef::CreateStacked(sender, "FULL", true);
button.SetClickHandler<&OnFullClick>(&state);
```

## 版本锁定

所有偏移与 RVA 都只对下面这一个二进制成立，启动器在注入前会做哈希校验，不匹配直接退出：

```
SHA-256  459F1D170EE21507739F8A99CCD03127C90EC9D4E2E3DC281B584D83A835BC42
基址     0x400000
```

换游戏版本意味着整张偏移表要重扒。

## 目录

```
src/SRSDK/          纯头 SDK, 按需要 include 或直接用 SRSDK.h
src/SRBridge/       Bootstrap 导出、Hook/Log/Mod 基础设施
src/SRLauncher/     命令行、进程启动与注入、日志管道
src/samples/        样例 mod
docs/               逆向笔记: Runtime 结构、Mod 数据面、符号源
tools/              Python 侧分析脚本 (pefile + capstone)
vendor/minhook      hook 库
```

## 文档

| 文件 | 内容 |
|---|---|
| `docs/RUNTIME-STRUCTURE.md` | Runtime 对象外形、生命周期、字段全表 |
| `docs/MOD-DATA-SURFACE.md` | mod 能读什么、能写什么、UI 怎么接、hook 点在哪 |
| `docs/APK-SYMBOLS.md` | 从 Android 符号反推 PC 侧类名与虚表 |

运行时约定：`/MD` 与游戏共用 UCRT，跨模块的 `new`/`delete` 与标准库类型可以安全传递；Debug 配置依赖调试版运行库，只用于本机调试。
