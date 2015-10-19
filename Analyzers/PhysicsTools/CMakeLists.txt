
include_directories(include)

file(GLOB EXHH "include/*.hh")

EXCLUDEEMACS(EXHH)

FOREACH (lib ${EXHH})
	GET_FILENAME_COMPONENT(libName ${lib} NAME_WE)
	add_library(l${libName}${LIBTYPEPOSTFIX} ${LIBTYPE} include/${libName}.hh src/${libName}.cc)
ENDFOREACH(lib)
