project("Salmon")

if(CMAKE_SYSTEM_NAME STREQUAL Windows)
    link_libraries(-static-libgcc -static-libstdc++)
endif()

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

set(LIB_SOURCES
    src/include_impl/audio_manager_ref.cpp
    src/include_impl/collision_ref.cpp
    src/include_impl/sound_ref.cpp
    src/include_impl/music_ref.cpp
    src/include_impl/gameinfo.cpp
    src/include_impl/data_block_ref.cpp
    src/include_impl/input_cache_ref.cpp
    src/include_impl/camera_ref.cpp
    src/include_impl/actor_ref.cpp
    src/include_impl/map_ref.cpp
    src/include_impl/text_ref.cpp
    src/include_impl/tile_instance.cpp
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
    src/audio/audio_manager.cpp
    src/audio/sound_effect.cpp
    src/audio/music.cpp
    )

set(CORE_SOURCES
    src/core/main.cpp
    src/core/gameinfo.cpp
    src/core/font_manager.cpp
    src/core/input_cache.cpp
    src/core/gamepad.cpp
    )

set(GRAPHICS_SOURCES
    src/graphics/texture.cpp
    src/graphics/texture_cache.cpp
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
    src/util/preloader.cpp
    )

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

find_package(B64 REQUIRED)
find_package(TinyXML2 REQUIRED)
if(NOT CMAKE_SYSTEM_NAME STREQUAL Emscripten)
find_package(ZLIB REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS} ${SDL2_MIXER_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS} ${TinyXML2_INCLUDE_DIRS} ${B64_INCLUDE_DIRS})

if(NOT CMAKE_SYSTEM_NAME STREQUAL Emscripten)
target_link_libraries(${PROJECT_NAME} stdc++fs ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARIES} ${SDL2_TTF_LIBRARIES} ${SDL2_MIXER_LIBRARIES} ${ZLIB_LIBRARIES} ${TinyXML2_LIBRARIES} ${B64_LIBRARIES})
else() # Explicitly linking experimental::fs freaks emscripten out
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARIES} ${SDL2_TTF_LIBRARIES} ${SDL2_MIXER_LIBRARIES} ${ZLIB_LIBRARIES} ${TinyXML2_LIBRARIES} ${B64_LIBRARIES})
endif()

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})
install(TARGETS ${PROJECT_NAME} DESTINATION lib)
