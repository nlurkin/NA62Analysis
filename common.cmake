MACRO(SUBDIRLIST dir result)
	FILE(GLOB children ${dir}/*)
	SET(dirlist "")
	FOREACH(child ${children})
		IF(IS_DIRECTORY ${child})
			SET(dirlist ${dirlist} ${child})
		ENDIF()
	ENDFOREACH(child)
	SET(${result} ${dirlist})
ENDMACRO()