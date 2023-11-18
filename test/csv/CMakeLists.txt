cmake_minimum_required(VERSION 3.14...3.26)

project(driver_csv)

add_library(driver_csv SHARED csv.hpp csv.cpp)
target_compile_features(driver_csv PUBLIC cxx_std_20)
target_link_libraries(driver_csv PUBLIC dynio)
