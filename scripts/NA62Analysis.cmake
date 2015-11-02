set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/.. )

set( LIBTYPE STATIC )
if(${LIBTYPE} STREQUAL STATIC)
	set( LIBTYPEPOSTFIX "-static")
else()
	set( LIBTYPEPOSTFIX "")
endif()

include(${NA62ANALYSIS}/scripts/common.cmake)

#require ROOT
include(${NA62ANALYSIS}/scripts/FindROOT.cmake)
#include($ENV{ROOTSYS}/etc/cmake/FindROOT.cmake)
include_directories(${ROOT_INCLUDE_DIR})
link_directories(${ROOT_LIBRARY_DIR})

# Use correct gcc libraries
link_directories($ENV{ANALYSISFW_STDLIBSPATH}/lib64)
link_directories($ENV{ANALYSISFW_STDLIBSPATH}/lib)

if(CMAKE_COMPILER_IS_GNUCXX)
	#Get GCC/G++ version to use correct c++11 flag
	GETGCCVERSION(maj min)
	if( "${maj}.${min}" VERSION_LESS "4.7" )
		SET(C++11_FLAG "-std=c++0x")
	else()
		SET(C++11_FLAG "-std=c++11")
	endif()
	
	# Test c++11 unordered_map feature availability
	set(CMAKE_CXX_FLAGS ${C++11_FLAG})
	if(${NA62USE_USER})
		try_compile(TEST_UN_MAP $ENV{ANALYSISFW_USERDIR}/build/test ${NA62ANALYSIS}/scripts/test_unordered_map.cpp)
	else ()
		try_compile(TEST_UN_MAP ${NA62ANALYSIS}/build/test ${NA62ANALYSIS}/scripts/test_unordered_map.cpp)
	endif()
	if(TEST_UN_MAP)
		message ("-- Testing C++11 compatibility -- works")
	else()
		message ("-- Testing C++11 compatibility -- failed")
	endif()
	
	
	set(CMAKE_CXX_FLAGS "")
	# Choose warning flags
	if(FULL_WARNING)
		message("-- Using Flag: FULL_WARNING")
		set(CMAKE_CXX_FLAGS "-pedantic-errors -Wall -Wextra -Wwrite-strings -Woverloaded-virtual -fno-nonansi-builtins -fno-gnu-keywords -fstrict-aliasing -Wno-long-long")
	else()
		set(CMAKE_CXX_FLAGS "-Wall -Wno-long-long")
	endif()
	
	# Choose debug flags
	if(NA62_DEBUG)
		message("-- Using Flag: NA62_DEBUG")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
	endif()
	
	# Choose c++11 flag
	if(C++11_COMPAT AND TEST_UN_MAP)
		message("-- Using Flag: C++11_COMPAT")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${C++11_FLAG} -DNA62_C11=1")
	endif()

endif()

#Get NA62MC and NA62RECO
set(SUBDIRS RICH LKr Spectrometer GigaTracker LAV IRC CHANTI Cedar CHOD NewCHOD MUV1 MUV2 SAC MUV3 MUV0 HAC)

set(NA62MCSOURCE $ENV{NA62MCSOURCE})
set(NA62MC_INC_DIRS ${NA62MCSOURCE}/Persistency/include)
set(NA62MC_LIB_DIRS ${NA62MCSOURCE}/Persistency/lib)
set(NA62MC_LIBS "")

IF(DEFINED ENV{NA62RECOSOURCE})
	set(NA62RECOSOURCE $ENV{NA62RECOSOURCE})
	set(NA62RECO_LIB_DIRS ${NA62RECOSOURCE}/RecoBase/lib ${NA62RECOSOURCE}/Service/lib ${NA62RECOSOURCE}/EventDisplay/lib ${NA62RECOSOURCE}/lib)
	set(NA62RECO_LIBS "")
ENDIF()

FOREACH (DIR ${SUBDIRS})
	include_directories(${NA62MCSOURCE}/${DIR}/Persistency/include)
	set(NA62MC_INC_DIRS ${NA62MC_INC_DIRS} ${NA62MCSOURCE}/${DIR}/Persistency/include)
	set(NA62MC_LIB_DIRS ${NA62MC_LIB_DIRS} ${NA62MCSOURCE}/${DIR}/Persistency/lib)
	set(NA62MC_LIBS ${NA62MC_LIBS} ${DIR}Persistency${LIBTYPEPOSTFIX})
	
	IF(DEFINED ENV{NA62RECOSOURCE})
		set(NA62RECO_LIB_DIRS ${NA62RECO_LIB_DIRS} ${NA62RECOSOURCE}/${DIR}/lib)
		set(NA62RECO_LIBS ${NA62RECO_LIBS} ${DIR}Reconstruction${LIBTYPEPOSTFIX})
	ENDIF()
ENDFOREACH(DIR)
set(NA62MC_LIBS ${NA62MC_LIBS} NA62Persistency${LIBTYPEPOSTFIX})
IF(DEFINED ENV{NA62RECOSOURCE})
	set(NA62RECO_LIBS ${NA62RECO_LIBS} RecoBase${LIBTYPEPOSTFIX} Service${LIBTYPEPOSTFIX} EventDisplay${LIBTYPEPOSTFIX} NA62Reconstruction${LIBTYPEPOSTFIX})
ENDIF()
include_directories(${NA62MCSOURCE}/Persistency/include)

#FW include directories
include_directories(${NA62ANALYSIS}/include)
include_directories(${NA62ANALYSIS}/ToolsLib/include)
include_directories(${NA62ANALYSIS}/Examples/include)

SUBDIRLIST(${NA62ANALYSIS}/Analyzers NA62ANALYSIS_ANALYZER_DIRS)
SET(NA62ANALYSIS_ANALYZER_INCLUDES "")
FOREACH(dir ${NA62ANALYSIS_ANALYZER_DIRS})
	SET(NA62ANALYSIS_ANALYZER_INCLUDES ${NA62ANALYSIS_ANALYZER_INCLUDES} ${dir}/include)
ENDFOREACH(dir)
