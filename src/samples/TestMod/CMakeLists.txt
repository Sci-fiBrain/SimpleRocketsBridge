add_library(TestMod SHARED
    TestMod.cpp
)

target_link_libraries(TestMod PRIVATE SRSDK)
