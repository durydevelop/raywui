######################################################################################################################
# This CMake project file is based on this guide https://github.com/robotology/how-to-export-cpp-library/tree/master #
######################################################################################################################
cmake_minimum_required(VERSION 3.13)

#### WASM support
# CMAKE_TOOLCHAIN_FILE must be set before project() otherwise it does not take effect
if (${PLATFORM} MATCHES "Web" OR EMSCRIPTEN)
    if (NOT CMAKE_TOOLCHAIN_FILE)
        if (DEFINED ENV{EMSDK})
            set(CMAKE_TOOLCHAIN_FILE "$ENV{EMSDK}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake" CACHE STRING "" FORCE)
            #include(${CMAKE_TOOLCHAIN_FILE})
        else()
            message(FATAL_ERROR  "Emscripten non found, cannot compile on PLATFORM Web")
        endif()
    endif()

    if (CMAKE_HOST_LINUX)
        # EMSCRIPTEN seems not set PLATFORM_NAME pre-processor directive
        add_compile_definitions(PLATFORM_NAME="linux") # ...for now emscripten build works only on linux
    endif()

	# Da provare ad attivare
	set(USE_ZLIB OFF)

    # Flags for raygui
    set(EMSDK_FLAGS "-s ASYNCIFY -s USE_GLFW=3 -s WASM=1 -s USE_ZLIB=0 -s GL_ENABLE_GET_PROC_ADDRESS=1 -s ASSERTIONS=1" CACHE STRING "" FORCE)
    
    # Add compile flags
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EMSDK_FLAGS}" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EMSDK_FLAGS}" CACHE STRING "" FORCE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${EMSDK_FLAGS}" CACHE STRING "" FORCE)
endif()

#### Create project
project(raywui
    VERSION 0.2.2
    LANGUAGES C CXX
)

#### Common libraries options
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
include(GNUInstallDirs)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
if(MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS yes)

#### Cmake modules helper
list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
list (REMOVE_DUPLICATES CMAKE_MODULE_PATH)
include(DCMakeHelper)

#### Default Options
if (PROJECT_IS_TOP_LEVEL)
    option(${PROJECT_NAME}_BUILD_EXAMPLES "Build examples" ON)
    option(${PROJECT_NAME}_INSTALL "Create install target" ON)
else()
    option(${PROJECT_NAME}_BUILD_EXAMPLES "Build examples" OFF)
    option(${PROJECT_NAME}_INSTALL "Create install target" OFF)
endif()
option(${PROJECT_NAME}_BUILD_TESTING "Build tests suite" OFF)
## USE_EXTERNAL_RAYLIB
# If USE_EXTERNAL_RAYLIB is ON one of these variables can be specified:
# Use custom git repo, i.e.:
# set(raylib_GIT_REPOSITORY https://github.com/raysan5/raylib.git)
# Use a local folder, i.e.:
# get_filename_component(raylib_ROOT ../../Raylib ABSOLUTE)
# If none specified:
# First:  Try to find it in system
# Second: Load from default git repo (https://github.com/raysan5/raylib.git)
option(USE_EXTERNAL_RAYLIB "Do not use embedded version of Raylib library" OFF)
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "No wayland" FORCE) # Set to OFF if failed to find wayland-scanner

## USE_EXTERNAL_DPPLIB
# If USE_EXTERNAL_DPPLIB is ON one of these variables can be specified:
# Use custom git repo, i.e.:
# set(dpplib_GIT_REPOSITORY git@gitlab.com:durydevelop/cpp/lib/dpptools.git)
# Use a local folder, i.e.:
# get_filename_component(dpplib_ROOT ../../Dpplib ABSOLUTE)
# If none specified:
# First:  Try to find it in system
# Second: Load from default git repo (git@gitlab.com:durydevelop/cpp/lib/dpptools.git)
option(USE_EXTERNAL_DPPLIB "Do not use embedded version of Dpplib library" ON)

#### Handle options (also set ${PROJECT_NAME}_<option> and <option>)
# EMSCRIPTEN
if (${PROJECT_NAME}_EMSCRIPTEN OR EMSCRIPTEN)
    set(${PROJECT_NAME}_EMSCRIPTEN ON)
    set(EMSCRIPTEN ON)
