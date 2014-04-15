cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(NA62Analysis)
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib )

#require ROOT
include(FindROOT.cmake)
include_directories(${ROOT_INCLUDE_DIR})
link_directories(%{ROOT_LIBRARY_DIR})

if(CMAKE_COMPILER_IS_GNUCXX)
#	set(CMAKE_CXX_FLAGS "-std=c++0x -pedantic-errors -Wall -Wextra -Wwrite-strings -Woverloaded-virtual -fno-nonansi-builtins -fno-gnu-keywords -fstrict-aliasing")
	set(CMAKE_CXX_FLAGS "-std=c++0x -Wall -Wextra")
endif()

#Get NA62MC
set(NA62MCSOURCE $ENV{NA62MCSOURCE})
set(SUBDIRS RICH LKr Spectrometer GigaTracker LAV IRC CHANTI Cedar CHOD MUV1 MUV2 SAC MUV3 MUV0 HAC)
FOREACH (DIR ${SUBDIRS})
	include_directories(${NA62MCSOURCE}/${DIR}/Persistency/include)
ENDFOREACH(DIR)
include_directories(${NA62MCSOURCE}/Persistency/include)

#FW include directories
include_directories(include)

#Get all FW source files
file(GLOB MAINO
	"include/*.hh"
	"src/*.cc"
)

add_library(AnalysisFW SHARED ${MAINO})

add_subdirectory(Examples)
add_subdirectory(Analyzers)
