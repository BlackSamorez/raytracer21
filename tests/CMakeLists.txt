include_directories(.)

add_executable(test_geometry geometry/test_geometry.cpp)
add_test(NAME test_geometry COMMAND test_geometry)

add_executable(test_reader reader/test_reader.cpp)