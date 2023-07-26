cmake_minimum_required(VERSION 3.20)

project(pong VERSION 1.0.0 LANGUAGES C)

find_package(raylib 4.5.0 QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib
        GIT_TAG        fec9613
    )
    FetchContent_MakeAvailable(raylib)
endif()

add_executable(${PROJECT_NAME} src/main.c)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
target_link_libraries(${PROJECT_NAME} PRIVATE m)
target_compile_features(${PROJECT_NAME} PUBLIC c_std_11)