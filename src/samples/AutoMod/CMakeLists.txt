add_library(AutoMod SHARED
    AutoMod.cpp
)

target_link_libraries(AutoMod PRIVATE SRSDK)

if(SR_MODS_DIR)
    add_custom_command(TARGET AutoMod POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${SR_MODS_DIR}"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:AutoMod>" "${SR_MODS_DIR}/AutoMod.dll"
        COMMENT "部署 AutoMod.dll 到 ${SR_MODS_DIR}")
endif()
