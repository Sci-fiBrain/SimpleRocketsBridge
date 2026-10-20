add_library(SRBridge SHARED
    Module.cpp
    Infrastructure/Log/PipeLogger.cpp
)

target_include_directories(SRBridge PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(SRBridge PRIVATE SRSDK)

target_compile_definitions(SRBridge PRIVATE SRBRIDGE_EXPORTS)
