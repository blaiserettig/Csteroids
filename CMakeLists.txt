cmake_minimum_required(VERSION 3.10)

project(Asteroids)

set(CMAKE_C_STANDARD 99)

# Add your source files here
add_executable(main src/main.c
        src/util/v2.h
        src/util/math_ext.h
        src/asteroid.h
        src/util/array_list.h
        src/util/array_list.c
        src/asteroid.c
        src/text.c
        src/text.h
        src/button.c
        src/button.h
        src/audio.c
        src/audio.h
)

# Include the SDL3 header directory
include_directories(${CMAKE_SOURCE_DIR}/include)

# Link SDL3
target_link_libraries(main ${CMAKE_SOURCE_DIR}/lib/libSDL3.dll.a)

# Copy SDL3.dll to the build directory after building
add_custom_command(TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/bin/SDL3.dll"
        $<TARGET_FILE_DIR:main>)

add_custom_command(
        TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/src/assets" "${CMAKE_CURRENT_BINARY_DIR}/assets"
)