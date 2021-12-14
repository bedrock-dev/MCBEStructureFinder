cmake_minimum_required(VERSION 3.19)
project(cubiomes)
set(CMAKE_CXX_STANDARD 17)
#add_compile_options(-O3)
include_directories(cubiomes .)

set(CUBIOMES_NAME cubiomes-be)
set(FINDER_NAME be-structure-finder)
#预编译cubiomes-bedrock
add_library(${CUBIOMES_NAME}
        STATIC
        cubiomes/generator.c
        cubiomes/layers.c
        cubiomes/mcrandom.c
        cubiomes/util.c
        )

add_library(${FINDER_NAME}
        structure.cpp
        be_random.cpp
        utils.cpp
        finder.cpp
        new_finder.cpp
        )
target_link_libraries(${FINDER_NAME} ${CUBIOMES_NAME})
#add_executable(speedrun speedrunseed.c)
add_executable(main main.cpp)
target_link_libraries(main ${FINDER_NAME})

function(create_test name files)
    add_executable(${name}
            ${files}
            ${PROJECT_SOURCE_DIR}/test/test_base.h
            )
    target_include_directories(
            ${name}
            PUBLIC
            ${PROJECT_SOURCE_DIR}/cubiomes
            ${PROJECT_SOURCE_DIR}
            ${PROJECT_SOURCE_DIR}/test
    )
    target_link_libraries(${name} ${FINDER_NAME})
endfunction(create_test)
create_test(utils_test test/utils_test.cpp)


