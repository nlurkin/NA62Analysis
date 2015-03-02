MACRO(SUBDIRLIST dir result)
	FILE(GLOB children ${dir}/*)
	FILE(GLOB svn ${dir}/.svn)
	LIST(REMOVE_ITEM children ${svn})
	SET(dirlist "")
	FOREACH(child ${children})
		IF(IS_DIRECTORY ${child})
			SET(dirlist ${dirlist} ${child})
		ENDIF()
	ENDFOREACH(child)
	SET(${result} ${dirlist})
ENDMACRO()


MACRO(GETGCCVERSION major minor)
    execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
                    OUTPUT_VARIABLE GCC_VERSION)
    string(REGEX MATCHALL "[0-9]+" GCC_VERSION_COMPONENTS ${GCC_VERSION})
    list(GET GCC_VERSION_COMPONENTS 0 GCC_MAJOR)
    list(GET GCC_VERSION_COMPONENTS 1 GCC_MINOR)

	SET(${major} ${GCC_MAJOR})
	SET(${minor} ${GCC_MINOR})
ENDMACRO()