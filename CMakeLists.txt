cmake_minimum_required(VERSION 3.19)
project(cubiomes C)

set(CMAKE_C_STANDARD 11)

include_directories(cubiomes .)
add_library(libcubiomes
        STATIC
        cubiomes/generator.c
        cubiomes/layers.c
        cubiomes/mcrandom.c
        cubiomes/util.c
        )

add_library(libbestucture structure.c bedrng.c)
target_link_libraries(libbestucture libcubiomes)

add_executable(main main.c)
target_link_libraries(main libbestucture)



