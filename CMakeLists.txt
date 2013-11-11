#cmake ver req
cmake_minimum_required(VERSION 2.8.3)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)   #for executables

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/modules/")

set(ASM_OPTIMISATIONS TRUE CACHE BOOL "use assembler optimisation for noise generation")

if(ASM_OPTIMISATIONS)
    add_definitions(-DNOISE_ASM)

    set(64_BIT_ASM TRUE CACHE BOOL "Generate 64 or 32 bit asm.")
    if(WIN32)
        set(YASM_PATH "${CMAKE_SOURCE_DIR}/yasm/yasm-1.2.0-win64.exe")
    else()
	    set(YASM_PATH "${CMAKE_SOURCE_DIR}/yasm/yasm-linux64")
    endif()
    enable_language(ASM-NASM)

    #if this option is chosen, then it #ifdef NOISE_ASM in th code will be run
endif(ASM_OPTIMISATIONS)

#project name
set(project_name blocks)
project(${project_name})

#Set source files.
set(source_files
	src/application/blocksapp.cpp
    src/application/client.cpp
	src/application/main.cpp
    src/application/server.cpp
    src/console/console.cpp
    src/entity/defaultsetters.cpp
    src/entity/entityfactory.cpp
    src/entity/entitydefinitionloader.cpp
    src/entity/entitysystem.cpp
    src/entity/controllers/collisioncontroller.cpp
    src/entity/controllers/entitycontroller.cpp
    src/entity/controllers/gfxcontroller.cpp
    src/entity/controllers/movementcontroller.cpp
    src/entity/controllers/movedirection.cpp
    src/entity/controllers/physicscontroller.cpp
    src/entity/controllers/playercontroller.cpp
    src/input/inputadaptor.cpp
    src/networking/clientconnection.cpp
    src/networking/clientconnectionlistener.cpp
    src/networking/localclientconnectionlistener.cpp
    src/networking/localserverclientbridge.cpp
    src/networking/package.cpp
	src/networking/remoteserverclientbridge.cpp
    src/networking/serverclientbridge.cpp
	src/rendering/debugrenderer.cpp
	src/rendering/billboard.cpp
	src/rendering/camera.cpp
	src/rendering/meshwalker.cpp
	src/rendering/renderer.cpp
	src/rendering/shader.cpp
	src/rendering/shaderprogram.cpp
    src/rendering/surfacemerger.cpp
	src/rendering/vbo.cpp
	src/rendering/vbocreator.cpp
    src/rendering/newvbo.cpp
    src/script/scriptcallback.cpp
    src/script/scriptengine.cpp
    src/script/scriptentity.cpp
    src/script/scriptentitycore.cpp
    src/script/scripthandler.cpp
    src/script/scriptinterface.cpp
    src/script/scriptmodule.cpp
    src/script/asaddons/scriptarray.cpp
    src/script/asaddons/scriptmath.cpp
    src/script/asaddons/scriptvectors.cpp
    src/script/asaddons/scriptbuilder.cpp
    src/script/asaddons/scriptstdstring.cpp
    src/script/asaddons/scriptstdstring_utils.cpp
    src/utilities/folderexploder.cpp
    src/utilities/lodepng.cpp
    src/utilities/logger.cpp
	src/utilities/random.cpp
    src/utilities/simplexnoise.cpp
	src/utilities/timer.cpp
	src/world/chunk.cpp
    src/world/dimension.cpp
    src/world/landscape.cpp
    src/world/localchunkprovider.cpp
	src/world/world.cpp
    src/world/worldinterface.cpp
	src/utilities/asmnoise.asm
)
set(header_files
	src/blockstd.h
	src/application/applicationmessages.h
	src/application/blocksapp.h
    src/application/client.h
    src/application/server.h
    src/console/consolecolor.h
    src/console/console.h
    src/console/keys.h
	src/entity/entitymessages.h
    src/entity/defaultsetters.h
    src/entity/entityfactory.h
    src/entity/entitydefinition.h
    src/entity/entitydefinitionloader.h
    src/entity/entitysystem.h
    src/entity/controllers/collisioncontroller.h
    src/entity/controllers/entitycontroller.h
    src/entity/controllers/gfxcontroller.h
    src/entity/controllers/moveaction.h
    src/entity/controllers/movedirection.h
    src/entity/controllers/movementcontroller.h
    src/entity/controllers/physicscontroller.h
    src/entity/controllers/physicstype.h
    src/entity/controllers/playercontroller.h
	src/input/inputmessages.h
    src/input/inputactions.h
    src/input/inputadaptor.h
    src/networking/clientconnection.h
    src/networking/clientconnectionlistener.h
    src/networking/localclientconnectionlistener.h
    src/networking/localserverclientbridge.h
    src/networking/package.h
    src/networking/packagetype.h
	src/networking/remoteserverclientbridge.h
    src/networking/serverclientbridge.h
	src/rendering/debugrenderer.h
	src/rendering/renderingmessages.h
	src/rendering/billboard.h
	src/rendering/camera.h
	src/rendering/meshwalker.h
	src/rendering/renderer.h
	src/rendering/shader.h
	src/rendering/shaderprogram.h
    src/rendering/surfacemerger.h
	src/rendering/vbo.h
	src/rendering/vbocreator.h
    src/rendering/newvbo.h
	src/script/scriptmessages.h
    src/script/scriptcallback.h
    src/script/scriptengine.h
    src/script/scriptentity.h
    src/script/scriptentitycore.h
    src/script/scripthandler.h
    src/script/scriptinterface.h
    src/script/scriptmodule.h
    src/script/asaddons/scriptarray.h
    src/script/asaddons/scriptmath.h
    src/script/asaddons/scriptvectors.h
    src/script/asaddons/scriptbuilder.h
    src/script/asaddons/scriptstdstring.h
    src/utilities/folderexploder.h
    src/utilities/lodepng.h
    src/utilities/logger.h
	src/utilities/random.h
    src/utilities/simplexnoise.h
	src/utilities/timer.h
	src/utilities/tinydir.h
	src/world/worldmessages.h
	src/world/chunk.h
    src/world/chunkprovider.h
    src/world/dimension.h
    src/world/landscape.h
    src/world/localchunkprovider.h
	src/world/world.h
    src/world/worldinterface.h
)

