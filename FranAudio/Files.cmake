# FranticDreamer 2022-2024

# ---
# FranAudio Files
# ---

# Header files
SET(GLOB FRANAUDIO_HEADERFILES

	#C API (Compiled as C++)
	FranAudio/FranAudioAPILegacy.h

	#Shared API Defines
	FranAudio/FranAudioAPI.hpp

	#Main
	FranAudio/FranAudio.hpp
	
	#Logger
	FranAudio/Logger/Logger.hpp

	#Sound
	FranAudio/Sound/Sound.hpp

	#WaveData
	FranAudio/Sound/WaveData/WaveData.hpp

	#Backend
	FranAudio/Backend/Backend.hpp
	FranAudio/Backend/miniaudio/Backend_miniaudio.hpp
	#FranAudio/Backend/OpenALSoft/OpenALSoft.hpp

	#Decoder
	FranAudio/Decoder/Decoder.hpp
	FranAudio/Decoder/miniaudio/Decoder_miniaudio.hpp
	FranAudio/Decoder/libnyquist/Decoder_libnyquist.hpp
	
	)

# Source files
FILE(GLOB FRANAUDIO_SOURCEFILES

	#C API (Compiled as C++)
	FranAudio/FranAudioAPILegacy.cpp

	#Shared API Defines
	#FranAudio/FranAudioAPI.cpp

	#Main
	FranAudio/FranAudio.cpp

	#Logger
	FranAudio/Logger/Logger.cpp

	#Sound
	FranAudio/Sound/Sound.cpp

	#WaveData
	FranAudio/Sound/WaveData/WaveData.cpp

	#Backend
	FranAudio/Backend/Backend.cpp
	FranAudio/Backend/miniaudio/Backend_miniaudio.cpp
	#FranAudio/Backend/OpenALSoft/OpenALSoft.cpp

	#Decoder
	FranAudio/Decoder/Decoder.cpp
	FranAudio/Decoder/miniaudio/Decoder_miniaudio.cpp
	FranAudio/Decoder/libnyquist/Decoder_libnyquist.cpp

	)

include_directories("FranAudio")