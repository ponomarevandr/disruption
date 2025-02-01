cmake_minimum_required(VERSION 3.14)

project(ElectricalBreakdown)

set(SOURCES
	sources/main.cpp
	sources/model/model.cpp
	sources/model/model_parameters.cpp
	sources/model/time_step_manager.cpp
	../common_sources/display/progress_bar.cpp
	../common_sources/evaluation/formula.cpp
	../common_sources/evaluation/environment.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
include_directories(${PROJECT_SOURCE_DIR}/sources ${PROJECT_SOURCE_DIR}/../common_sources)

add_executable(tuning ${SOURCES})
target_link_libraries(tuning matheval)