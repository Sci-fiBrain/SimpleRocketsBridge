add_library(SRSDK INTERFACE)

target_include_directories(SRSDK INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_compile_features(SRSDK INTERFACE cxx_std_20)
