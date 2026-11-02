add_library(PartMod SHARED
    PartMod.cpp
    PanelLayer.cpp
    SelectionLayer.cpp
    PanelLayer.h
    SelectionLayer.h
)

target_link_libraries(PartMod PRIVATE SRSDK)

if(SR_MODS_DIR)
    add_custom_command(TARGET PartMod POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${SR_MODS_DIR}"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:PartMod>" "${SR_MODS_DIR}/PartMod.dll"
        COMMENT "部署 PartMod.dll 到 ${SR_MODS_DIR}")
endif()
