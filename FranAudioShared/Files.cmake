# FranticDreamer 2022-2025

# ---
# FranAudio Shared Files
# ---

# Header files
FILE(GLOB FRANAUDIOSHARED_HEADERFILES

	#Core
	FranAudioShared/FranAudioShared.hpp

	#Network
	FranAudioShared/Network/Network.hpp

	#Logger
	FranAudioShared/Logger/Logger.hpp

	#Containers
	FranAudioShared/Containers/UnorderedMap.hpp
	FranAudioShared/Containers/Vector.hpp
	)

# Source files
FILE(GLOB FRANAUDIOSHARED_SOURCEFILES

	#Logger
	FranAudioShared/Logger/Logger.cpp
	)

#include_directories("FranAudioShared")

