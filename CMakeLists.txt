cmake_minimum_required(VERSION 3.14...3.26)

project(dynio VERSION 0.1.0 LANGUAGES C CXX)

add_library(dynio_c
    "${CMAKE_CURRENT_LIST_DIR}/include/dynio/driver.h")
add_library(dynio::c ALIAS dynio_c)
target_compile_options(dynio_c PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>)
set_target_properties(dynio_c
    PROPERTIES
        LINKER_LANGUAGE C
        C_STANDARD 90
        C_EXTENSIONS OFF)

add_library(dynio_cxx
    "${CMAKE_CURRENT_LIST_DIR}/include/dynio/driver.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/include/dynio/loader.hpp")
add_library(dynio::cxx ALIAS dynio_c)
target_compile_options(dynio_cxx PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>)
set_target_properties(dynio_cxx
    PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_STANDARD 11
        C_EXTENSIONS OFF)

add_library(dynio INTERFACE)
target_include_directories(dynio INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(dynio INTERFACE ${CMAKE_DL_LIBS} dynio::c dynio::cxx)

if("${CMAKE_PROJECT_NAME}" STREQUAL "${PROJECT_NAME}")
    include(CTest)
    if(${BUILD_TESTING})
        add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/test)
    endif()
endif()
