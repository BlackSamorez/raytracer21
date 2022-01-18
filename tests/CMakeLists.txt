include_directories(.)

Include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v2.13.8
)

FetchContent_MakeAvailable(Catch2)
option(TEST_SOLUTION "Build solution" OFF)
option(ENABLE_PRIVATE_TESTS "Enable private tests" OFF)
option(GRADER "Building on CI" OFF)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)
include(CTest)
include(Catch)

add_executable(compile_catch2 catch2_main_holder.cpp)
target_link_libraries(compile_catch2 PRIVATE Catch2::Catch2)
catch_discover_tests(compile_catch2)

add_subdirectory(geometry)

find_package(PNG)
find_package(JPEG)

if (${PNG_FOUND} AND ${JPEG_FOUND})
    message(STATUS "PNG and JPEG found! Enabling related tests")

    add_subdirectory(reader)
    add_subdirectory(debug_mode)
    add_subdirectory(raytracer)
else()
    message(STATUS "raytracer disabled. PNG:${PNG_FOUND} JPEG:${JPEG_FOUND}")
endif()
