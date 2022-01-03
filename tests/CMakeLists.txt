include_directories(.)

Include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.0.0-preview3
)

FetchContent_MakeAvailable(Catch2)
option(TEST_SOLUTION "Build solution" OFF)
option(ENABLE_PRIVATE_TESTS "Enable private tests" OFF)
option(GRADER "Building on CI" OFF)


set(CMAKE_CXX_COMPILER "/usr/bin/g++-10")

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)

add_executable(test_geometry geometry/test_geometry.cpp)
target_link_libraries(test_geometry PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(test_geometry)

add_executable(test_reader reader/test_reader.cpp)
target_link_libraries(test_reader PRIVATE Catch2::Catch2WithMain)
target_compile_definitions(test_reader PUBLIC PROGRAM_DIR="${CMAKE_CURRENT_SOURCE_DIR}/reader/")
catch_discover_tests(test_reader)

