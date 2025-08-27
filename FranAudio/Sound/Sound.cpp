// FranticDreamer 2022-2025

#include "Sound.hpp"

#include "FranAudio.hpp"

#include "Backend/Backend.hpp"

#include "FranAudioShared/Logger/Logger.hpp"

FranAudio::Sound::Sound::Sound(size_t soundID, size_t waveDataIndex)
	: soundID(soundID), waveDataIndex(waveDataIndex)
{

}

bool FranAudio::Sound::Sound::IsValid() const
{
	return FranAudio::GetBackend()->IsSoundValid(soundID);
}

size_t FranAudio::Sound::Sound::GetSoundID() const
{
    return soundID;
}

size_t FranAudio::Sound::Sound::GetWaveDataIndex() const
{
	return waveDataIndex;
}

void FranAudio::Sound::Sound::Stop() const
{
	FranAudio::GetBackend()->StopPlayingSound(soundID);
}

void FranAudio::Sound::Sound::SetVolume(float volume) const
{
	FranAudio::GetBackend()->SetSoundVolume(soundID, volume);
}

float FranAudio::Sound::Sound::GetVolume() const
{
	return FranAudio::GetBackend()->GetSoundVolume(soundID);
}

void FranAudio::Sound::Sound::SetPosition(const float position[3]) const
{
	FranAudio::GetBackend()->SetSoundPosition(soundID, position);
}

void FranAudio::Sound::Sound::GetPosition(float outPosition[3]) const
{
	FranAudio::GetBackend()->GetSoundPosition(soundID, outPosition);
}
