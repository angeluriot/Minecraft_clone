#Version of cmake
cmake_minimum_required(VERSION 2.8)

#Name of the project
project(Minecraft_clone)

#Tells where are the destination directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY   ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY   ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY   ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#C++11 in Debug mode
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_STANDARD 17)

#Some options
#TODO add another option if a new library is to be added (follow this)

#Windows / MINGW (Code::Blocks)
if(MINGW)
    MESSAGE(STATUS "Compiling for MINGW")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        SET(GLEW_INCLUDE_PATH       ${CMAKE_SOURCE_DIR}/libs/include        CACHE PATH "Path to GLEW include location")
        SET(SDL2_INCLUDE_PATH       ${CMAKE_SOURCE_DIR}/libs/include        CACHE PATH "Path to SDL2 library location")
        SET(SDL2_LIBRARY_PATH       ${CMAKE_SOURCE_DIR}/libs/mingw/x64/bin  CACHE PATH "Path to SDL2 include location")
        SET(SDL2_IMAGE_LIBRARY_PATH ${CMAKE_SOURCE_DIR}/libs/mingw/x64/bin  CACHE PATH "Path to SDL2 IMAGE include location")
        SET(SDL2_IMAGE_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/libs/include        CACHE PATH "Path to SDL2 IMAGE library location")
        SET(GLEW_LIBRARY_PATH       ${CMAKE_SOURCE_DIR}/libs/mingw/x64/bin  CACHE PATH "Path to GLEW library location")

    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        SET(GLEW_INCLUDE_PATH       ${CMAKE_SOURCE_DIR}/libs/include        CACHE PATH "Path to GLEW include location")
        SET(SDL2_INCLUDE_PATH       ${CMAKE_SOURCE_DIR}/libs/include        CACHE PATH "Path to SDL2 library location")
        SET(SDL2_LIBRARY_PATH       ${CMAKE_SOURCE_DIR}/libs/mingw/x86/bin  CACHE PATH "Path to SDL2 include location")
        SET(SDL2_IMAGE_LIBRARY_PATH ${CMAKE_SOURCE_DIR}/libs/mingw/x86/bin  CACHE PATH "Path to SDL2 IMAGE include location")
        SET(SDL2_IMAGE_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/libs/include        CACHE PATH "Path to SDL2 IMAGE library location")
        SET(GLEW_LIBRARY_PATH       ${CMAKE_SOURCE_DIR}/libs/mingw/x86/bin  CACHE PATH "Path to GLEW library location")
    endif()

    #Windows / MSVC (Visual Studio)
elseif(MSVC)
    MESSAGE(STATUS "Compiling for MSVC")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        SET(SDL2_LIBRARY_PATH       ${CMAKE_SOURCE_DIR}/libs/VS/x64  CACHE PATH "Path to SDL2 include location")
        SET(GLEW_LIBRARY_PATH       ${CMAKE_SOURCE_DIR}/libs/VS/x64  CACHE PATH "Path to GLEW library location")
        SET(SDL2_IMAGE_LIBRARY_PATH ${CMAKE_SOURCE_DIR}/libs/VS/x64  CACHE PATH "Path to SDL2 IMAGE include location")
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        SET(SDL2_LIBRARY_PATH       ${CMAKE_SOURCE_DIR}/libs/VS/x86  CACHE PATH "Path to SDL2 include location")
        SET(GLEW_LIBRARY_PATH       ${CMAKE_SOURCE_DIR}/libs/VS/x86  CACHE PATH "Path to GLEW library location")
        SET(SDL2_IMAGE_LIBRARY_PATH ${CMAKE_SOURCE_DIR}/libs/VS/x86  CACHE PATH "Path to SDL2 IMAGE include location")
    endif()

    #Linux
else()
    MESSAGE(STATUS "Compiling for Linux/Unix")
    SET(GLEW_INCLUDE_PATH       /usr/include  CACHE PATH "Path to GLEW include location")
    SET(SDL2_INCLUDE_PATH       /usr/include  CACHE PATH "Path to SDL2 library location")
    SET(SDL2_LIBRARY_PATH       /usr/lib      CACHE PATH "Path to SDL2 include location")
    SET(GLEW_LIBRARY_PATH       /usr/lib      CACHE PATH "Path to GLEW library location")
    SET(GL_LIBRARY_PATH         /usr/lib      CACHE PATH "Path to GL   library location")
    SET(GL_INCLUDE_PATH         /usr/include  CACHE PATH "Path to GL   include location")
    SET(SDL2_IMAGE_LIBRARY_PATH /usr/lib      CACHE PATH "Path to SDL2 IMAGE library location")
    SET(SDL2_IMAGE_INCLUDE_PATH /usr/include/ CACHE PATH "Path to SDL2 IMAGE include location")


    if((NOT EXISTS "${GL_LIBRARY_PATH}/libGL.so" ) AND (NOT EXISTS "${GL_LIBRARY_PATH}/GL.dll" ))
        MESSAGE(FATAL_ERROR "Could not find libGL in ${GL_LIBRARY_PATH}")
    endif()
endif()

#Workaround for MSVC not creating DEBUG and RELEASE folder
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/bin)
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/lib)
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/lib)
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

if(MSVC)
    set(WARNING_FLAGS "${WARNING_FLAGS} /W4")
else()
    set(WARNING_FLAGS "${WARNING_FLAGS} -Wall")
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_FLAGS}")
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   ${WARNING_FLAGS}")

include_directories(SYSTEM include)

if(MSVC OR MINGW)
    include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/libs/include)
endif()

#Check paths
if((NOT EXISTS "${SDL2_LIBRARY_PATH}/libSDL2.so" ) AND (NOT EXISTS "${SDL2_LIBRARY_PATH}/SDL2.dll" ))
    MESSAGE(FATAL_ERROR "Could not find libSDL2 in ${SDL2_LIBRARY_PATH}")
