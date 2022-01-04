add_executable(test_reader test_reader.cpp)
target_link_libraries(test_reader PRIVATE Catch2::Catch2)
target_compile_definitions(test_reader PUBLIC PROGRAM_DIR="${CMAKE_CURRENT_SOURCE_DIR}/")
catch_discover_tests(test_reader)