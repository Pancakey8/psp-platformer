cmake_minimum_required(VERSION 3.11)
project(psp-platformer)

add_executable(psp-platformer src/main.c src/player.c src/object.c src/camera.c src/scene.c)

include(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2)

target_include_directories(psp-platformer PRIVATE ${SDL2_INCLUDE_DIRS})
target_compile_options(psp-platformer PRIVATE -ggdb -Wall -Wextra)

target_link_libraries(psp-platformer PRIVATE ${SDL2_LIBRARIES})

if(PSP)
    create_pbp_file(
        TARGET psp-platformer
        ICON_PATH NULL
        BACKGROUND_PATH NULL
        PREVIEW_PATH NULL
        TITLE "PSP Platformer"
        VERSION 01.00)
else()
    target_link_libraries(psp-platformer PRIVATE m)
    target_compile_options(psp-platformer PRIVATE -DLINUX=1)
    target_link_options(psp-platformer PRIVATE -fsanitize=address,undefined)
endif()
