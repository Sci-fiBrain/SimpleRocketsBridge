cmake_minimum_required(VERSION 3.28)

project(SRBridge LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 目标游戏自身使用 UCRT (MSVCP140 / VCRUNTIME140 / api-ms-win-crt-*), 因此统一走 /MD
# 与游戏共享同一份运行库, 跨模块的 new/delete 与标准库类型才能安全传递
# Debug 配置依赖 MSVCP140D / VCRUNTIME140D / ucrtbased, 只有装了 Visual Studio 的机器才有, 仅限本机调试
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

# 多配置生成器会在输出目录后追加配置名, 实际落到 bin/Debug 与 bin/Release
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")

if(MSVC)
    add_compile_options(/W4 /WX /utf-8 /permissive-)
endif()

set_property(DIRECTORY "${CMAKE_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT SRLauncher)

# 样例 mod 的部署目录, 构建后会拷贝样例模组
set(SR_MODS_DIR "C:/Program Files (x86)/Steam/steamapps/common/SimpleRockets/mods" CACHE PATH "样例 mod 部署目录")

add_subdirectory(vendor/minhook)
add_subdirectory(src/SRSDK)
add_subdirectory(src/SRBridge)
add_subdirectory(src/SRLauncher)
add_subdirectory(src/samples/FullMod)
add_subdirectory(src/samples/AutoMod)
add_subdirectory(src/samples/PartMod)
