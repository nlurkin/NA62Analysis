
include_directories(include)

file(GLOB EXHH "include/*.hh")

FOREACH (lib ${EXHH})
	GET_FILENAME_COMPONENT(libName ${lib} NAME_WE)
	IF( NOT ${libName} STREQUAL "toto")
		add_library(${libName} SHARED include/${libName}.hh src/${libName}.cc)
	ENDIF( NOT ${libName} STREQUAL "toto")
ENDFOREACH(lib)
