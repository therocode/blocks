#cmake ver req
cmake_minimum_required(VERSION 2.8.3)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)   #for executables

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/modules/")

set(BUILD_EXPBIOMES FALSE CACHE BOOL "build the biome experiment application")
set(BUILD_TESTS TRUE CACHE BOOL "build the tests")
set(BUILD_GAME TRUE CACHE BOOL "build the actual game")

#project name
set(project_name blocks)
project(${project_name})

if(BUILD_EXPBIOMES)
    add_subdirectory(expbiomes)
endif()

#Set source files.
set(source_files
    src/gameinterface.cpp
	src/application/blocksapp.cpp
    src/application/client.cpp
	src/application/main.cpp
    src/application/server.cpp

    src/console/console.cpp

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

    src/networking/serverclientbridge.cpp
    
    src/rendering/chunkgeneratorqueue.cpp
    src/rendering/debugrenderer.cpp
    src/rendering/billboard.cpp
    src/rendering/camera.cpp
    src/rendering/frustum.cpp
    src/rendering/meshwalker.cpp
    src/rendering/renderer.cpp
    src/rendering/shader.cpp
    src/rendering/shaderprogram.cpp
    src/rendering/surfacemerger.cpp
    src/rendering/texture.cpp
    src/rendering/vbo.cpp
    src/rendering/vbocreator.cpp
    src/rendering/marchingcubegenerator.cpp
    src/rendering/gl_core_3_2.c

    src/script/scriptcallback.cpp
    src/script/scriptengine.cpp
    src/script/scriptentity.cpp
    src/script/scriptentitycore.cpp
    src/script/scriptsystem.cpp
    src/script/scriptmodule.cpp
    src/script/asaddons/scriptarray.cpp
    src/script/asaddons/scriptmath.cpp
    src/script/asaddons/scriptvectors.cpp
    src/script/asaddons/scriptbuilder.cpp
    src/script/asaddons/scriptstdstring.cpp
    src/script/asaddons/scriptstdstring_utils.cpp
    src/script/callers/scriptcaller.cpp
    src/script/callers/frametimecaller.cpp
    src/script/callers/gameeventcaller.cpp
    src/script/callers/ongroundcaller.cpp
    src/script/interfaces/entityinterface.cpp
    src/script/interfaces/landscapeinterface.cpp
    src/script/interfaces/mathsinterface.cpp
    src/script/interfaces/physicsinterface.cpp
    src/script/interfaces/printinterface.cpp
    src/script/interfaces/randominterface.cpp
    src/script/interfaces/scriptinterface.cpp
    src/script/interfaces/stringinterface.cpp

    src/utilities/coordinateconverter.cpp
    src/utilities/directorycreator.cpp
    src/utilities/folderexploder.cpp
    src/utilities/fpscontroller.cpp
    src/utilities/geomath.cpp
    src/utilities/lodepng.cpp
    src/utilities/logger.cpp
    src/utilities/random.cpp
    src/utilities/timer.cpp

    src/world/chunk.cpp
    src/world/worldprovider.cpp
    src/world/highlightmanager.cpp
    src/world/modmanager.cpp
    src/world/ranges.cpp
    src/world/raycaster.cpp
    src/world/regiondatafragment.cpp
    src/world/worldsystem.cpp
    src/world/worldloader.cpp
    src/world/generation/biome.cpp
    src/world/generation/biomestorage.cpp
    src/world/generation/chunkgenerator.cpp
    src/world/generation/regiondatagenerator.cpp
    )

