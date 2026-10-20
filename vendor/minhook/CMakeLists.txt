add_library(MinHook STATIC
    src/buffer.c
    src/hook.c
    src/trampoline.c
    src/hde/hde32.c
    src/hde/hde64.c
)

target_include_directories(MinHook PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(MinHook PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

# 第三方代码不继承项目的 /W4 /WX
target_compile_options(MinHook PRIVATE /W0 /WX-)
