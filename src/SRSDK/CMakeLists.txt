add_library(SRSDK INTERFACE)

target_include_directories(SRSDK INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_compile_features(SRSDK INTERFACE cxx_std_20)

# 纯头库本身不产生编译单元, 该目标只负责让头文件与 static_assert 进入构建
add_library(SRSDKChecks OBJECT SdkHeaders.cpp)

target_link_libraries(SRSDKChecks PRIVATE SRSDK)
