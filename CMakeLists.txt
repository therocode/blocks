#cmake ver req
cmake_minimum_required(VERSION 2.8.3)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)   #for executables

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/modules/")

#project name
set(project_name blocks)
project(${project_name})

#Set source files.
set(source_files
	src/blocksapp.cpp
    src/client.cpp
    src/server.cpp
    src/serverclientbridge.cpp
    src/localserverclientbridge.cpp
    src/package.cpp
    src/chunkloadedpackage.cpp
    src/gfxentityaddedpackage.cpp
    src/gfxentitymovedpackage.cpp
	src/chunk.cpp
	src/vbo.cpp
	src/billboard.cpp
	src/renderer.cpp
	src/vbocreator.cpp
	src/world.cpp
	src/main.cpp
	src/camera.cpp
    src/lodepng.cpp
    src/dimension.cpp
    src/landscape.cpp
    src/localchunkprovider.cpp
    src/input/inputadaptor.cpp
    src/simplexnoise.cpp
    src/focuspoint.cpp
	src/timer.cpp
    src/entitysystem.cpp
    src/defaultsetters.cpp
    src/controllers/entitycontroller.cpp
    src/controllers/physicscontroller.cpp
    src/controllers/collisioncontroller.cpp
    src/controllers/gfxcontroller.cpp
    src/controllers/playercontroller.cpp
    src/worldinterface.cpp
	src/shader.cpp
	src/shaderprogram.cpp
	src/remoteserverclientbridge.cpp
)
set(header_files
	src/blocksapp.h
    src/client.h
    src/server.h
    src/serverclientbridge.h
    src/localserverclientbridge.h
    src/package.h
    src/chunkloadedpackage.h
    src/gfxentityaddedpackage.h
    src/gfxentitymovedpackage.h
	src/blockstd.h
	src/chunk.h
	src/vbo.h
	src/billboard.h
	src/messages.h
	src/renderer.h
	src/vbocreator.h
	src/world.h
	src/camera.h
    src/lodepng.h
    src/dimension.h
    src/landscape.h
    src/chunkprovider.h
    src/localchunkprovider.h
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
    src/controllers/gfxcontroller.cpp
    src/controllers/playercontroller.h
    src/worldinterface.h
	src/shader.h
	src/shaderprogram.h
	src/remoteserverclientbridge.h
)

find_package(Featherkit REQUIRED COMPONENTS entity structure ui messaging)
find_package(GLM)
find_package(SFML COMPONENTS window system graphics)
find_package(GLEW)
find_package(OpenGL REQUIRED)
find_package(ENet)
find_package(angelscript REQUIRED)

#This has to be done before add_executable. I'm adding source files.
if(NOT GLEW_FOUND)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/glew/include)
	set(source_files ${CMAKE_CURRENT_SOURCE_DIR}/extlibs/glew/glew.c ${source_files})
endif()

add_executable(${project_name} ${header_files} ${source_files})
#instead of setting compiler flags globally. SFML doesn't like building with this flag on.
set_target_properties(${project_name} PROPERTIES COMPILE_FLAGS "-std=c++11 -pthread")
if(WIN32)
add_definitions(-std=c++11 -pthread)
endif(WIN32)
if(ENET_FOUND)
	include_directories(${ENet_INCLUDE_DIRS})
	target_link_libraries(${project_name} ${ENet_LIBRARIES})
else(ENET_FOUND)
	subdirs(extlibs/enet)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/enet/include)
	link_directories(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/enet)
	SET(ELIBS enet)
	IF(WIN32)
        SET(WINDOWS_ENET_DEPENDENCIES "ws2_32;winmm")
        SET(ELIBS ${ELIBS} ${WINDOWS_ENET_DEPENDENCIES})
    ELSE(WIN32)
        SET(ELIBS ${ELIBS})
    ENDIF(WIN32)
	target_link_libraries(${project_name} ${ELIBS})
endif(ENET_FOUND)

if(ANGELSCRIPT_FOUND)
    include_directories(${ANGELSCRIPT_INCLUDE_DIRECTORY})
    target_link_libraries(${project_name} ${ANGELSCRIPT_LIBRARY})
endif(ANGELSCRIPT_FOUND)

if(FEATHERKIT_FOUND)
	include_directories(${FEATHERKIT_INCLUDE_DIRS})
	target_link_libraries(${project_name} ${FEATHERKIT_LIBRARIES})
endif(FEATHERKIT_FOUND)

if(GLM_FOUND)
	include_directories(${GLM_INCLUDE_DIRS})
else(GLM_FOUND)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/glm)
endif(GLM_FOUND)

if(SFML_FOUND)
	include_directories(${SFML_INCLUDE_DIR})
	target_link_libraries(${project_name} ${SFML_LIBRARIES})
	message(${SFML_LIBRARIES})
else(SFML_FOUND)
	subdirs(extlibs/SFML)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/SFML/include)
	link_directories(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/SFML/lib)
	target_link_libraries(${project_name} sfml-window sfml-system sfml-graphics) 
	add_definitions(-DGLEW_STATIC)
endif(SFML_FOUND)

if(GLEW_FOUND)
	include_directories(${GLEW_INCLUDE_DIR})
	target_link_libraries(${project_name} ${GLEW_LIBRARIES})
endif(GLEW_FOUND)

#target_link_libraries(${project_name} GLEW GL GLU)

if(OPENGL_FOUND)
	include_directories(${OPENGL_INCLUDE_DIRS})
	target_link_libraries(${project_name} ${OPENGL_LIBRARIES})
endif(OPENGL_FOUND)
