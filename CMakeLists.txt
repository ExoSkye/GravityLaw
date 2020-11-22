cmake_minimum_required(VERSION 3.2)
project(GravityLaw)

set(CMAKE_CXX_STANDARD 20)

add_executable(GravityLaw main.cpp tracy/TracyClient.cpp)

add_subdirectory("libs/glm")
add_subdirectory("libs/SDL")

target_link_libraries(GravityLaw PUBLIC glm PUBLIC SDL2 PUBLIC SDL2main PUBLIC pthread PUBLIC dl)
target_compile_definitions(GravityLaw PUBLIC TRACY_ENABLE)
target_include_directories(GravityLaw PUBLIC libs/glm PUBLIC libs/SDL/include PUBLIC tracy)