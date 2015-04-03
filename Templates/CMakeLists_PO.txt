
include_directories(include)
include_directories(.)
include_directories(${EXTRA_INCLUDE_DIRS})

file(GLOB EXHH "include/*.hh")

SET (USERPOLIBS "")
FOREACH (lib ${EXHH})
	GET_FILENAME_COMPONENT(libName ${lib} NAME_WE)
	IF (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/include/LinkDef_${libName}.h)
		ROOT_GENERATE_DICTIONARY(include/${libName}.hh include/LinkDef_${libName}.h ${libName}DICT.cc "${ROOT_INCLUDE_DIR};${CMAKE_CURRENT_SOURCE_DIR};${NA62MC_INC_DIRS}")
		add_library(${libName}${LIBTYPEPOSTFIX} ${LIBTYPE} include/${libName}.hh src/${libName}.cc ${libName}DICT.cc)
	ELSE()
		add_library(${libName}${LIBTYPEPOSTFIX} ${LIBTYPE} include/${libName}.hh src/${libName}.cc)
	ENDIF()
	SET (USERPOLIBS ${USERPOLIBS} ${libName}${LIBTYPEPOSTFIX})
ENDFOREACH(lib)

SET (USERPOLIBS ${USERPOLIBS} PARENT_SCOPE)
