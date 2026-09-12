cmake_minimum_required(VERSION 3.15)
project(SM64Source35E VERSION 7.0 LANGUAGES C)

include(GenerateG1A)
find_package(Gint 2.9 REQUIRED)

# Accepte les deux organisations :
#   1) sources SM64 directement à la racine (src/, include/, actors/...)
#   2) sources SM64 dans original/
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/game/mario.c")
    set(SM64_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
    message(STATUS "SM64 sources trouvees a la racine")
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/original/src/game/mario.c")
    set(SM64_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/original")
    message(STATUS "SM64 sources trouvees dans original/")
else()
    message(FATAL_ERROR
        "Sources SM64 introuvables. Il faut src/game/mario.c "
        "a la racine ou original/src/game/mario.c.")
endif()

add_executable(sm64src
    main.c
    port/compat.c
    port/sandbox_stubs.c
    port/renderer.c
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
    ${SM64_ROOT}/src/game/camera.c
    ${SM64_ROOT}/src/engine/math_util.c
    ${SM64_ROOT}/src/engine/graph_node.c
)

target_include_directories(sm64src PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/generated
    ${SM64_ROOT}
    ${SM64_ROOT}/include
    ${SM64_ROOT}/src
)

# L'environnement original fournit normalement M_PI.
# On le definit ici pour laisser math_util.c intact.
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
