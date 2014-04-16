
include_directories(include)

file(GLOB LIBFILES
	"include/*.hh"
	"src/*.cc"
)

add_library(ToolsLib SHARED ${LIBFILES})