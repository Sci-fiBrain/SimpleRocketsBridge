# SimpleRocketsBridge

SimpleRockets 1（PC / Steam，32 位）的 mod 加载框架。

启动器校验目标可执行文件后注入 `SRBridge.dll`，桥接层在游戏主线程安装 hook，并将 Runtime 事件分发给 `mods/` 目录下的各 mod。

## 快速使用

从 [Releases](../../releases/latest) 下载最新压缩包，把内容覆盖到游戏根目录。

```
SimpleRockets/
├── SimpleRockets.exe
├── SRLauncher.exe        启动器
├── SRBridge.dll          桥接层
└── mods/
    ├── FullMod.dll
    ├── AutoMod.dll
    └── PartMod.dll
```

双击 `SRLauncher.exe` 启动游戏，控制台同步显示桥接层日志，游戏退出后跟随退出。

| 示例 mod | 效果 |
|---|---|
| `FullMod` | HUD 按钮，点击后为所有油箱补满燃料 |
| `AutoMod` | 高度保持，按钮切换控制器开关 |
| `PartMod` | 全屏选中层与信息面板，可启停零件、触发分离器 |

不需要哪个示例，直接从 `mods/` 删掉对应 DLL。自己的 mod 同样丢进 `mods/` 即可。

## 从源码构建

要求：CMake ≥ 3.28，Visual Studio 2026（工具集 v145），Win32 目标，C++20。

```powershell
cmake --preset win32
cmake --build --preset win32-release
```

产物位于 `bin/Release`：`SRLauncher.exe`、`SRBridge.dll` 及各示例 mod。开发时显式传入游戏路径：

```powershell
bin\Release\SRLauncher.exe "<游戏目录>/SimpleRockets.exe"
```

省略参数时启动同目录下的 `SimpleRockets.exe`。

仅支持 Release 配置。Debug 配置的 `std::string` 布局与游戏不一致，注入后会在创建控件时破坏相邻字段。

示例 mod 的部署目录由缓存变量 `SR_MODS_DIR` 控制，留空则不部署：

```powershell
cmake --preset win32 -DSR_MODS_DIR="<游戏目录>/mods"
```

工程结构：

| 目标 | 类型 | 职责 |
|---|---|---|
| `SRLauncher` | exe | 校验目标 exe、启动游戏、注入 `SRBridge.dll`、将日志经管道回显到控制台 |
| `SRBridge` | dll | 加载 `mods/*.dll`、安装 hook、将 Runtime 事件分发给已注册的 mod |
| `SRSDK` | 纯头库 | mod 侧视图与接口：`Environment`、`RuntimeHookBase`、各类 Ref |
| `FullMod` / `AutoMod` / `PartMod` | dll | 示例 mod |

## 编写 mod

mod 是普通 Win32 DLL，必须导出 `SRModLoad`：

```cpp
extern "C" __declspec(dllexport) SimpleRockets::Boolean SRModLoad(SimpleRockets::SDK::Environment* environment)
{
    SimpleRockets::SDK::Environment::Attach(environment);
    return true;
}
```

`SRModLoad` 在加载时调用一次。必须调用 `Environment::Attach`，SDK 的 `GetLogger` 与 `GetAddress` 依赖该上下文，未附加时解引用空指针。返回 `false` 表示拒绝加载，桥接层随即卸载该 DLL。

`SRModRuntimeHook` 可选，返回 mod 持有的 hook 实例；未导出时视为该 mod 不订阅 Runtime 事件。hook 对象由 mod 持有，桥接层只保存裸指针，不参与其生命周期。

`RuntimeHookBase` 的所有事件均有空实现，按需覆写。全部回调在游戏主线程同步执行，必须快速返回。

| 事件 | 时机 | sender |
|---|---|---|
| `OnInitialized` | `Runtime::Initialize` 之后 | `RuntimeRef` |
| `OnLoaded` | UI 层构造完成，晚于 `OnInitialized` | `RuntimeLayerRef` |
| `OnUpdating` | 每帧更新之前，此刻写入的字段会被本帧读取 | `RuntimeRef` |
| `OnUpdated` | 每帧更新之后 | `RuntimeRef` |
| `OnSaved` | 世界序列化之后 | `RuntimeRef` |
| `OnNextStageActivated` | 分级推进之后 | `PodObjectRef` |
| `OnDisposing` | `Runtime` 析构之前 | `RuntimeRef` |

## 示例

三个示例分别覆盖不同的接入深度：

| 示例 | 演示内容 |
|---|---|
| `FullMod` | 最小闭环。`OnLoaded` 中创建 HUD 按钮并注册点击回调，点击后遍历零件树为所有油箱补满燃料。 |
| `AutoMod` | 周期控制。`OnUpdating` 中前置写入油门与锁定航向，由高度环与垂速环驱动；按钮切换控制器开关。 |
| `PartMod` | 自定义层。派生 `LayerBase` 实现全屏选中层与信息面板，处理触摸事件选中零件，并按帧刷新面板内容与零件启停状态。 |

## 版本绑定

SDK 内的字段偏移、虚表槽位与函数地址只对特定游戏版本成立。启动器在注入前计算目标 exe 的 SHA-256，与编译期常量比对，不一致时直接退出。更换游戏版本需要重新适配全部偏移。

## 目录

```
src/SRSDK/       纯头 SDK
src/SRBridge/    Bootstrap 导出、Hook / Log / Mod 基础设施
src/SRLauncher/  命令行、进程启动与注入、日志管道
src/samples/     示例 mod
vendor/minhook   hook 库
```

## 许可

本项目采用 MIT 许可证，见 `LICENSE`。`vendor/minhook` 为 BSD 2-Clause，见其自带许可文件。

本项目与 Jundroo 无关，未获官方授权，不包含任何游戏文件。