endif()

if((NOT EXISTS "${GLEW_LIBRARY_PATH}/libGLEW.so" ) AND (NOT EXISTS "${GLEW_LIBRARY_PATH}/glew32.dll" ))
    MESSAGE(FATAL_ERROR "Could not find libGLEW in ${GLEW_LIBRARY_PATH}")
endif()

if((NOT EXISTS "${SDL2_IMAGE_LIBRARY_PATH}/libSDL2_image.so" ) AND (NOT EXISTS "${SDL2_IMAGE_LIBRARY_PATH}/SDL2_image.dll" ))
    MESSAGE(FATAL_ERROR "Could not find libSDL2_Image in ${SDL2_IMAGE_LIBRARY_PATH}")
endif()

make_directory("bin")

#Configure Minecraft_clone
file(GLOB_RECURSE SOURCES    sources/*.cpp sources/*.c)
file(GLOB_RECURSE HEADERS include/*.h include/*.hpp)

#TODO add the library path here
link_directories(${SDL2_LIBRARY_PATH} ${GLEW_LIBRARY_PATH} ${SDL2_IMAGE_LIBRARY_PATH})
add_executable(Minecraft_clone ${SOURCES} ${HEADERS})
target_compile_definitions(Minecraft_clone PUBLIC _USE_MATH_DEFINES)

#TODO add another -I parameter (include directory to take account to) and a -l parameter (libraries to link to)
#Normally you have just to modify the target_compile_options

target_include_directories(Minecraft_clone PUBLIC
        ${SDL2_INCLUDE_PATH}
        ${SDL2_IMAGE_INCLUDE_PATH}
        ${GLEW_INCLUDE_PATH}
        ${GL_INCLUDE_PATH})

if(MINGW)
    target_link_libraries(Minecraft_clone PUBLIC
        -lOpenGL32
        -lglew32
        -lSDL2
        -lSDL2_image)

    #Scripts to copy to bin/
    file(GLOB BINRESOURCES ${CMAKE_SOURCE_DIR}/libs/VS/x86/*.dll ${CMAKE_SOURCE_DIR}/libs/VS/x86/*.lib)

    foreach(sourcefile ${BINRESOURCES})
        get_filename_component(dirname  "${sourcefile}" DIRECTORY)
        get_filename_component(filename "${sourcefile}" NAME)
        file(RELATIVE_PATH relpath "${CMAKE_CURRENT_SOURCE_DIR}" "${sourcefile}")

        set(BinOutput ${BinOutput} "${relpath}")

        add_custom_command(
            OUTPUT "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${filename}"
            DEPENDS ${relpath}
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${relpath}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${filename}"
            COMMENT "Generating ${filename}"
            VERBATIM
            )
    endforeach(sourcefile)

    add_custom_target(BinTarget DEPENDS ${BinOutput})
    add_dependencies(Minecraft_clone BinTarget)

elseif(MSVC)
    target_link_libraries(Minecraft_clone general
        "OpenGL32.lib"
        "glew32.lib"
        "SDL2.lib"
        "SDL2main.lib"
        "SDL2_image.lib")

    #Scripts to copy to bin/
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        file(GLOB BINRESOURCES ${CMAKE_SOURCE_DIR}/libs/VS/x64/*.dll ${CMAKE_SOURCE_DIR}/libs/VS/x64/*.lib)
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        file(GLOB BINRESOURCES ${CMAKE_SOURCE_DIR}/libs/VS/x86/*.dll ${CMAKE_SOURCE_DIR}/libs/VS/x86/*.lib)
    endif()

    foreach(sourcefile ${BINRESOURCES})
        get_filename_component(dirname  "${sourcefile}" DIRECTORY)
        get_filename_component(filename "${sourcefile}" NAME)
        file(RELATIVE_PATH relpath "${CMAKE_CURRENT_SOURCE_DIR}" "${sourcefile}")

        set(BinOutput ${BinOutput} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${filename}")

        add_custom_command(
            OUTPUT "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${filename}"
            DEPENDS ${relpath}
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${relpath}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${filename}"
            COMMENT "Generating ${filename}"
            VERBATIM
            )
    endforeach(sourcefile)

    add_custom_target(BinTarget DEPENDS ${BinOutput})
    add_dependencies(Minecraft_clone BinTarget)
else()
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    target_link_libraries(Minecraft_clone PUBLIC
        ${OPENGL_gl_LIBRARY}
        ${GLEW_LIBRARIES}
        -lSDL2
        -lSDL2_image)
endif()


#Scripts to copy to bin/
file(GLOB SHADERRESOURCES
    shaders/*
    textures/*
	textures/lens_flare/*)
foreach(sourcefile ${SHADERRESOURCES})
    get_filename_component(dirname  "${sourcefile}" DIRECTORY)
    get_filename_component(filename "${sourcefile}" NAME)
    file(RELATIVE_PATH relpath "${CMAKE_CURRENT_SOURCE_DIR}" "${sourcefile}")
    file(RELATIVE_PATH dirrelpath "${CMAKE_CURRENT_SOURCE_DIR}" "${dirname}")

    set(ShadersOutput ${ShadersOutput} "${relpath}")

    add_custom_command(
        OUTPUT "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${relpath}"
        DEPENDS ${relpath}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${dirrelpath}"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${relpath}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${dirrelpath}/${filename}"
        COMMENT "Generating ${relpath}"
        VERBATIM
        )
endforeach(sourcefile)

add_custom_target(ShaderTarget DEPENDS ${ShadersOutput})
add_dependencies(Minecraft_clone ShaderTarget)
