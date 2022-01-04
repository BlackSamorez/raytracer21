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

set(CMAKE_CXX_COMPILER "/usr/bin/g++-10")

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)
include(CTest)
include(Catch)

add_executable(compile_catch2 catch2_main_holder.cpp)
target_link_libraries(compile_catch2 PRIVATE Catch2::Catch2)
catch_discover_tests(compile_catch2)

add_executable(test_geometry geometry/test_geometry.cpp)
target_link_libraries(test_geometry PRIVATE Catch2::Catch2)
catch_discover_tests(test_geometry)

add_executable(test_reader reader/test_reader.cpp)
target_link_libraries(test_reader PRIVATE Catch2::Catch2)
target_compile_definitions(test_reader PUBLIC PROGRAM_DIR="${CMAKE_CURRENT_SOURCE_DIR}/reader/")
catch_discover_tests(test_reader)

find_package(PNG)
find_package(JPEG)

if (${PNG_FOUND} AND ${JPEG_FOUND})
    message(STATUS "PNG and JPEG found! Enabling related tests")

    add_executable(test_debug_mode debug_mode/test_debug_mode.cpp)
    target_link_libraries(test_debug_mode PRIVATE Catch2::Catch2)
    target_link_libraries(test_debug_mode PRIVATE ${PNG_LIBRARY} ${JPEG_LIBRARIES})
    target_compile_definitions(test_debug_mode PUBLIC PROGRAM_DIR="${CMAKE_CURRENT_SOURCE_DIR}/debug_mode/")
    catch_discover_tests(test_debug_mode)

    add_executable(test_raytracer raytracer/test_raytracer.cpp)
    target_link_libraries(test_raytracer PRIVATE Catch2::Catch2)
    target_link_libraries(test_raytracer PRIVATE ${PNG_LIBRARY} ${JPEG_LIBRARIES})
    target_compile_definitions(test_raytracer PUBLIC PROGRAM_DIR="${CMAKE_CURRENT_SOURCE_DIR}/raytracer/")
    catch_discover_tests(test_raytracer)
else()
    message(STATUS "raytracer disabled. PNG:${PNG_FOUND} JPEG:${JPEG_FOUND}")
endif()
