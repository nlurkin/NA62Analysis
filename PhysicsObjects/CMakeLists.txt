
include_directories(include)

file(GLOB EXHH "include/*.hh")

FOREACH (lib ${EXHH})
	GET_FILENAME_COMPONENT(libName ${lib} NAME_WE)
	ROOT_GENERATE_DICTIONARY(${libName}.hh LinkDef.h ${libName}DICT.cc "${ROOT_INCLUDE_DIR};${CMAKE_CURRENT_SOURCE_DIR};${NA62MC_INC_DIRS}")
	add_library(${libName} SHARED include/${libName}.hh src/${libName}.cc)
ENDFOREACH(lib)
