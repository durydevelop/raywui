
project(rw-show VERSION 1.0.0)

add_executable(${PROJECT_NAME} rw-show.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC raywui)

# PROJECT_VERSION for pre-processor
target_compile_definitions(${PROJECT_NAME} PUBLIC PROJECT_VERSION="${PROJECT_VERSION}")

add_custom_target(copy_layout_files ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/layouts
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/layouts
)
