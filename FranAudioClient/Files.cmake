# FranticDreamer 2022-2024

# ---
# FranAudio Client Library Files
# ---

# Header files
SET(GLOB FRANAUDIOCLIENT_HEADERFILES

	#Shared
	FranAudioShared/FranAudioShared.hpp

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

