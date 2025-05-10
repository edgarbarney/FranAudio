// FranticDreamer 2022-2024

#include "Sound.hpp"

#include "Logger/Logger.hpp"

FranAudio::Sound::Sound::Sound(size_t soundID, size_t waveDataIndex)
	: soundID(soundID), waveDataIndex(waveDataIndex)
{
	if (soundID == SIZE_MAX)
	{
		Logger::LogError("FranAudio: Sound ID cannot stay default");
	}

	if (waveDataIndex == SIZE_MAX)
	{
		Logger::LogError("FranAudio: Sound Wave data index is empty!");
	}
}

size_t FranAudio::Sound::Sound::GetSoundID() const
{
    return soundID;
}

size_t FranAudio::Sound::Sound::GetWaveDataIndex() const
{
	return waveDataIndex;
}
