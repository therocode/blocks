#cmake ver req
cmake_minimum_required(VERSION 2.8.3)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)   #for executables

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/modules/")

set(BUILD_EXPBIOMES FALSE CACHE BOOL "build the biome experiment application")
set(BUILD_TESTS TRUE CACHE BOOL "build the tests")
set(BUILD_PROFILING TRUE CACHE BOOL "build the profiling tests")
set(BUILD_GAME TRUE CACHE BOOL "build the actual game")
set(USE_CTAGS FALSE CACHE BOOL "use ctags to generate source symbol information")

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

    src/client/clientworld.cpp

    src/console/console.cpp

    src/entity/entityfactory.cpp
    src/entity/entitydefinitionloader.cpp
    src/entity/entitysystem.cpp
    src/entity/controllers/collisioncontroller.cpp
    src/entity/controllers/entitycontroller.cpp
    src/entity/controllers/highlightcontroller.cpp
    src/entity/controllers/movedirection.cpp
    src/entity/controllers/movementcontroller.cpp
    src/entity/controllers/physicscontroller.cpp
    src/entity/controllers/playercontroller.cpp
    src/entity/controllers/worldcontroller.cpp

    src/input/inputadaptor.cpp

    src/networking/chunkrequesthandler.cpp
    src/networking/clientnetworkingsystem.cpp
    src/networking/enet.cpp
    src/networking/enetclient.cpp
    src/networking/enetserver.cpp
    src/networking/messageserializer.cpp
    src/networking/servernetworkingsystem.cpp

    src/utilities/coordinateconverter.cpp
    src/utilities/directorycreator.cpp
    src/utilities/distancechecker.cpp
    src/utilities/folderexploder.cpp
    src/utilities/fpscontroller.cpp
    src/utilities/geomath.cpp
    src/utilities/gimgui.cpp
    src/utilities/lodepng.cpp
    src/utilities/logger.cpp
    src/utilities/random.cpp
    src/utilities/signalcatcher.cpp
    src/utilities/timer.cpp

    src/rendering/animationshader.cpp
    src/rendering/baseshader.cpp
    src/rendering/buffer.cpp
    src/rendering/camera.cpp
    src/rendering/chunkmodelcreator.cpp
    src/rendering/debugrenderable.cpp
    src/rendering/debugrenderer.cpp
    src/rendering/extrarenderable.cpp
    src/rendering/extrarenderer.cpp
    src/rendering/frustum.cpp
    src/rendering/gl_core_3_3.c
    src/rendering/glcontext.cpp
    src/rendering/guirenderer.cpp
    src/rendering/guirenderable.cpp
    src/rendering/mesh.cpp
    src/rendering/meshwalker.cpp
    src/rendering/model.cpp
    src/rendering/modelrenderer.cpp
    src/rendering/modelrenderable.cpp
    src/rendering/renderable.cpp
    src/rendering/renderer.cpp
    src/rendering/renderingsystem.cpp
    src/rendering/rendermodule.cpp
    src/rendering/rendermode.cpp
    src/rendering/shader.cpp
    src/rendering/shaderassembler.cpp
    src/rendering/surfacemerger.cpp
    src/rendering/texturearray.cpp
    src/rendering/uniform.cpp
    src/rendering/vao.cpp
    src/rendering/vertexattribute.cpp
    src/rendering/voxelchunkrenderer.cpp
    src/rendering/voxelchunkrenderable.cpp
    src/rendering/shapes.hpp

    src/resources/extensionmetadata.cpp
    src/resources/extensionmetadatafromfileloader.cpp
	src/resources/gfxentitydefinitionfromfileloader.cpp
    src/resources/worldfromfileloader.cpp
    src/resources/attributefromfileloader.cpp
    src/resources/entityfromfileloader.cpp
    src/resources/fontfromfileloader.cpp
    src/resources/image.cpp
    src/resources/iqmfromfileloader.cpp
    src/resources/resourceexception.cpp
    src/resources/resourcepathtoname.cpp
    src/resources/resourcesystem.cpp
    src/resources/shaderdefinitionfromfileloader.cpp
    src/resources/shadersourcefromfileloader.cpp
    src/resources/texture.cpp
    src/resources/imagefromfileloader.cpp

    src/script/scriptcallback.cpp
    src/script/scriptchunk.cpp
    src/script/scriptengine.cpp
    src/script/scriptentity.cpp
    src/script/scriptentitycore.cpp
    src/script/scriptsystem.cpp
    src/script/scriptmodule.cpp
    src/script/asaddons/scriptarray.cpp
    src/script/asaddons/scriptmath.cpp
    src/script/asaddons/scriptvectors.cpp
    src/script/asaddons/scriptvec3.cpp
    src/script/asaddons/scriptu8vec3.cpp
    src/script/asaddons/scripti64vec3.cpp
    src/script/asaddons/scriptbuilder.cpp
    src/script/asaddons/scriptstdstring.cpp
    src/script/asaddons/scriptprimitivealiases.cpp
    src/script/asaddons/scriptstdstring_utils.cpp
    src/script/callers/scriptcaller.cpp
    src/script/callers/frametimecaller.cpp
    src/script/callers/gameeventcaller.cpp
    src/script/callers/chunkeventcaller.cpp
    src/script/callers/ongroundcaller.cpp
    src/script/callers/worldcaller.cpp
    src/script/interfaces/animationinterface.cpp
    src/script/interfaces/chunkinterface.cpp
    src/script/interfaces/entityinterface.cpp
    src/script/interfaces/extensioninterface.cpp
    src/script/interfaces/worldinterface.cpp
    src/script/interfaces/mathsinterface.cpp
    src/script/interfaces/physicsinterface.cpp
    src/script/interfaces/printinterface.cpp
    src/script/interfaces/randominterface.cpp
    src/script/interfaces/scriptinterface.cpp
    src/script/interfaces/stringinterface.cpp

    src/ui/debugui.cpp

    src/world/biome.cpp
    src/world/biomeprovider.cpp
    src/world/chunk.cpp
    src/world/chunkprovider.cpp
	src/world/explorationmanager.cpp
    src/world/fieldgenerator.cpp
    src/world/highlightmanager.cpp
    src/world/modmanager.cpp
    src/world/ranges.cpp
    src/world/raycaster.cpp
    src/world/worldentry.cpp
    src/world/worldsystem.cpp
    src/world/generation/chunkgenerator.cpp
    )

