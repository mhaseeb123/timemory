cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

project(timemory-Python-Example LANGUAGES C CXX)

if("${CMAKE_PROJECT_NAME}" STREQUAL "timemory")
    if(NOT TIMEMORY_USE_PYTHON)
        return()
    endif()
endif()

if(NOT PYTHON_EXECUTABLE)
    set(PYTHON_EXECUTABLE "/usr/bin/env python")
endif()

configure_file(${PROJECT_SOURCE_DIR}/ex_sample.py
    ${CMAKE_BINARY_DIR}/ex_python_sample @ONLY)

configure_file(${PROJECT_SOURCE_DIR}/ex_profiler.py
    ${CMAKE_BINARY_DIR}/ex_python_profiler @ONLY)
  
install(
    FILES
        ${CMAKE_BINARY_DIR}/ex_python_sample
        ${CMAKE_BINARY_DIR}/ex_python_profiler
    DESTINATION bin
    PERMISSIONS
        OWNER_EXECUTE OWNER_READ OWNER_WRITE
        GROUP_EXECUTE GROUP_READ
        WORLD_EXECUTE WORLD_READ)

if(NOT TARGET pybind11::module)
    find_package(pybind11 QUIET)
endif()

if(NOT TARGET pybind11::module)
    return()
endif()

find_package(MPI QUIET)
if(NOT MPI_FOUND)
    return()
endif()

set(TIMEMORY_INTERFACE_LIBRARY timemory-ex-python-bindings)
find_package(timemory COMPONENTS headers OPTIONAL_COMPONENTS mpi cxx shared)

pybind11_add_module(libex_python_bindings ${PROJECT_SOURCE_DIR}/libex_bindings.cpp)

target_compile_definitions(libex_python_bindings PRIVATE USE_MPI)
target_link_libraries(libex_python_bindings PRIVATE
    MPI::MPI_C MPI::MPI_CXX)

# target_link_libraries(libex_python_bindings INTERFACE timemory-ex-python-bindings)

configure_file(${PROJECT_SOURCE_DIR}/ex_bindings.py
    ${CMAKE_BINARY_DIR}/ex_python_bindings @ONLY)