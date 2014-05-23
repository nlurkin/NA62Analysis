
include_directories(include)

file(GLOB LIBFILES
	"include/*.hh"
	"src/*.cc"
)

add_library(ToolsLib${LIBTYPEPOSTFIX} ${LIBTYPE} ${LIBFILES})