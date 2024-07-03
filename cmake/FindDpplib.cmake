#### Find Dpplib lib

function(get_package_from_git)
    include(FetchContent)
    set(FETCHCONTENT_QUIET OFF)
    set(FETCHCONTENT_UPDATES_DISCONNECTED OFF)
    set(FETCHCONTENT_BASE_DIR _deps)
    FetchContent_Declare(
        ${ARGV0}
        GIT_REPOSITORY ${ARGV1}
        #GIT_TAG main
        #GIT_PROGRESS TRUE
    )
    # Check if population has already been performed
    FetchContent_GetProperties(${ARGV0})
    if(NOT ${ARGV0}_POPULATED)
        FetchContent_Populate(${ARGV0})
        add_subdirectory(${${ARGV0}_SOURCE_DIR} ${${ARGV0}_BINARY_DIR})
    endif()
    FetchContent_MakeAvailable(${ARGV0})
endfunction()

if (USE_EXTERNAL_DPPLIB)
    # Do not use internal Dpplib
    message_c(${BOLD_MAGENTA} "${BOLD_WHITE}${PROJECT_NAME}${BOLD_MAGENTA} is finding <${BOLD_CYAN}dpplib${BOLD_MAGENTA}>")
    if (dpplib_GIT_REPOSITORY)
        # GIT forced
        get_package_from_git(dpplib ${dpplib_GIT_REPOSITORY})
        if (dpplib_FOUND)
            message_c(${BOLD_MAGENTA} "${BOLD_WHITE}${PROJECT_NAME}${BOLD_MAGENTA} <${BOLD_CYAN}dpplib${BOLD_MAGENTA}> loaded custom repo")
        endif()
    elseif(dpplib_ROOT)
        # Local FORCED
        add_subdirectory(${dpplib_ROOT} _deps/dpplib)
    else()
        # Try to find in system
        find_package(dpplib QUIET CONFIG)
        if (dpplib_FOUND)
            get_target_property(dpplib_INCLUDE_DIRECTORIES dpplib::dpplib INTERFACE_INCLUDE_DIRECTORIES)
            message_c(${BOLD_MAGENTA} "${BOLD_WHITE}${PROJECT_NAME}${BOLD_MAGENTA} <${BOLD_CYAN}dpplib${BOLD_MAGENTA}> found in system")
        else()
            # Load from official repo
            message_c(${BOLD_MAGENTA} "loading from official repo")
            get_package_from_git(dpplib git@gitlab.com:durydevelop/cpp/lib/dpptools.git)
            set(dpplib_FOUND TRUE)
            message_c(${BOLD_MAGENTA} "${BOLD_WHITE}${PROJECT_NAME}${BOLD_MAGENTA} <${BOLD_CYAN}dpplib${BOLD_MAGENTA}> loaded from official repo")
        endif()
    endif()
else()
    # Use embedded one
    message_c(${BOLD_MAGENTA} "${BOLD_WHITE}${PROJECT_NAME}${BOLD_MAGENTA} use <${BOLD_CYAN}dpplib${BOLD_MAGENTA}> embedded")
    add_subdirectory(external/dpplib)
    install(
        DIRECTORY ${dpplib_SOURCE_DIR}/dpplib
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_TARGET_NAME}"
        FILES_MATCHING PATTERN "*.h"
    )
endif()

#get_target_property(dpplib_SOURCE_DIR dpplib SOURCE_DIR)
#get_target_property(dpplib_INCLUDE_DIRECTORIES dpplib INTERFACE_INCLUDE_DIRECTORIES)

#print_var(dpplib)
#print_target_prop(dpplib)
