cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(NA62AnalysisUser)

# Get NA62Analysis
set(NA62ANALYSIS $ENV{ANALYSISFW_PATH})
include(${NA62ANALYSIS}/NA62Analysis.cmake)

# Define NA62Analysis/NA62MC include and lib directories  
include_directories(Analyzers/include)
include_directories(${NA62ANALYSIS_ANALYZER_INCLUDES})
link_directories(${NA62ANALYSIS}/lib)
link_directories(${NA62MC_LIB_DIRS})

# Include POs
add_subdirectory(PhysicsObjects)

# Get analyzers definition from builder
include(analyzers.cmake)

# Create libraries for user analyzers
FOREACH(usrAna ${USER_ANALYZERS})
	add_library(${usrAna} SHARED Analyzers/src/${usrAna}.cc Analyzers/include/${usrAna}.hh)
ENDFOREACH(usrAna)

# Create executable from builder
add_executable(${TARGET_EXEC} main.cc)

# Specify all analyzers libraries
target_link_libraries(${TARGET_EXEC} ${ANA_LIBS})

# Specify NA62MC/NA62Analysis/ROOT libraries
target_link_libraries(${TARGET_EXEC} AnalysisFW)
target_link_libraries(${TARGET_EXEC} PhysicsLib)
target_link_libraries(${TARGET_EXEC} ${ROOT_LIBRARIES} Minuit Geom TMVA EG)
target_link_libraries(${TARGET_EXEC} ${NA62MC_LIBS})

# Move target to user dir
install(TARGETS ${TARGET_EXEC} DESTINATION bin/..)