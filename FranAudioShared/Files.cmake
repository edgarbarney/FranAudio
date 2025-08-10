# FranticDreamer 2022-2024

# ---
# FranAudio Shared Files
# ---

# Header files
SET(GLOB FRANAUDIOSHARED_HEADERFILES

	#Network
	FranAudioShared/Network.hpp

	#Logger
	FranAudioShared/Logger/Logger.hpp
	)

# Source files
FILE(GLOB FRANAUDIOSHARED_SOURCEFILES

	#Logger
	FranAudioShared/Logger/Logger.cpp
	)

#include_directories("FranAudioShared")

