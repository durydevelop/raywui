
project(rw-generate VERSION 0.0.1)

add_executable(${PROJECT_NAME} rw-generate.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC raywui)

# PROJECT_VERSION for pre-processor
target_compile_definitions(${PROJECT_NAME} PUBLIC PROJECT_VERSION="${PROJECT_VERSION}")