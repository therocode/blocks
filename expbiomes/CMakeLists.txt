cmake_minimum_required(VERSION 2.8.3)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)   #for executables
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/modules/")

#project name
set(project_name expbiomes)
project(${project_name})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")

file(GLOB_RECURSE source_files "src/*.cpp")
file(GLOB_RECURSE header_files "src/*.h")

add_executable(${project_name} ${header_files} ${source_files})

find_package(Featherkit COMPONENTS rendering ui messaging structure entity REQUIRED)

if(FEATHERKIT_FOUND)
    include_directories(${FEATHERKIT_INCLUDE_DIRS})
    target_link_libraries(${project_name} ${FEATHERKIT_LIBRARIES})
endif()

include_directories(../../libnoise/noise/include/)

target_link_libraries(${project_name} GL GLU GLEW sfml-window sfml-system sfml-graphics /home/tobbe/projects/libnoise/noise/lib/libnoise.so.0.3)
