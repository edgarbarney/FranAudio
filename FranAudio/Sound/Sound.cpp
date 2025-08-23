// FranticDreamer 2022-2025

#include "Sound.hpp"

#include "FranAudio.hpp"

#include "Backend/Backend.hpp"

#include "FranAudioShared/Logger/Logger.hpp"

FranAudio::Sound::Sound::Sound(size_t soundID, size_t waveDataIndex)
	: soundID(soundID), waveDataIndex(waveDataIndex)
{

}

size_t FranAudio::Sound::Sound::GetSoundID() const
{
    return soundID;
}

size_t FranAudio::Sound::Sound::GetWaveDataIndex() const
{
	return waveDataIndex;
}

void FranAudio::Sound::Sound::SetPosition(const float position[3])
{
	FranAudio::GetBackend()->SetSoundPosition(soundID, position);
}
