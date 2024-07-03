#.rst:
# AddUninstallTarget
# ------------------
#
# Add the "uninstall" target for your project::
#
#   include(AddUninstallTarget)
#
#
# will create a file cmake_uninstall.cmake in the build directory and add a
# custom target uninstall that will remove the files installed by your package
# (using install_manifest.txt)

#=============================================================================
# Copyright 2008-2013 Kitware, Inc.
# Copyright 2013 Istituto Italiano di Tecnologia (IIT)
#   Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)


if(DEFINED __ADD_UNINSTALL_TARGET_INCLUDED)
  return()
endif()
set(__ADD_UNINSTALL_TARGET_INCLUDED TRUE)

# Get only the last one (added by this lib)
foreach(DIR ${CMAKE_MODULE_PATH})
    set (IN_FILE ${DIR}/uninstall.cmake.in)
    message_c(${BOLD_WHITE} "finding ${IN_FILE}")
    if (EXISTS "${IN_FILE}")
        configure_file(
            ${IN_FILE}
            "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
            IMMEDIATE @ONLY
        )
        set(UNINSTALL_FOUND TRUE)
        break()
    endif()
endforeach()

if (NOT UNINSTALL_FOUND)
    message_c(${BOLD_YELLOW} "uninstall.cmake.in file not found, 'uninstall' target is not set")
endif()

if("${CMAKE_GENERATOR}" MATCHES "^(Visual Studio|Xcode)")
    set(_uninstall "UNINSTALL")
else()
    set(_uninstall "uninstall")
endif()

if (TARGET ${_uninstall})
    set(_uninstall "Uninstall")
endif()

add_custom_target(${_uninstall} COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
set_property(TARGET ${_uninstall} PROPERTY FOLDER "CMakePredefinedTargets")
