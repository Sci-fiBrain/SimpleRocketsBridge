add_library(SRSDK INTERFACE)

target_include_directories(SRSDK INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_compile_features(SRSDK INTERFACE cxx_std_20)

# windows.h 默认把 min/max 定义成宏, 会破坏 std::min/std::max; 使用方统一关掉
target_compile_definitions(SRSDK INTERFACE NOMINMAX)
