
project(rw-generate)

add_executable(rw-generate rw-generate.cpp)
target_link_libraries(rw-generate PUBLIC raywui)
