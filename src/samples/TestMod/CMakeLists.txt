add_library(TestMod SHARED
    TestMod.cpp
)

target_link_libraries(TestMod PRIVATE SRSDK)

if(SR_MODS_DIR)
    add_custom_command(TARGET TestMod POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${SR_MODS_DIR}"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:TestMod>" "${SR_MODS_DIR}/TestMod.dll"
        COMMENT "部署 TestMod.dll 到 ${SR_MODS_DIR}")
endif()
