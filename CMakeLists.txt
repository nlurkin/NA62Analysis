cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(NA62Analysis)

set(NA62ANALYSIS ${PROJECT_SOURCE_DIR})
set(NA62FW 1)

include(scripts/NA62Analysis.cmake)

#Get all FW source files
file(GLOB MAINO
	"include/*.hh"
	"src/*.cc"
)

add_library(AnalysisFW SHARED ${MAINO})

add_subdirectory(Examples)
add_subdirectory(Analyzers)
add_subdirectory(PhysicsObjects)
add_subdirectory(PhysicsLib)