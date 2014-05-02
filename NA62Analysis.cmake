set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/.. )

include(${NA62ANALYSIS}/common.cmake)

#require ROOT
include(${NA62ANALYSIS}/FindROOT.cmake)
include_directories(${ROOT_INCLUDE_DIR})
link_directories(${ROOT_LIBRARY_DIR})

if(CMAKE_COMPILER_IS_GNUCXX)
	if(FULL_WARNING)
		message("Using Flag: FULL_WARNING")
		set(CMAKE_CXX_FLAGS "-pedantic-errors -Wall -Wextra -Wwrite-strings -Woverloaded-virtual -fno-nonansi-builtins -fno-gnu-keywords -fstrict-aliasing -Wno-long-long")
	else()
		set(CMAKE_CXX_FLAGS "-Wall -Wno-long-long")
	endif()
	
	if(NA62_DEBUG)
		message("Using Flag: NA62_DEBUG")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
	endif()
	
	if(C++11_COMPAT)
		message("Using Flag: C++11_COMPAT")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -DNA62_C11=1")
	endif()
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
include_directories(${NA62ANALYSIS}/Examples/include)

SUBDIRLIST(${NA62ANALYSIS}/Analyzers NA62ANALYSIS_ANALYZER_DIRS)
SET(NA62ANALYSIS_ANALYZER_INCLUDES "")
FOREACH(dir ${NA62ANALYSIS_ANALYZER_DIRS})
	SET(NA62ANALYSIS_ANALYZER_INCLUDES ${NA62ANALYSIS_ANALYZER_INCLUDES} ${dir}/include)
ENDFOREACH(dir)
