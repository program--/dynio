cmake_minimum_required(VERSION 3.14...3.26)

project(dynio VERSION 0.1.0 LANGUAGES C CXX)

add_library(dynio INTERFACE)
target_include_directories(dynio INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(dynio INTERFACE ${CMAKE_DL_LIBS})

include(CTest)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/test)
