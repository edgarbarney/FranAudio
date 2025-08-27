# FranticDreamer 2022-2025

# ---
# FranAudio Client Library Files
# ---

# Header files
FILE(GLOB FRANAUDIOCLIENT_HEADERFILES

	#Main
	FranAudioClient/FranAudioClient.hpp

	#API
	FranAudioClient/FranAudioClientAPI.hpp

	)

# Source files
FILE(GLOB FRANAUDIOCLIENT_SOURCEFILES

	#Main
	FranAudioClient/FranAudioClient.cpp
	FranAudioClient/FranAudioClient_Win32.cpp
	FranAudioClient/FranAudioClient_Linux.cpp
	)

#include_directories("FranAudioClient")

