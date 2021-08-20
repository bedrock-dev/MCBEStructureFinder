cmake_minimum_required(VERSION 3.19)
project(cubiomes C)

set(CMAKE_C_STANDARD 11)
add_compile_options(-O3)
include_directories(cubiomes .)
add_library(cubiomes
        STATIC
        cubiomes/generator.c
        cubiomes/layers.c
        cubiomes/mcrandom.c
        cubiomes/util.c
        )

add_library(bestructure structure.c bedrng.c)
target_link_libraries(bestructure cubiomes)

#add_executable(speedrun speedrunseed.c)
#target_link_libraries(speedrun bestructure)
