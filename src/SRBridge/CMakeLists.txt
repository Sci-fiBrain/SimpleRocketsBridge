add_library(SRBridge SHARED
    Module.cpp
    SRBridge.def
    Bootstrap/Exports.cpp
    Infrastructure/Hook/Detours.cpp
    Infrastructure/Hook/HookBridge.cpp
    Infrastructure/Log/PipeLogger.cpp
)

target_include_directories(SRBridge PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(SRBridge PRIVATE SRSDK MinHook)

target_compile_definitions(SRBridge PRIVATE SRBRIDGE_EXPORTS)
