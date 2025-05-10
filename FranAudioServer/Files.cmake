# FranticDreamer 2022-2024

# ---
# FranAudio Server Program Files
# ---

# Header files
SET(GLOB FRANAUDIOSERVER_HEADERFILES

	#Main
	FranAudioServer/FranAudioServer.hpp
	)

# Source files
FILE(GLOB FRANAUDIOSERVER_SOURCEFILES

	#Main
	FranAudioServer/FranAudioServer.cpp
	FranAudioServer/FranAudioServer_Win32.cpp
	FranAudioServer/FranAudioServer_Linux.cpp
	)

#include_directories("FranAudioServer")

