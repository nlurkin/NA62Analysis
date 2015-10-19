
include_directories(include)

file(GLOB LIBFILES
	"include/*.hh"
	"src/*.cc"
)

EXCLUDEEMACS(LIBFILES)

add_library(ToolsLib${LIBTYPEPOSTFIX} ${LIBTYPE} ${LIBFILES})