else()
    set(${PROJECT_NAME}_EMSCRIPTEN OFF)
    set(EMSCRIPTEN OFF)
endif()
# Shared / static lib
if (${PROJECT_NAME}_BUILD_SHARED_LIBS OR BUILD_SHARED_LIBS)
    set(${PROJECT_NAME}_BUILD_SHARED_LIBS ON)
endif()
# Examples
if (${PROJECT_NAME}_BUILD_EXAMPLES OR BUILD_EXAMPLES)
    set(${PROJECT_NAME}_BUILD_EXAMPLES ON)
    set(BUILD_EXAMPLES OFF) # to prevent Raylib to build his examples
endif()
# Tests
if (${PROJECT_NAME}_BUILD_TESTING OR BUILD_TESTING)
    set(${PROJECT_NAME}_BUILD_TESTING ON)
    set(BUILD_TESTING ON)
else()
    set(${PROJECT_NAME}_BUILD_TESTING OFF)
    set(BUILD_TESTING OFF)
endif()
# Install
if (${PROJECT_NAME}_INSTALL OR ${PROJECT_NAME_UPPER}_INSTALL)
    set(${PROJECT_NAME}_INSTALL ON)
    set(${PROJECT_NAME_UPPER}_INSTALL ON)
else()
    set(${PROJECT_NAME}_INSTALL OFF)
    set(${PROJECT_NAME_UPPER}_INSTALL OFF)
endif()
# Raylib external
if (${PROJECT_NAME}_USE_EXTERNAL_RAYLIB OR USE_EXTERNAL_RAYLIB)
    set(${PROJECT_NAME}_USE_EXTERNAL_RAYLIB ON)
    set(USE_EXTERNAL_RAYLIB ON)
else()
    set(${PROJECT_NAME}_USE_EXTERNAL_RAYLIB OFF)
    set(USE_EXTERNAL_RAYLIB OFF)
endif()
# Dpplib external
if (${PROJECT_NAME}_USE_EXTERNAL_DPPLIB OR USE_EXTERNAL_DPPLIB)
    set(${PROJECT_NAME}_USE_EXTERNAL_DPPLIB ON)
    set(USE_EXTERNAL_DPPLIB ON)
else()
    set(${PROJECT_NAME}_USE_EXTERNAL_DPPLIB OFF)
    set(USE_EXTERNAL_DPPLIB OFF)
endif()

#### Encourage user to specify a build type (e.g. Release, Debug, etc.), otherwise set it to Debug.
if(NOT CMAKE_CONFIGURATION_TYPES)
    if(NOT CMAKE_BUILD_TYPE)
        message(STATUS "Setting build type to 'Debug' as none was specified.")
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY VALUE "Debug")
    endif()
endif()

#### Add target
add_subdirectory(src)
#target_include_directories(${PROJECT_NAME} PUBLIC ${EMSCRIPTEN_SYSROOT}/include)

#### Install stuff
if (${PROJECT_NAME}_INSTALL)
    # Enable RPATH support for installed binaries and libraries
    include(AddInstallRPATHSupport)
    add_install_rpath_support(
        BIN_DIRS "${CMAKE_INSTALL_FULL_BINDIR}"
        LIB_DIRS "${CMAKE_INSTALL_FULL_LIBDIR}"
        INSTALL_NAME_DIR "${CMAKE_INSTALL_FULL_LIBDIR}"
        USE_LINK_PATH
    )

    # Create and install CMake configuration files
    include(InstallBasicPackageFiles)
    install_basic_package_files(${PROJECT_NAME}
        VERSION ${${PROJECT_NAME}_VERSION}
        COMPATIBILITY AnyNewerVersion
        VARS_PREFIX ${PROJECT_NAME}
        NO_CHECK_REQUIRED_COMPONENTS_MACRO
    )
    # Add the uninstall target
    include(AddUninstallTarget)
endif()

#### Integration tests
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(test)
endif()

