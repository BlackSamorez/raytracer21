enable_testing()
add_executable(test_geometry test_geometry.cpp)
target_link_libraries(test_geometry PRIVATE Catch2::Catch2)
catch_discover_tests(test_geometry)