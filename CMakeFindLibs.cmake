# Find Vorbis and Opus
# This module finds the Vorbis and Opus libraries and includes them in the build.
# It sets the following variables:
#   FRANAUDIO_CAN_USE_VORBIS - ON if Vorbis is found
#   FRANAUDIO_CAN_USE_OPUS - ON if Opus is found
#
# Note: If vorbis is not found then opus upport will be ignored.


macro (find_vorbis_and_opus)
	set (FRANAUDIO_CAN_USE_VORBIS OFF)
	set (FRANAUDIO_CAN_USE_OPUS OFF)

	if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Libs/ogg/CMakeLists.txt)
		message(STATUS "Ogg Detected. Checking for Vorbis...")
		
		if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Libs/vorbis/CMakeLists.txt)
			message(STATUS "Vorbis Detected. Enabling Vorbis for backends that supports it.")
			set(FRANAUDIO_CAN_USE_VORBIS ON)

			if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Libs/opus/CMakeLists.txt)
				message(STATUS "Opus Detected. Enabling Opus for backends that supports it.")
				set(FRANAUDIO_CAN_USE_OPUS ON)
			else()
				message(STATUS "Opus not found. Opus disabled.")
				set(FRANAUDIO_CAN_USE_OPUS OFF)
			endif()

		else()
			message(STATUS "Vorbis not found. Vorbis and Opus are disabled.")
		endif()

	else()
		message(STATUS "Ogg not found. Vorbis and Opus are disabled.")
	endif()
endmacro()
