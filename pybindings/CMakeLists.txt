cmake_minimum_required(VERSION 3.0)

find_package(PythonInterp)
find_package(Boost REQUIRED COMPONENTS python3)
find_package(PythonInterp 3)
find_package(PythonLibs 3 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})

# Add parent MatsuokaEngine cpp/c
file(GLOB PARENT_C "${CMAKE_CURRENT_SOURCE_DIR}/../*.c")

LINK_LIBRARIES(${Boost_LIBRARIES} ${PYTHON_LIBRARIES} ${PARENT_C})

message(STATUS "PYTHON_LIBRARIES = ${PYTHON_LIBRARIES}")
message(STATUS "PYTHON_EXECUTABLE = ${PYTHON_EXECUTABLE}")
message(STATUS "PYTHON_INCLUDE_DIRS = ${PYTHON_INCLUDE_DIRS}")
message(STATUS "BOOST_LIBRARIES = ${Boost_LIBRARIES}")
message(STATUS "MATS_LIBRARIES = ${PARENT_C}")

PYTHON_ADD_MODULE(cpglib cpglib.cpp)
