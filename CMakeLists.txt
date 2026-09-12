cmake_minimum_required(VERSION 3.15)
project(SM64Source35E VERSION 10.0 LANGUAGES C)

include(GenerateG1A)
find_package(Gint 2.9 REQUIRED)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/game/mario.c")
    set(SM64_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
    message(STATUS "SM64 sources trouvees a la racine")
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/original/src/game/mario.c")
    set(SM64_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/original")
    message(STATUS "SM64 sources trouvees dans original/")
else()
    message(FATAL_ERROR "Sources SM64 introuvables")
endif()

add_executable(sm64src
    main.c
    port/compat.c
    port/sandbox_stubs.c
    port/renderer.c
    port/platform_graph35.c
    port/math_util_graph35.c
    generated/animation_data.c

    ${SM64_ROOT}/src/game/mario.c
    ${SM64_ROOT}/src/game/mario_actions_stationary.c
    ${SM64_ROOT}/src/game/mario_actions_moving.c
    ${SM64_ROOT}/src/game/mario_actions_airborne.c
    ${SM64_ROOT}/src/game/mario_actions_object.c
    ${SM64_ROOT}/src/game/mario_actions_submerged.c
    ${SM64_ROOT}/src/game/mario_actions_cutscene.c
    ${SM64_ROOT}/src/game/mario_actions_automatic.c
    ${SM64_ROOT}/src/game/mario_step.c
    port/camera_graph35.c
    ${SM64_ROOT}/src/engine/graph_node.c
)

target_include_directories(sm64src PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/port
    ${CMAKE_CURRENT_SOURCE_DIR}/generated
    ${SM64_ROOT}
    ${SM64_ROOT}/include
    ${SM64_ROOT}/src
    ${SM64_ROOT}/src/engine
    ${SM64_ROOT}/src/game
)

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
    -fomit-frame-pointer
    -Wno-unused-parameter
    -Wno-implicit-fallthrough
    -Wno-discarded-qualifiers
)

target_link_options(sm64src PRIVATE -Wl,--gc-sections -Wl,-Map=SM64SRC.map)
target_link_libraries(sm64src Gint::Gint m)

generate_g1a(
    TARGET sm64src
    OUTPUT "SM64SRC.g1a"
    NAME "SM64Src"
    ICON assets-fx/icon.png
)
