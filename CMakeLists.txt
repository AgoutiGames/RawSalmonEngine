project("Salmon")

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_BUILD_TYPE DEBUG)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -Wfatal-errors -pg")

include_directories(src)

set(ACTOR_SOURCES
    src/actor/actor.cpp
    src/actor/cause.cpp
    src/actor/data_block.cpp
    )

set(AUDIO_SOURCES
    src/audio/sound_effect.cpp
    )

set(CORE_SOURCES
    src/core/main.cpp
    src/core/gameinfo.cpp
    )

set(EVENT_SOURCES
    src/event/ae_accelerate.cpp
    src/event/ae_decelerate.cpp
    src/event/ae_move.cpp
    src/event/ae_move_direction.cpp
    src/event/ae_move_sustained.cpp
    src/event/ae_animate.cpp
    src/event/ae_jump.cpp
    src/event/ae_fall.cpp
    src/event/ae_multi.cpp
    src/event/ate_on_ground.cpp
    src/event/ae_set_var.cpp
    src/event/ae_sound.cpp
    src/event/ae_ge_wrapper.cpp
    src/event/ae_me_wrapper.cpp
    src/event/me_set_var.cpp
    src/event/ge_set_var.cpp
    src/event/ge_quit.cpp
    )

set(GRAPHICS_SOURCES
    src/graphics/texture.cpp
    )

set(MAP_SOURCES
    src/map/camera.cpp
    src/map/mapdata.cpp
    src/map/layer.cpp
    src/map/layer_collection.cpp
    src/map/map_layer.cpp
    src/map/image_layer.cpp
    src/map/object_layer.cpp
    src/map/tileset.cpp
    src/map/tileset_collection.cpp
    src/map/tile.cpp
    )

set(UTIL_SOURCES
    src/util/base64.cpp
    src/util/game_types.cpp
    src/util/parse.cpp
    src/util/tinyxml2.cpp
    )

set(SALMON_SOURCES
    ${ACTOR_SOURCES}
    ${AUDIO_SOURCES}
    ${CORE_SOURCES}
    ${EVENT_SOURCES}
    ${GRAPHICS_SOURCES}
    ${MAP_SOURCES}
    ${UTIL_SOURCES}
)

add_executable(${PROJECT_NAME} ${SALMON_SOURCES})

INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf)
PKG_SEARCH_MODULE(SDL2MIXER REQUIRED SDL2_mixer)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIRS} ${SDL2MIXER_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES} ${SDL2MIXER_LIBRARIES})
