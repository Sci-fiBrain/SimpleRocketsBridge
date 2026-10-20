add_executable(SRLauncher
    main.cpp
    Infrastructure/Log/ConsoleWriter.cpp
    Infrastructure/Log/LocalLogger.cpp
    Infrastructure/Log/LogQueue.cpp
    Infrastructure/Log/PipeReader.cpp
)

target_include_directories(SRLauncher PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(SRLauncher PRIVATE SRSDK)
