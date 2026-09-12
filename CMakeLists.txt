cmake_minimum_required(VERSION 3.15)
project(SM64Source35E VERSION 6.0 LANGUAGES C)

include(GenerateG1A)
find_package(Gint 2.9 REQUIRED)

add_executable(sm64src
    main.c
    port/compat.c
    port/sandbox_stubs.c
    port/renderer.c
    generated/animation_data.c

    original/src/game/mario.c
    original/src/game/mario_actions_stationary.c
    original/src/game/mario_actions_moving.c
    original/src/game/mario_actions_airborne.c
    original/src/game/mario_actions_object.c
    original/src/game/mario_actions_submerged.c
    original/src/game/mario_actions_cutscene.c
    original/src/game/mario_actions_automatic.c
    original/src/game/mario_step.c
    original/src/game/camera.c
    original/src/engine/math_util.c
    original/src/engine/graph_node.c
)

target_include_directories(sm64src PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/generated
    ${CMAKE_CURRENT_SOURCE_DIR}/original
    ${CMAKE_CURRENT_SOURCE_DIR}/original/include
    ${CMAKE_CURRENT_SOURCE_DIR}/original/src
)

# M_PI is normally supplied by the original build environment. Defining it at
# build level lets math_util.c stay byte-for-byte identical to the source ZIP.
target_compile_definitions(sm64src PRIVATE
    _LANGUAGE_C
    NON_MATCHING
    AVOID_UB
    VERSION_EU
    M_PI=3.14159265358979323846
)

target_compile_options(sm64src PRIVATE
    -Os
    -ffunction-sections
    -fdata-sections
    -Wno-unused-parameter
    -Wno-implicit-fallthrough
)

target_link_options(sm64src PRIVATE -Wl,--gc-sections)
target_link_libraries(sm64src Gint::Gint m)

generate_g1a(
    TARGET sm64src
    OUTPUT "SM64SRC.g1a"
    NAME "SM64Src"
    ICON assets-fx/icon.png
)
