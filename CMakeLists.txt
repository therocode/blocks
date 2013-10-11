#cmake ver req
cmake_minimum_required(VERSION 2.8.3)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)   #for executables

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/modules/")

#project name
set(project_name blocks)
project(${project_name})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#Set source files.
set(source_files
	src/blocksapp.cpp
	src/chunk.cpp
	src/chunkvbo.cpp
	src/renderer.cpp
	src/vbocreator.cpp
	src/world.cpp
	src/main.cpp
	src/camera.cpp
    src/lodepng.cpp
    src/dimension.cpp
    src/landscape.cpp
    src/localchunkdeliverer.cpp
    src/input/inputadaptor.cpp
    src/simplexnoise.cpp
    src/focuspoint.cpp
	src/timer.cpp
    src/entitysystem.cpp
    src/defaultsetters.cpp
    src/controllers/entitycontroller.cpp
    src/controllers/physicscontroller.cpp
    src/controllers/collisioncontroller.cpp
    src/worldinterface.cpp
)
set(header_files
	src/blocksapp.h
	src/blockstd.h
	src/chunk.h
	src/chunkvbo.h
	src/messages.h
	src/renderer.h
	src/vbocreator.h
	src/world.h
	src/camera.h
    src/lodepng.h
    src/dimension.h
    src/landscape.h
    src/chunkdeliverer.h
    src/localchunkdeliverer.h
    src/input/inputadaptor.h
    src/simplexnoise.h
    src/focuspoint.h
    src/input/inputactions.h
	src/timer.h
    src/entitysystem.h
    src/defaultsetters.h
    src/controllers/entitycontroller.h
    src/controllers/physicscontroller.h
    src/controllers/collisioncontroller.h
    src/worldinterface.h
)

add_executable(${project_name} ${header_files} ${source_files})

find_package(Featherkit REQUIRED COMPONENTS entity structure ui messaging)
if(FEATHERKIT_FOUND)
	include_directories(${FEATHERKIT_INCLUDE_DIRS})
	target_link_libraries(${project_name} ${FEATHERKIT_LIBRARIES})
endif(FEATHERKIT_FOUND)

find_package(GLM)
if(GLM_FOUND)
	include_directories(${GLM_INCLUDE_DIRS})
endif(GLM_FOUND)

find_package(SFML COMPONENTS window system graphics REQUIRED)
if(SFML_FOUND)
	include_directories(${SFML_INCLUDE_DIR})
	target_link_libraries(${project_name} ${SFML_LIBRARIES})
endif(SFML_FOUND)

find_package(GLEW REQUIRED)
if(GLEW_FOUND)
	include_directories(${GLEW_INCLUDE_DIR})
	target_link_libraries(${project_name} ${GLEW_LIBRARIES})
endif(GLEW_FOUND)

target_link_libraries(${project_name} GLEW GL GLU)

find_package(OpenGL REQUIRED)
if(OPENGL_FOUND)
	include_directories(${OPENGL_INCLUDE_DIRS})
	target_link_libraries(${project_name} ${OPENGL_LIBRARIES})
endif(OPENGL_FOUND)

#set(USE_CLANG "of course I should use clang.")
if(USE_CLANG)
	SET (CMAKE_C_COMPILER             "/usr/bin/clang")
	SET (CMAKE_C_FLAGS                "-Wall -std=c99")
	SET (CMAKE_C_FLAGS_DEBUG          "-g")
	SET (CMAKE_C_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
	SET (CMAKE_C_FLAGS_RELEASE        "-O4 -DNDEBUG")
	SET (CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")

	SET (CMAKE_CXX_COMPILER             "/usr/bin/clang++")
	SET (CMAKE_CXX_FLAGS                "-Wall -ggdb -std=c++11")
	SET (CMAKE_CXX_FLAGS_DEBUG          "-g")
	SET (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
	SET (CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
	SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

	SET (CMAKE_AR      "/usr/bin/llvm-ar")
	SET (CMAKE_LINKER  "/usr/bin/llvm-ld")
	SET (CMAKE_NM      "/usr/bin/llvm-nm")
	SET (CMAKE_OBJDUMP "/usr/bin/llvm-objdump")
	SET (CMAKE_RANLIB  "/usr/bin/llvm-ranlib")
endif(USE_CLANG)
