
include_directories(include)
include_directories(TRecoLKrCandidateMC)
add_subdirectory(TRecoLKrCandidateMC)

file(GLOB EXHH "include/*.hh")

FOREACH (lib ${EXHH})
	GET_FILENAME_COMPONENT(libName ${lib} NAME_WE)
	add_library(l${libName}${LIBTYPEPOSTFIX} ${LIBTYPE} include/${libName}.hh src/${libName}.cc)
ENDFOREACH(lib)
