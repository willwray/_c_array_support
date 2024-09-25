cmake_minimum_required(VERSION 3.23)

project(c_array_supportTests LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(c_array_support REQUIRED)
  enable_testing()
endif()

# ---- Tests ----

add_executable(test_c_array_support test_c_array_support.cpp)
target_link_libraries(test_c_array_support PRIVATE c_array::support)
target_compile_features(test_c_array_support PRIVATE cxx_std_20)
add_test(NAME test_c_array_support COMMAND test_c_array_support)

add_executable(test_c_array_compare test_c_array_compare.cpp)
target_link_libraries(test_c_array_compare PRIVATE c_array::support)
target_compile_features(test_c_array_compare PRIVATE cxx_std_20)
add_test(NAME test_c_array_compare COMMAND test_c_array_compare)

add_executable(test_c_array_assign test_c_array_assign.cpp)
target_link_libraries(test_c_array_assign PRIVATE c_array::support)
target_compile_features(test_c_array_assign PRIVATE cxx_std_20)
add_test(NAME test_c_array_assign COMMAND test_c_array_assign)

# ---- End-of-file commands ----