find_package(Featherkit REQUIRED COMPONENTS entity structure ui messaging)
find_package(GLM)
find_package(SDL2)
find_package(GLEW)
find_package(OpenGL REQUIRED)
find_package(ENet)
find_package(angelscript)

include_directories(src)

#This has to be done before add_executable. I'm adding source files.
if(NOT GLEW_FOUND)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/glew/include)
	link_directories(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/glew/lib)
	set(source_files ${CMAKE_CURRENT_SOURCE_DIR}/extlibs/glew/glew.c ${source_files})
	add_definitions(-DGLEW_STATIC)
	message("GLEW NOT FOUND SO WE ADDED IT")
endif()
if(WIN32)
    set(source_files ${source_files} windowsresource.rc)
endif(WIN32)
if(NOT SDL2_FOUND)
	link_directories(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/SDL-2.0/lib)
endif(NOT SDL2_FOUND)
add_executable(${project_name} ${header_files} ${source_files})



if(SDL2_FOUND)
	include_directories(${SDL2_INCLUDE_DIR})
	target_link_libraries(${project_name} ${SDL2_LIBRARY})
else(SDL2_FOUND)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/SDL-2.0/include)
	target_link_libraries(${project_name} SDL2main SDL2)
endif()

#instead of setting compiler flags globally. SFML doesn't like building with this flag on.
set_target_properties(${project_name} PROPERTIES COMPILE_FLAGS "-std=c++11 -pthread")

#But that doesn't work in windows.
if(WIN32)
    if(MINGW)
      set(CMAKE_RC_COMPILER_INIT windres)
      ENABLE_LANGUAGE(RC)
      SET(CMAKE_RC_COMPILE_OBJECT
        "<CMAKE_RC_COMPILER> -i <SOURCE> -o <OBJECT>")
    endif(MINGW)
    add_definitions(-std=c++11 -pthread)
	add_definitions(-O4)
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
    target_link_libraries(${project_name} ${ANGELSCRIPT_LIBRARY} -lpthread)
else(ANGELSCRIPT_FOUND)
	subdirs(extlibs/angelscript/angelscript/projects/cmake)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/angelscript/angelscript/include)
	link_directories(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/angelscript/angelscript/lib)
	target_link_libraries(${project_name} Angelscript -lpthread)
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

if(GLEW_FOUND)
	include_directories(${GLEW_INCLUDE_DIR})
	target_link_libraries(${project_name} ${GLEW_LIBRARIES})
else(GLEW_FOUND)
	# target_link_libraries(${project_name} glew32)
endif(GLEW_FOUND)


if(OPENGL_FOUND)
	include_directories(${OPENGL_INCLUDE_DIRS})
	target_link_libraries(${project_name} ${OPENGL_LIBRARIES})
else(OPENGL_FOUND)
	target_link_libraries(${project_name} opengl32)
endif(OPENGL_FOUND)
