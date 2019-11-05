project("Salmon")

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${Salmon_SOURCE_DIR}/cmake")

if(NOT MSVC)
    # Add custom profile build type
    set (CMAKE_C_FLAGS_PROFILE "${CMAKE_C_FLAGS_RELEASE} -pg" CACHE STRING
        "Flags used by the C compiler during PROFILE builds."
        FORCE )
    set (CMAKE_CXX_FLAGS_PROFILE "${CMAKE_CXX_FLAGS_RELEASE} -pg" CACHE STRING
        "Flags used by the C++ compiler during PROFILE builds."
        FORCE )
    set (CMAKE_EXE_LINKER_FLAGS_PROFILE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -pg" CACHE STRING
        "Flags used for linking binaries during PROFILE builds."
        FORCE )
    set (CMAKE_SHARED_LINKER_FLAGS_PROFILE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -pg" CACHE STRING
        "Flags used by the shared libraries linker during maintainer builds."
        FORCE )
    MARK_AS_ADVANCED(
        CMAKE_C_FLAGS_PROFILE
        CMAKE_CXX_FLAGS_PROFILE
        CMAKE_EXE_LINKER_FLAGS_PROFILE
        CMAKE_SHARED_LINKER_FLAGS_PROFILE)
    # Update the documentation string of CMAKE_BUILD_TYPE for GUIs
    SET( CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel Profile."
        FORCE )
endif()

if (NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type selected, default to Debug")
  set(CMAKE_BUILD_TYPE DEBUG)
endif()

if (NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -Wfatal-errors")
endif()

include_directories(src)

option(LIB_TARGET "Build library version without event support" OFF)

if(LIB_TARGET)
  add_definitions(-DLIB_BUILD)
endif()

set(LIB_SOURCES
    src/include/gameinfo.cpp
    )

set(ACTOR_SOURCES
    src/actor/actor.cpp
    src/actor/collision.cpp
    src/actor/data_block.cpp
    src/actor/primitive.cpp
    src/actor/primitive_rectangle.cpp
    src/actor/primitive_text.cpp
    )

set(AUDIO_SOURCES
    src/audio/sound_effect.cpp
    src/audio/music.cpp
    )

set(CORE_SOURCES
    src/core/main.cpp
    src/core/gameinfo.cpp
    src/core/font_manager.cpp
    src/core/input_handler.cpp
    )

set(EVENT_SOURCES
    src/event/ae_accelerate.cpp
    src/event/ae_decelerate.cpp
    src/event/ae_erase_this.cpp
    src/event/ae_move.cpp
    src/event/ae_move_direction.cpp
    src/event/ae_move_static.cpp
    src/event/ae_animate.cpp
    src/event/ae_fall.cpp
    src/event/ae_multi.cpp
    src/event/ae_spawn_actor.cpp
    src/event/ate_on_ground.cpp
    src/event/ate_math.cpp
    src/event/ate_mouse_buttons.cpp
    src/event/ate_single.cpp
    src/event/ate_switch.cpp
    src/event/ae_set_var.cpp
    src/event/ae_set_anim_frame.cpp
    src/event/ae_sound.cpp
    src/event/ae_ge_wrapper.cpp
    src/event/ae_me_wrapper.cpp
    src/event/ae_spawn_text.cpp
    src/event/ae_rotate.cpp
    src/event/ate_collision.cpp
    src/event/ae_wait.cpp
    src/event/me_erase_actor.cpp
    src/event/me_ge_wrapper.cpp
    src/event/me_play_music.cpp
    src/event/me_layer_state.cpp
    src/event/me_set_var.cpp
    src/event/me_template_layer.cpp
    src/event/ge_change_map.cpp
    src/event/ge_close_map.cpp
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
    src/util/attribute_parser.cpp
    src/util/game_types.cpp
    src/util/logger.cpp
    src/util/parse.cpp
    )

if(NOT LIB_TARGET)
    set(SALMON_SOURCES
        ${ACTOR_SOURCES}
        ${AUDIO_SOURCES}
        ${CORE_SOURCES}
        ${EVENT_SOURCES}
        ${GRAPHICS_SOURCES}
        ${MAP_SOURCES}
        ${UTIL_SOURCES}
        ${LIB_SOURCES}
    )
    add_executable(${PROJECT_NAME} ${SALMON_SOURCES})

else()
    set(SALMON_SOURCES
        ${ACTOR_SOURCES}
        ${AUDIO_SOURCES}
        ${CORE_SOURCES}
        ${GRAPHICS_SOURCES}
        ${MAP_SOURCES}
        ${UTIL_SOURCES}
        ${LIB_SOURCES}
    )
    add_library(${PROJECT_NAME} SHARED ${SALMON_SOURCES})
endif()

find_package(B64 REQUIRED)
find_package(TinyXML2 REQUIRED)
find_package(ZLIB REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS} ${SDL2_MIXER_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS} ${TinyXML2_INCLUDE_DIRS} ${B64_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARIES} ${SDL2_TTF_LIBRARIES} ${SDL2_MIXER_LIBRARIES} ${ZLIB_LIBRARIES} ${TinyXML2_LIBRARIES} ${B64_LIBRARIES})
