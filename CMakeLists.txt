cmake_minimum_required(VERSION 3.19)
project(cubiomes)
set(CMAKE_CXX_STANDARD 17)
add_compile_options(-O1)
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
add_executable(finder finder.cpp)
target_link_libraries(finder bestructure)
