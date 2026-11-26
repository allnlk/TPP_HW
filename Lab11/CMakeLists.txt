cmake_minimum_required(VERSION 3.21)
project(FunctionalCpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Кожна частина лабораторної роботи - окрема програма
add_executable(part1_max            src/part1_max.cpp)
add_executable(part2_is_in_range    src/part2_is_in_range.cpp)
add_executable(part3_remove_center  src/part3_remove_center.cpp)
add_executable(part4_pipeline       src/part4_pipeline.cpp)
add_executable(part5_1_remove_if    src/part5_1_remove_if.cpp)
add_executable(part5_2_capture      src/part5_2_capture.cpp)
add_executable(part5_3_dangling     src/part5_3_dangling.cpp)
add_executable(part5_4_average      src/part5_4_average.cpp)

# Початкові фрагменти частини 5 (5.3 і 5.4 навмисно не компілюються)
# збираються вручну, див. README.md
