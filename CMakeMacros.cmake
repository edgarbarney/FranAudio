#List of subdirectories
macro(subdirlist result curdir)
	file(GLOB children ${curdir}/*) # This was changed
	set(dirlist "")
	foreach(child ${children})
	    if(IS_DIRECTORY ${child}) # This was changed
	        list(APPEND dirlist ${child})
	    endif()
	endforeach()
	set(${result} ${dirlist})
endmacro()