set(header_files
    src/gameinterface.hpp
    src/lognames.hpp
    src/application/applicationmessages.hpp
    src/application/blocksapp.hpp
    src/application/client.hpp
    src/application/server.hpp

    src/client/clientworld.hpp
    src/client/clientmessages.hpp

    src/console/consolecolor.hpp
    src/console/console.hpp
    src/console/keys.hpp

    src/entity/entitymessages.hpp
    src/entity/entityfactory.hpp
    src/entity/entitydefinition.hpp
    src/entity/entitydefinitionloader.hpp
    src/entity/entitysystem.hpp
    src/entity/playermessages.hpp
    src/entity/controllers/collisioncontroller.hpp
    src/entity/controllers/entitycontroller.hpp
    src/entity/controllers/highlightcontroller.hpp
    src/entity/controllers/moveaction.hpp
    src/entity/controllers/movedirection.hpp
    src/entity/controllers/movementcontroller.hpp
    src/entity/controllers/physicscontroller.hpp
    src/entity/controllers/physicstype.hpp
    src/entity/controllers/playercontroller.hpp
    src/entity/controllers/worldcontroller.hpp

    src/input/inputmessages.hpp
    src/input/inputactions.hpp
    src/input/inputadaptor.hpp

    src/networking/channels.hpp
    src/networking/chunkrequesthandler.hpp
    src/networking/clientnetworkingsystem.hpp
    src/networking/enet.hpp
    src/networking/enetclient.hpp
    src/networking/enetserver.hpp
    src/networking/messageserializer.hpp
    src/networking/networkingmessages.hpp
    src/networking/networkingprotocol.hpp
    src/networking/servernetworkingsystem.hpp

    src/rendering/animationshader.hpp
    src/rendering/baseshader.hpp
    src/rendering/buffer.hpp
    src/rendering/camera.hpp
    src/rendering/chunkmodelcreator.hpp
    src/rendering/debugrenderable.hpp
    src/rendering/debugrenderer.hpp
    src/rendering/extrarenderable.hpp
    src/rendering/extrarenderer.hpp
    src/rendering/frustum.hpp
    src/rendering/gl_core_3_3.h
    src/rendering/glcontext.hpp
    src/rendering/guirenderer.hpp
    src/rendering/guirenderable.hpp
    src/rendering/mesh.hpp
    src/rendering/meshwalker.hpp
    src/rendering/model.hpp
    src/rendering/modelattribute.hpp
    src/rendering/modelrenderer.hpp
    src/rendering/modelrenderable.hpp
    src/rendering/opengl.hpp
    src/rendering/renderable.hpp
    src/rendering/renderer.hpp
    src/rendering/renderingmessages.hpp
    src/rendering/renderingsystem.hpp
    src/rendering/rendermodule.hpp
    src/rendering/rendermode.hpp
    src/rendering/shader.hpp
    src/rendering/shaderassembler.hpp
    src/rendering/shaderattribute.hpp
    src/rendering/surfacemerger.hpp
    src/rendering/texturearray.hpp
    src/rendering/uniform.hpp
    src/rendering/vao.hpp
    src/rendering/vertexattribute.hpp
    src/rendering/voxelchunkrenderer.hpp
    src/rendering/voxelchunkrenderable.hpp

    src/resources/extensionmetadata.hpp
    src/resources/extensionmetadatafromfileloader.hpp
    src/resources/fontfromfileloader.hpp
	src/resources/gfxentitydefinition.hpp
	src/resources/gfxentitydefinitionfromfileloader.hpp
    src/resources/image.hpp
    src/resources/imagefromfileloader.hpp
    src/resources/iqm.h
    src/resources/iqmfromfileloader.hpp
	src/resources/rawmodel.hpp
	src/resources/resourcecache.hpp
    src/resources/resourceexception.hpp
	src/resources/resourcemessages.hpp
	src/resources/resourcepathtoname.hpp
    src/resources/resourcesystem.hpp
    src/resources/shaderdefinition.hpp
    src/resources/shaderdefinitionfromfileloader.hpp
    src/resources/shadersource.hpp
    src/resources/shadersourcefromfileloader.hpp
    src/resources/texture.hpp
    src/resources/texturedefinition.hpp
    src/resources/worldfromfileloader.hpp
    src/resources/attributefromfileloader.hpp
    src/resources/entityfromfileloader.hpp

    src/utilities/coordinateconverter.hpp
    src/utilities/directorycreator.hpp
    src/utilities/folderexploder.hpp
    src/utilities/fpscontroller.hpp
    src/utilities/geomath.hpp
    src/utilities/gridnotifier.hpp
    src/utilities/glm.hpp
	src/utilities/gimgui.hpp
    src/utilities/idmapper.hpp
    src/utilities/idpool.hpp
    src/utilities/interpolationgrid3d.hpp
    src/utilities/interpolators.hpp
    src/utilities/lodepng.hpp
    src/utilities/logger.hpp
    src/utilities/namedenum.hpp
	src/utilities/random.hpp
	src/utilities/rectangle.hpp
    src/utilities/signalcatcher.hpp

	src/utilities/timer.hpp
	src/utilities/tinydir.hpp

    src/script/scriptmessages.hpp
    src/script/scriptchunk.hpp
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
    src/script/asaddons/scriptvec3.hpp
    src/script/asaddons/scriptu8vec3.hpp
    src/script/asaddons/scripti64vec3.hpp
    src/script/asaddons/scriptbuilder.hpp
    src/script/asaddons/scriptstdstring.hpp
    src/script/asaddons/scriptprimitivealiases.hpp
    src/script/callers/scriptcaller.hpp
    src/script/callers/frametimecaller.hpp
    src/script/callers/gameeventcaller.hpp
    src/script/callers/ongroundcaller.hpp
    src/script/callers/chunkeventcaller.hpp
    src/script/callers/worldcaller.hpp
    src/script/interfaces/animationinterface.hpp
    src/script/interfaces/chunkinterface.hpp
    src/script/interfaces/entityinterface.hpp
    src/script/interfaces/extensioninterface.hpp
    src/script/interfaces/worldinterface.hpp
    src/script/interfaces/mathsinterface.hpp
    src/script/interfaces/physicsinterface.hpp
    src/script/interfaces/printinterface.hpp
    src/script/interfaces/randominterface.hpp
    src/script/interfaces/scriptinterface.hpp
    src/script/interfaces/stringinterface.hpp

    src/ui/debugui.hpp

	src/world/explorationmanager.hpp
	src/world/biome.hpp
	src/world/biomedefines.hpp
	src/world/biomeprovider.hpp
	src/world/chunk.hpp
	src/world/chunkmap.hpp
    src/world/field.hpp
    src/world/fieldgenerator.hpp
    src/world/valuemap.hpp
    src/world/highlightmanager.hpp
    src/world/modmanager.hpp
    src/world/raycaster.hpp
    src/world/worlddata.hpp
    src/world/worlddefines.hpp
    src/world/worldentry.hpp
	src/world/worldsystem.hpp
	src/world/worldmessages.hpp
    src/world/chunkprovider.hpp
    src/world/generation/chunkgenerator.hpp
)

