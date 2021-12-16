CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
project(mcbe_structue)
set(CMAKE_CXX_STANDARD 17)

set(BIOME_LIB_NAME cubiomes)
set(BE_STRUCTUR_LIB_NAME bestructure)
include_directories(
        PUBLIC
        ${PROJECT_SOURCE_DIR}/cubiomes
        ${PROJECT_SOURCE_DIR}/
)
add_library(
        ${BIOME_LIB_NAME}
        cubiomes/util.c
        cubiomes/noise.c
        cubiomes/rng.h
        cubiomes/layers.c
        cubiomes/generator.c
        cubiomes/finders.c
        cubiomes/biome_tree.c
)


add_library(
        ${BE_STRUCTUR_LIB_NAME}
        be_random.cpp
        tools.cpp
)
target_link_libraries(${BE_STRUCTUR_LIB_NAME} ${BIOME_LIB_NAME})


add_executable(test main.cpp)
target_link_libraries(test ${BE_STRUCTUR_LIB_NAME})
