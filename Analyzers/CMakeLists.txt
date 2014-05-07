include(../scripts/common.cmake)

SUBDIRLIST(. dirs)

FOREACH (dir ${dirs})
	add_subdirectory(${dir})
ENDFOREACH(dir)
