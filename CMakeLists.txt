cmake_minimum_required(VERSION 3.16)
project(pokemon_rpg LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# raylib - если нет в системе, скачивается
find_package(raylib 5.0 QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.5
        GIT_SHALLOW TRUE
    )
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(raylib)
endif()

add_executable(pokemon_rpg
    src/main.cpp
    src/ability.cpp
    src/pokemon.cpp
    src/player.cpp
    src/world_map.cpp
    src/battle_system.cpp
    src/graphics.cpp
    src/game.cpp
)

target_include_directories(pokemon_rpg PRIVATE src)
target_link_libraries(pokemon_rpg PRIVATE raylib)

if (MSVC)
    target_compile_options(pokemon_rpg PRIVATE /W4 /utf-8)
else()
    target_compile_options(pokemon_rpg PRIVATE -Wall -Wextra)
endif()

# копируем картинки рядом с exe
add_custom_command(TARGET pokemon_rpg POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:pokemon_rpg>/assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data $<TARGET_FILE_DIR:pokemon_rpg>/data
)