find_package(Featherkit REQUIRED COMPONENTS entity structure ui sdl2 util rendering)
find_package(GLM)
find_package(SDL2)
find_package(OpenGL REQUIRED)
find_package(ENet)
find_package(angelscript)
find_package(JsonCpp REQUIRED)
find_package(Freetype REQUIRED)

if(USE_CTAGS)
    find_program(CTAGS_FOUND ctags)
endif(USE_CTAGS)

include_directories(include)

if(NOT SDL2_FOUND)
	link_directories(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/SDL-2.0/lib)
endif(NOT SDL2_FOUND)

if(NOT MSVC)
    set(CPP_14_FLAG -std=c++14)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CPP_14_FLAG} -pthread")

if(BUILD_GAME)
    add_executable(${project_name} ${header_files} ${source_files})
    
    if(SDL2_FOUND)
    	include_directories(${SDL2_INCLUDE_DIR})
    	target_link_libraries(${project_name} ${SDL2_LIBRARY})
    else(SDL2_FOUND)
    	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/SDL-2.0/include)
    	target_link_libraries(${project_name} SDL2main SDL2)
    endif()
    
    #instead of setting compiler flags globally. SFML doesn't like building with this flag on.
    #set_target_properties(${project_name} PROPERTIES COMPILE_FLAGS "-std=c++14 -pthread")
    if(NOT MSVC)
        set(CPP_14_FLAG -std=c++14)
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CPP_14_FLAG} -pthread")
    
    #But that doesn't work in windows.
    #if(WIN32)
        #    add_definitions(-std=c++14 -pthread)
        #	add_definitions(-O4)
        #endif(WIN32)
    
    if(ENET_FOUND)
    	include_directories(${ENet_INCLUDE_DIRS})
    	target_link_libraries(${project_name} ${ENet_LIBRARIES})
    endif(ENET_FOUND)

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

    include_directories(${FREETYPE_INCLUDE_DIRS})

    find_package(GimGui REQUIRED)
    if(GIMGUI_FOUND)
        include_directories(${GIMGUI_INCLUDE_DIRS})
        target_link_libraries(${project_name} ${GIMGUI_LIBRARIES})
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

    if(USE_CTAGS)
        if(CTAGS_FOUND)
            set_source_files_properties(tags PROPERTIES GENERATED true)
            add_custom_target(tags
                COMMAND ctags -R --c++-kinds=+p --fields=+iaS --extra=+q .
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
            add_dependencies(${project_name} tags)
        endif(CTAGS_FOUND)
    endif(USE_CTAGS)

endif(BUILD_GAME)

#tests
if(BUILD_TESTS)
    add_executable(tests "test/main.cpp"
        "test/resources/resourcecachetest.cpp"
        "test/resources/resourcepathtonametest.cpp"
        "test/utilities/gridnotifiertest.cpp"
        "test/utilities/interpolationgrid3dtest.cpp"
        "test/utilities/directorycreatortest.cpp"
        "test/utilities/scopeddirectorytest.cpp"
        "test/utilities/idmapperpooltest.cpp"
        "test/utilities/namedenumtest.cpp"
        "test/world/chunktest.cpp"
        "test/world/coordinatetest.cpp"
        "test/world/explorationmanagertest.cpp"
        "test/world/highlightmanagertest.cpp"
        "test/world/modmanagertest.cpp"
		"test/script/scriptchunktest.cpp"

        "test/testhelpers.cpp"
        "test/scopeddirectory.cpp"
        "test/network/serializetest.cpp"
        "test/network/chunkrequesthandlertest.cpp"

        "src/world/highlightmanager.cpp"
        "src/world/modmanager.cpp"
        "src/utilities/coordinateconverter.cpp"
        "src/utilities/directorycreator.cpp"
        "src/utilities/distancechecker.cpp"
        "src/utilities/folderexploder.cpp"
        "src/world/chunk.cpp"
        "src/world/explorationmanager.cpp" "src/utilities/coordinateconverter.cpp"
        "src/networking/messageserializer.cpp"
        "src/networking/chunkrequesthandler.cpp"
        "src/script/scriptchunk.cpp"
        "src/script/asaddons/scriptarray.cpp"
        "src/resources/resourcepathtoname.cpp"
        )

    add_executable(interpolation_visual "test/visual/interpolationvisual.cpp")

    target_link_libraries(tests ${FEATHERKIT_LIBRARIES})
    target_link_libraries(interpolation_visual ${FEATHERKIT_LIBRARIES} ${SDL2_LIBRARY})
    
	include_directories(${ANGELSCRIPT_INCLUDE_DIRECTORY})
    target_link_libraries(tests ${ANGELSCRIPT_LIBRARY} -lpthread)
endif(BUILD_TESTS)
	
	
if(BUILD_PROFILING)
    #profiling
    add_executable(profile_modmanager 
		"profile/world/modmanagerprofile.cpp" 
		
		"src/world/modmanager.cpp" 
		"src/world/chunk.cpp" 
		"src/utilities/directorycreator.cpp" 
		"src/utilities/coordinateconverter.cpp"
		)
		
    target_link_libraries(profile_modmanager ${FEATHERKIT_LIBRARIES})
endif(BUILD_PROFILING)


