cmake_minimum_required(VERSION 3.0)

find_package(PythonInterp)
find_package(Boost REQUIRED COMPONENTS python3)
find_package(PythonInterp 3)
find_package(PythonLibs 3 REQUIRED)
message(STATUS "PYTHON_LIBRARIES = ${PYTHON_LIBRARIES}")
message(STATUS "PYTHON_EXECUTABLE = ${PYTHON_EXECUTABLE}")
message(STATUS "PYTHON_INCLUDE_DIRS = ${PYTHON_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARIES = ${Boost_LIBRARIES}")
include_directories(${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})
LINK_LIBRARIES(${Boost_LIBRARIES} ${PYTHON_LIBRARIES})

PYTHON_ADD_MODULE(cpglib cpglib.cpp)