set(header_files
    src/gameinterface.hpp
    src/lognames.hpp
    src/application/applicationmessages.hpp
    src/application/blocksapp.hpp
    src/application/client.hpp
    src/application/server.hpp

    src/console/consolecolor.hpp
    src/console/console.hpp
    src/console/keys.hpp

    src/entity/entitymessages.hpp
    src/entity/entityfactory.hpp
    src/entity/entitydefinition.hpp
    src/entity/entitydefinitionloader.hpp
    src/entity/entitysystem.hpp
    src/entity/controllers/collisioncontroller.hpp
    src/entity/controllers/entitycontroller.hpp
    src/entity/controllers/gfxcontroller.hpp
    src/entity/controllers/moveaction.hpp
    src/entity/controllers/movedirection.hpp
    src/entity/controllers/movementcontroller.hpp
    src/entity/controllers/physicscontroller.hpp
    src/entity/controllers/physicstype.hpp
    src/entity/controllers/playercontroller.hpp

    src/input/inputmessages.hpp
    src/input/inputactions.hpp
    src/input/inputadaptor.hpp

    src/networking/clientconnection.hpp
    src/networking/clientconnectionlistener.hpp
    src/networking/localclientconnectionlistener.hpp
    src/networking/localserverclientbridge.hpp
    src/networking/package.hpp
    src/networking/packagetype.hpp

    src/networking/clientconnection.cpp
    src/networking/clientconnectionlistener.cpp
    src/networking/localclientconnectionlistener.cpp
    src/networking/localserverclientbridge.cpp
    src/networking/package.cpp

    src/rendering/chunkgeneratorqueue.hpp
    src/rendering/debugrenderer.hpp
    src/rendering/renderingmessages.hpp
    src/rendering/billboard.hpp
    src/rendering/camera.hpp
    src/rendering/frustum.hpp
    src/rendering/mesh.hpp
    src/rendering/meshwalker.hpp
    src/rendering/renderer.hpp
    src/rendering/shader.hpp
    src/rendering/shaderprogram.hpp
    src/rendering/surfacemerger.hpp
    src/rendering/texture.hpp
    src/rendering/vbo.hpp
    src/rendering/vbocreator.hpp
    src/rendering/marchingcubegenerator.hpp
    src/rendering/gl_core_3_2.h
    src/rendering/opengl.hpp

    src/script/scriptmessages.hpp
    src/script/scriptcallback.hpp
    src/script/scriptengine.hpp
    src/script/scriptentity.hpp
    src/script/scriptentitycore.hpp
    src/script/scriptsystem.hpp
    src/script/scriptmodule.hpp
    src/script/scriptstd.hpp
    src/script/asaddons/scriptarray.hpp
    src/script/asaddons/scriptmath.hpp
    src/script/asaddons/scriptvectors.hpp
    src/script/asaddons/scriptbuilder.hpp
    src/script/asaddons/scriptstdstring.hpp
    src/script/callers/scriptcaller.hpp
    src/script/callers/frametimecaller.hpp
    src/script/callers/gameeventcaller.hpp
    src/script/callers/ongroundcaller.hpp
    src/script/interfaces/entityinterface.hpp
    src/script/interfaces/landscapeinterface.hpp
    src/script/interfaces/mathsinterface.hpp
    src/script/interfaces/physicsinterface.hpp
    src/script/interfaces/printinterface.hpp
    src/script/interfaces/randominterface.hpp
    src/script/interfaces/scriptinterface.hpp
    src/script/interfaces/stringinterface.hpp

    src/utilities/coordinateconverter.hpp
    src/utilities/directorycreator.hpp
    src/utilities/folderexploder.hpp
    src/utilities/fpscontroller.hpp
    src/utilities/geomath.hpp
    src/utilities/glm.hpp
    src/utilities/interpolationgrid3d.hpp
    src/utilities/lodepng.hpp
    src/utilities/logger.hpp
	src/utilities/random.hpp
	src/utilities/timer.hpp
	src/utilities/tinydir.hpp

	src/world/chunk.hpp
    src/world/worlddata.hpp
    src/world/valuemap.hpp
    src/world/highlightmanager.hpp
    src/world/modmanager.hpp
    src/world/raycaster.hpp
    src/world/regiondatafragment.hpp
    src/world/worldconstants.hpp
	src/world/worldsystem.hpp
    src/world/worldloader.hpp
	src/world/worldmessages.hpp
    src/world/worldprovider.hpp
	src/world/generation/biome.hpp
	src/world/generation/biomestorage.hpp
    src/world/generation/chunkgenerator.hpp
    src/world/generation/regiondatagenerator.hpp
)

find_package(Featherkit REQUIRED COMPONENTS entity structure ui sdl2 util)
find_package(GLM)
find_package(SDL2)
find_package(OpenGL REQUIRED)
#find_package(ENet)
find_package(angelscript)

find_package(JsonCpp REQUIRED)


include_directories(src)

#This has to be done before add_executable. I'm adding source files.

if(WIN32)
    message("Added windows resource.")
    set(source_files ${source_files} ${CMAKE_CURRENT_SOURCE_DIR}/windowsresource.rc)
    if(MINGW)
        message("Enabled rc compiler")
        set(CMAKE_RC_COMPILER_INIT windres)
        ENABLE_LANGUAGE(RC)
        SET(CMAKE_RC_COMPILE_OBJECT
            "<CMAKE_RC_COMPILER> -i <SOURCE> -o <OBJECT>")
    endif(MINGW)
endif(WIN32)

if(NOT SDL2_FOUND)
	link_directories(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/SDL-2.0/lib)
endif(NOT SDL2_FOUND)

if(NOT MSVC)
    set(CPP_11_FLAG -std=c++11)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CPP_11_FLAG} -pthread")

