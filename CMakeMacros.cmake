# List of subdirectories
macro (subdirlist result curdir)
	file (GLOB children ${curdir}/*) # This was changed
	set (dirlist "")
	foreach (child ${children})
		if (IS_DIRECTORY ${child}) # This was changed
			list (APPEND dirlist ${child})
		endif()
	endforeach()
	set (${result} ${dirlist})
endmacro()


# Export files
macro (COPY_FILES OUTPUT_TARGET COPY_FILE_LIST_VAR OUTPUT_DIR)
	message (STATUS "=========================================")
	message (STATUS "Copying files for ${OUTPUT_TARGET}")

	foreach (header ${${COPY_FILE_LIST_VAR}})
		# Compute RELATIVE path
		file (RELATIVE_PATH relpath
			${CMAKE_CURRENT_SOURCE_DIR}
			${header}  
		)
		get_filename_component (dir ${header} DIRECTORY)
		get_filename_component (relpathdir ${relpath} DIRECTORY)

		add_custom_command (TARGET ${OUTPUT_TARGET} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_DIR}/${relpathdir}
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
					${header}
					${OUTPUT_DIR}/${relpath}
		)

		message (STATUS "Copying file from: ${header} to ${OUTPUT_DIR}/${relpath}")
	endforeach()

	message (STATUS "Files copied for ${OUTPUT_TARGET}")
	message (STATUS "=========================================")
endmacro()
