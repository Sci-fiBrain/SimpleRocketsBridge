cmake_minimum_required(VERSION 3.28)

project(SRBridge LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 注入到目标进程的模块不能依赖目标机器的 VC 运行库, 因此统一使用静态 CRT
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# 多配置生成器会在输出目录后追加配置名, 实际落到 bin/Debug 与 bin/Release
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")

if(MSVC)
    add_compile_options(/W4 /WX /utf-8 /permissive-)
endif()

set_property(DIRECTORY "${CMAKE_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT SRLauncher)

add_subdirectory(vendor/minhook)
add_subdirectory(src/SRSDK)
add_subdirectory(src/SRBridge)
add_subdirectory(src/SRLauncher)