#### Examples
if (${PROJECT_NAME}_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

#### Doxygen support
if (PROJECT_IS_TOP_LEVEL)
    set(DOXYGEN_THEME_NAME example)
    set(DOXYGEN_LANGUAGE Italian)
    #add_subdirectory(doxygen)
    include(doxygen/Doxygen.cmake)
endif()

## C++ Pre-processor variables
# raywui_VERSION
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_NAME}_VERSION="${PROJECT_VERSION}")
# RAYWUI_VERSION (uppercase)
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_NAME_UPPER}_VERSION="${PROJECT_VERSION}")

## Print summary
message_c(${BOLD_CYAN} "Summary:")
message_c(${BOLD_CYAN} "Project name:             ${PROJECT_NAME} - Version: ${PROJECT_VERSION}")
message_c(${BOLD_CYAN} "PROJECT_IS_TOP_LEVEL:     ${PROJECT_IS_TOP_LEVEL}")
message_c(${BOLD_CYAN} "BUILD_SHARED_LIBS:        ${${PROJECT_NAME}_BUILD_SHARED_LIBS}")
message_c(${BOLD_CYAN} "USE_EXTERNAL_RAYLIB:      ${USE_EXTERNAL_RAYLIB}")
message_c(${BOLD_CYAN} "USE_EXTERNAL_DPPLIB:      ${USE_EXTERNAL_DPPLIB}")
message_c(${BOLD_CYAN} "Build examples:           ${${PROJECT_NAME}_BUILD_EXAMPLES}")
message_c(${BOLD_CYAN} "Build testing:            ${${PROJECT_NAME}_BUILD_TESTING}")
if (EMSCRIPTEN)
    message_c(${BOLD_CYAN} "WASM support:             ON (EMSCRIPTEN)")
    message_c(${BOLD_CYAN} "CMAKE_TOOLCHAIN_FILE:     ${CMAKE_TOOLCHAIN_FILE}")
else()
    message_c(${BOLD_CYAN} "WebSocket backend:        IXWebSocket")
    message_c(${BOLD_CYAN} "USE_EXTERNAL_IXWEBSOCKET: ${USE_EXTERNAL_IXWEBSOCKET}")
endif()

#[[
## Setup Target (as SHARED)
## Build shared libraries (.dll/.so) instead of static ones (.lib/.a)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_library(${PROJECT_NAME} SHARED ${SRC_FILES})
if(MSVC)
    # Workaround for some projects
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()
# Include dirs
string(REPLACE ";" "$<SEMICOLON>" LSRC_DIRS "${SRC_DIRS}")
target_include_directories(
    ${PROJECT_NAME} PUBLIC
    "$<BUILD_INTERFACE:${LSRC_DIRS}>"
#    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>"
)

## External dependencies
# Raylib
message_c("Get Raylib...")
set(BUILD_SHARED_LIBS ON)
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "No wayland" FORCE) # TODO: detect wayland_scanner error
#set(raylib_VERBOSE 1)
find_package(Raylib)
target_link_libraries(raylib)

# Dpplib
message_c("Get Dpplib...")
find_package(Dpplib)
target_link_libraries(${PROJECT_NAME} PUBLIC dpplib)
print_var(dpplib)
message_c("SRC_DIRS=${SRC_DIRS}")
message_c("SRC_FILES=${SRC_FILES}")

## C++ Pre-processor variables
# raywui_VERSION
add_compile_definitions(${PROJECT_NAME}_VERSION="${PROJECT_VERSION}")
# RAYWUI_VERSION
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
add_compile_definitions(${PROJECT_NAME_UPPER}_VERSION="${PROJECT_VERSION}")

## Print summary
message_c(${BOLD_CYAN} "Summary:")
message_c(${BOLD_CYAN} "Project name:   ${PROJECT_NAME} - Version: ${PROJECT_VERSION}")
message_c(${BOLD_CYAN} "Build type:     ${CMAKE_BUILD_TYPE}")
message_c(${BOLD_CYAN} "Build as:       SHARED LIB")
message_c(${BOLD_CYAN} "WASM support:   ${${PROJECT_NAME}_EMSCRIPTEN}")
message_c(${BOLD_CYAN} "Build examples: ${BUILD_EXAMPLES} TODO")
message_c(${BOLD_CYAN} "Build tests:    ${BUILD_TESTS} TODO")
]]#