cmake_minimum_required(VERSION 3.14)

project(GasKinetics)

set(SOURCES
	sources/main.cpp
	sources/model/model.cpp
	sources/model/model_parameters.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
include_directories(${PROJECT_SOURCE_DIR}/sources)

add_executable(one_dim ${SOURCES})