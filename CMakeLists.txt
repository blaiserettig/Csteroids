cmake_minimum_required(VERSION 3.10)

project(Asteroids)

set(CMAKE_C_STANDARD 99)

# Add your source files here
add_executable(main src/main.c
        include/util/v2.h
        include/util/math_ext.h
        src/asteroid.h
        include/util/array_list.h
        include/util/array_list.c
        src/asteroid.c
        src/particle.c
        src/particle.h
)

# Include the SDL3 header directory
include_directories(${CMAKE_SOURCE_DIR}/include)

# Link SDL3
target_link_libraries(main ${CMAKE_SOURCE_DIR}/lib/libSDL3.dll.a)

