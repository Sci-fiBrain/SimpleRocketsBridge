add_library(FullMod SHARED
    FullMod.cpp
)

target_link_libraries(FullMod PRIVATE SRSDK)

if(SR_MODS_DIR)
    add_custom_command(TARGET FullMod POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${SR_MODS_DIR}"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:FullMod>" "${SR_MODS_DIR}/FullMod.dll"
        COMMENT "部署 FullMod.dll 到 ${SR_MODS_DIR}")
endif()
