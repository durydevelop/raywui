######################################################################################################################
# This CMake project file is based on this guide https://github.com/robotology/how-to-export-cpp-library/tree/master #
######################################################################################################################
cmake_minimum_required(VERSION 3.28)

##set(PLATFORM Desktop CACHE STRING "Platform to build for")
#set(PLATFORM Web CACHE STRING "Platform to build for")
#set(PLATFORM DRM CACHE STRING "Platform to build for")

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
    VERSION 0.2.6
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

#message("CMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if (NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20 CACHE STRING "Standard C++ to use")
elseif (CMAKE_CXX_STANDARD LESS 17)
    message(WARNING "This project supports al least C++17, current CMAKE_CXX_STANDARD is ${CMAKE_CXX_STANDARD} and will set to 20")
    set(CMAKE_CXX_STANDARD 20 CACHE STRING "Standard C++ to use" FORCE)
endif()

#target_compile_features(... cxx_std_20)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#### Cmake modules helper
list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
list (REMOVE_DUPLICATES CMAKE_MODULE_PATH)
include(DCMakeHelper)

#### Default Options
option(${PROJECT_NAME}_BUILD_SHARED_LIBS "Build shared libraries (.dll/.so) instead of static ones (.lib/.a)" OFF)
if (PROJECT_IS_TOP_LEVEL)
    option(${PROJECT_NAME}_BUILD_EXAMPLES "Build examples" ON)
    option(${PROJECT_NAME}_INSTALL "Create install target" ON)
else()
    option(${PROJECT_NAME}_BUILD_EXAMPLES "Build examples" OFF)
    option(${PROJECT_NAME}_INSTALL "Create install target" OFF)
endif()
# Testing
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
get_filename_component(dpplib_ROOT /home/fabio/Dev/cpp/lib/dpplib ABSOLUTE)
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
else()
    set(${PROJECT_NAME}_BUILD_SHARED_LIBS OFF)
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

#### Sources

# Source library files
set(${PROJECT_NAME}_SRC)
set(${PROJECT_NAME}_HDR)
# Include base dir
set(${PROJECT_NAME}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src)

## Add sources
add_subdirectory(src/core)
add_subdirectory(src/widgets)
add_subdirectory(src/raygui)

#### Dependencies
#cmake_policy(SET CMP0077 NEW) # for BUILD_SHARED_LIBS and BUILD_EXAMPLES in raylib
#find_package(Raylib REQUIRED)
#set(${PROJECT_NAME}_INCLUDE_DIRS ${${PROJECT_NAME}_INCLUDE_DIRS} ${raylib_INCLUDE_DIRECTORIES})
#set(raylib_VERBOSE 1)
add_subdirectory(src/external/raylib)
#print_var(raylib)

#if (NOT TARGET dpplib)
    find_package(Dpplib REQUIRED)
    #set(${LIBRARY_TARGET_NAME}_INCLUDE_DIRS ${${LIBRARY_TARGET_NAME}_INCLUDE_DIRS} ${dpplib_INCLUDE_DIRECTORIES})
#endif()

#message("${PROJECT_NAME}_SRC=${${PROJECT_NAME}_SRC}")
#message("${PROJECT_NAME}_INCLUDE_DIRS=${${PROJECT_NAME}_INCLUDE_DIRS}")

#### Setup library target
add_library(${PROJECT_NAME} ${${PROJECT_NAME}_SRC})

# Header pubblici
target_sources(${PROJECT_NAME}
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS src
        FILES ${${PROJECT_NAME}_HDR}
)

# Alias per comodità
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Proprietà della libreria
set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

# Collego raywui a raylib
target_link_libraries(${PROJECT_NAME}
    PUBLIC
        raylib
)

# Percorsi include
target_include_directories(${PROJECT_NAME}
    PUBLIC
        "$<BUILD_INTERFACE:${${PROJECT_NAME}_INCLUDE_DIRS}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/external/raylib/src>" # dove sta raylib.h
)

#### Installazione e export
if (${PROJECT_NAME}_INSTALL)

    # Installa la libreria e headers
    install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}-targets
        LIBRARY       DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT shlib
        ARCHIVE       DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT lib
        RUNTIME       DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT bin
        FILE_SET HEADERS DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}"
    )

    # Esporta i target locali
    install(EXPORT ${PROJECT_NAME}-targets
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
        FILE ${PROJECT_NAME}-targets.cmake
    )

    # Genera ConfigVersion.cmake
    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
    )

    # Config.cmake da template
    configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    )

    # Installa i file di configurazione
    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    )

    # Target uninstall
    include(AddUninstallTarget)
endif()

#### Integration tests
if(${PROJECT_NAME}BUILD_TESTING)
    set(USE_EXTERNAL_googletest ON)
    find_package(GoogleTest)
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
    add_subdirectory(doxygen)
#    include(doxygen/Doxygen.cmake)
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
