set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/.. )

include(${NA62ANALYSIS}/common.cmake)

#require ROOT
include(${NA62ANALYSIS}/FindROOT.cmake)
include_directories(${ROOT_INCLUDE_DIR})
link_directories(${ROOT_LIBRARY_DIR})

if(CMAKE_COMPILER_IS_GNUCXX)
#	set(CMAKE_CXX_FLAGS "-std=c++0x -pedantic-errors -Wall -Wextra -Wwrite-strings -Woverloaded-virtual -fno-nonansi-builtins -fno-gnu-keywords -fstrict-aliasing")
	set(CMAKE_CXX_FLAGS "-std=c++0x -Wall -Wextra")
endif()

#Get NA62MC
set(NA62MCSOURCE $ENV{NA62MCSOURCE})
set(SUBDIRS RICH LKr Spectrometer GigaTracker LAV IRC CHANTI Cedar CHOD MUV1 MUV2 SAC MUV3 MUV0 HAC)
set(NA62MC_LIB_DIRS ${NA62MCSOURCE}/Persistency/lib)
set(NA62MC_LIBS NA62Persistency)
FOREACH (DIR ${SUBDIRS})
	include_directories(${NA62MCSOURCE}/${DIR}/Persistency/include)
	set(NA62MC_LIB_DIRS ${NA62MC_LIB_DIRS} ${NA62MCSOURCE}/${DIR}/Persistency/lib)
	set(NA62MC_LIBS ${NA62MC_LIBS} ${DIR}Persistency)
ENDFOREACH(DIR)
include_directories(${NA62MCSOURCE}/Persistency/include)

#FW include directories
include_directories(${NA62ANALYSIS}/include)
include_directories(${NA62ANALYSIS}/PhysicsLib/include)

SUBDIRLIST(${NA62ANALYSIS}/Analyzers NA62ANALYSIS_ANALYZER_DIRS)
SET(NA62ANALYSIS_ANALYZER_INCLUDES "")
FOREACH(dir ${NA62ANALYSIS_ANALYZER_DIRS})
	SET(NA62ANALYSIS_ANALYZER_INCLUDES ${NA62ANALYSIS_ANALYZER_INCLUDES} ${dir}/include)
ENDFOREACH(dir)
