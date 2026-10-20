add_executable(SRLauncher
    main.cpp
    Application/CommandLine.cpp
    Application/Run.cpp
    Infrastructure/FileHash.cpp
    Infrastructure/Log/ConsoleWriter.cpp
    Infrastructure/Log/LocalLogger.cpp
    Infrastructure/Log/LogQueue.cpp
    Infrastructure/Log/PipeReader.cpp
    Infrastructure/PathUtil.cpp
    Infrastructure/PipePair.cpp
    Infrastructure/ProcessHost.cpp
    Infrastructure/ProcessInjector.cpp
)

target_include_directories(SRLauncher PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(SRLauncher PRIVATE SRSDK bcrypt)
