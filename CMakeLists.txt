cmake_minimum_required(VERSION 3.31)

# This project uses SFML - so include it using FetchContent
include(FetchContent)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)

FetchContent_MakeAvailable(SFML)

project(PredatorAndPrey LANGUAGES CXX)

add_executable(Simulation)
set_target_properties(Simulation PROPERTIES CXX_STANDARD 23)
set_target_properties(Simulation PROPERTIES CXX_STANDARD_REQUIRED ON)
set_target_properties(Simulation PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(Simulation PRIVATE include)
target_sources(Simulation PRIVATE src/main.cpp)
target_link_libraries(Simulation PRIVATE sfml-graphics sfml-window sfml-system)
