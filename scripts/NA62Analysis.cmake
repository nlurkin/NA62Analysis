set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/.. )

if(SHARED_LIB)
    set( LIBTYPE SHARED )
else()
    set( LIBTYPE STATIC )
endif()

if(${LIBTYPE} STREQUAL STATIC)
    message("-- Using static libraries")
	set( LIBTYPEPOSTFIX "-static")
	set( EXTLIBTYPEPOSTFIX "-static")
else()
    message("-- Using shared libraries")
	set( LIBTYPEPOSTFIX "")
	set( EXTLIBTYPEPOSTFIX "-static")
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
find_package(NA62MC)
# It defines the following variables
# Paths
#  NA62MC_PERS_INCLUDE_DIRS - include directories for NA62MC Persistency
#  NA62MC_CONFIG_DIR        - configuration files path (cmake, conf, ...)
#  NA62MC_PERS_LIBRARY_DIRS - library directories for NA62MC Persistency
# Lists
#  NA62MC_PERS_LIBRARIES    - persistency libraries to link against

#set(SUBDIRS RICH LKr Spectrometer GigaTracker LAV IRC CHANTI Cedar CHOD MUV1 MUV2 SAC MUV3 MUV0 HAC)

IF(DEFINED ENV{NA62RECOSOURCE})
	set(NA62RECOSOURCE $ENV{NA62RECOSOURCE})
	set(NA62RECO_LIB_DIRS ${NA62RECOSOURCE}/RecoBase/lib ${NA62RECOSOURCE}/Service/lib ${NA62RECOSOURCE}/EventDisplay/lib ${NA62RECOSOURCE}/lib)
	set(NA62RECO_LIBS "")

    FOREACH (DIR ${SUBDIRS})
		set(NA62RECO_LIB_DIRS ${NA62RECO_LIB_DIRS} ${NA62RECOSOURCE}/${DIR}/lib)
		set(NA62RECO_LIBS ${NA62RECO_LIBS} ${DIR}Reconstruction${EXTLIBTYPEPOSTFIX})
    ENDFOREACH(DIR)
    
    set(NA62RECO_LIBS ${NA62RECO_LIBS} RecoBase${EXTLIBTYPEPOSTFIX} Service${EXTLIBTYPEPOSTFIX} EventDisplay${EXTLIBTYPEPOSTFIX} NA62Reconstruction${EXTLIBTYPEPOSTFIX})
ENDIF()

include_directories(${NA62MC_PERS_INCLUDE_DIRS})

#FW include directories
include_directories(${NA62ANALYSIS}/include)
include_directories(${NA62ANALYSIS}/ToolsLib/include)
include_directories(${NA62ANALYSIS}/Examples/include)

SUBDIRLIST(${NA62ANALYSIS}/Analyzers NA62ANALYSIS_ANALYZER_DIRS)
SET(NA62ANALYSIS_ANALYZER_INCLUDES "")
FOREACH(dir ${NA62ANALYSIS_ANALYZER_DIRS})
	SET(NA62ANALYSIS_ANALYZER_INCLUDES ${NA62ANALYSIS_ANALYZER_INCLUDES} ${dir}/include)
ENDFOREACH(dir)