if(BUILD_GAME)
    add_executable(${project_name} ${header_files} ${source_files})
    
    if(SDL2_FOUND)
    	include_directories(${SDL2_INCLUDE_DIR})
    	target_link_libraries(${project_name} ${SDL2_LIBRARY})
    else(SDL2_FOUND)
    	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/SDL-2.0/include)
    	target_link_libraries(${project_name} SDL2main SDL2)
    endif()
    
    #if(ENET_FOUND)
    #	include_directories(${ENet_INCLUDE_DIRS})
    #	target_link_libraries(${project_name} ${ENet_LIBRARIES})
    #else(ENET_FOUND)
    #	subdirs(extlibs/enet)
    #	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/enet/include)
    #	link_directories(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/enet)
    #	SET(ELIBS enet)
    #	IF(WIN32)
    #        SET(WINDOWS_ENET_DEPENDENCIES "ws2_32;winmm")
    #        SET(ELIBS ${ELIBS} ${WINDOWS_ENET_DEPENDENCIES})
    #    ELSE(WIN32)
    #        SET(ELIBS ${ELIBS})
    #    ENDIF(WIN32)
    #	target_link_libraries(${project_name} ${ELIBS})
    #endif(ENET_FOUND)
    if(ZLIB_FOUND)
        include_directories(${ZLib_INCLUDE_DIRS})
        target_link_libraries(${project_name} ${ZLib_LIBRARIES})
    #else(ZLIB_FOUND)
    #    subdirs(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/zlib-1.2.8)
    #    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/zlib-1.2.8)
    #    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/zlib-1.2.8)
    #    target_link_libraries(${project_name} zlibstatic)
    endif(ZLIB_FOUND)
    
    if(ANGELSCRIPT_FOUND)
        include_directories(${ANGELSCRIPT_INCLUDE_DIRECTORY})
        target_link_libraries(${project_name} ${ANGELSCRIPT_LIBRARY} -lpthread)
    #else(ANGELSCRIPT_FOUND)
    #	subdirs(extlibs/angelscript/angelscript/projects/cmake)
    #	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/angelscript/angelscript/include)
    #	link_directories(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/angelscript/angelscript/lib)
    #	target_link_libraries(${project_name} Angelscript -lpthread)
    endif(ANGELSCRIPT_FOUND)

    find_package(JsonCpp REQUIRED)
    if(JSONCPP_FOUND)
        include_directories(${JSONCPP_INCLUDE_DIRS})
    endif()
    
    if(FEATHERKIT_FOUND)
    	include_directories(${FEATHERKIT_INCLUDE_DIRS})
    	target_link_libraries(${project_name} ${FEATHERKIT_LIBRARIES})
    endif(FEATHERKIT_FOUND)
    
    if(GLM_FOUND)
    	include_directories(${GLM_INCLUDE_DIRS})
    else(GLM_FOUND)
    	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/glm)
    endif(GLM_FOUND)
    
    if(JSONCPP_FOUND)
        include_directories(${JSONCPP_INCLUDE_DIRS})
        target_link_libraries(${project_name} ${JSONCPP_LIBRARIES})
    endif()
    
    
    if(OPENGL_FOUND)
    	include_directories(${OPENGL_INCLUDE_DIRS})
    	target_link_libraries(${project_name} ${OPENGL_LIBRARIES})
    else(OPENGL_FOUND)
    	target_link_libraries(${project_name} opengl32)
    endif(OPENGL_FOUND)
endif(BUILD_GAME)

#tests
if(BUILD_TESTS)
    add_executable(test_modmanager "test/world/modmanagertest.cpp" "src/world/modmanager.cpp" "src/world/chunk.cpp" "src/utilities/directorycreator.cpp" "src/utilities/coordinateconverter.cpp")
    target_link_libraries(test_modmanager ${FEATHERKIT_LIBRARIES})
    
    add_executable(test_highlightmanager "test/world/highlightmanagertest.cpp" "src/world/highlightmanager.cpp")
    target_link_libraries(test_highlightmanager ${FEATHERKIT_LIBRARIES})
    
    add_executable(test_coordinate "test/world/coordinatetest.cpp" "src/utilities/coordinateconverter.cpp")
    target_link_libraries(test_coordinate ${FEATHERKIT_LIBRARIES})
    
    #add_executable(test_world_misc "test/world/misctest.cpp" "src/world/worldconstants.cpp" "src/world/region.cpp" "src/world/regiondatafragment.cpp")
    #target_link_libraries(test_world_misc ${FEATHERKIT_LIBRARIES})
    
    add_executable(test_utilities "test/utilities/utilitiestest.cpp" "src/utilities/directorycreator.cpp")
    target_link_libraries(test_utilities ${FEATHERKIT_LIBRARIES})
    
    add_executable(test_chunk "test/world/chunktest.cpp" "src/world/chunk.cpp")
    target_link_libraries(test_chunk ${FEATHERKIT_LIBRARIES})
    
    add_executable(test_interpolation_grid3d "test/utilities/interpolationgrid3dtest.cpp")
    target_link_libraries(test_interpolation_grid3d ${FEATHERKIT_LIBRARIES})
	
    #profiling
    add_executable(profile_modmanager "profile/world/modmanagerprofile.cpp" "src/world/modmanager.cpp" "src/world/chunk.cpp" "src/utilities/directorycreator.cpp" "src/utilities/coordinateconverter.cpp")
    target_link_libraries(profile_modmanager ${FEATHERKIT_LIBRARIES})
endif(BUILD_TESTS)
