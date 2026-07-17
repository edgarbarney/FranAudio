// FranticDreamer 2022-2025

#include "Sound.hpp"

#include "FranAudio.hpp"

#include "Backend/Backend.hpp"

#include "FranAudioShared/Logger/Logger.hpp"

namespace FranAudio::Sound
{
	FRANAUDIO_API Sound::Sound(size_t soundID, size_t waveDataID)
		: soundID(soundID), waveDataID(waveDataID)
	{

	}

	FRANAUDIO_API bool Sound::IsValid() const
	{
		return FranAudio::GetBackend()->IsSoundValid(soundID);
	}

	FRANAUDIO_API size_t Sound::GetSoundID() const
	{
		return soundID;
	}

	FRANAUDIO_API size_t Sound::GetWaveDataID() const
	{
		return waveDataID;
	}

	FRANAUDIO_API void Sound::Stop() const
	{
		FranAudio::GetBackend()->StopPlayingSound(soundID);
	}

	FRANAUDIO_API void Sound::SetPaused(bool isPaused) const
	{
		FranAudio::GetBackend()->SetSoundPaused(soundID, isPaused);
	}

	FRANAUDIO_API bool Sound::IsPaused() const
	{
		return FranAudio::GetBackend()->IsSoundPaused(soundID);
	}

	FRANAUDIO_API void Sound::SetVolume(float volume) const
	{
		FranAudio::GetBackend()->SetSoundVolume(soundID, volume);
	}

	FRANAUDIO_API float Sound::GetVolume() const
	{
		return FranAudio::GetBackend()->GetSoundVolume(soundID);
	}

	FRANAUDIO_API void Sound::SetPitch(float pitch) const
	{
		FranAudio::GetBackend()->SetSoundPitch(soundID, pitch);
	}

	FRANAUDIO_API float Sound::GetPitch() const
	{
		return FranAudio::GetBackend()->GetSoundPitch(soundID);
	}

	FRANAUDIO_API void Sound::SetLooping(bool looping) const
	{
		FranAudio::GetBackend()->SetSoundLooping(soundID, looping);
	}

	FRANAUDIO_API bool Sound::IsLooping() const
	{
		return FranAudio::GetBackend()->IsSoundLooping(soundID);
	}

	FRANAUDIO_API void Sound::SetGroup(const std::string& groupName) const
	{
		FranAudio::GetBackend()->SetSoundGroup(soundID, groupName);
	}

	FRANAUDIO_API std::string Sound::GetGroup() const
	{
		return FranAudio::GetBackend()->GetSoundGroup(soundID);
	}

	FRANAUDIO_API void Sound::SetPosition(const FranAudioShared::Vector3& position) const
	{
		FranAudio::GetBackend()->SetSoundPosition(soundID, position);
	}

	FRANAUDIO_API FranAudioShared::Vector3 Sound::GetPosition() const
	{
		return FranAudio::GetBackend()->GetSoundPosition(soundID);
	}

	FRANAUDIO_API void Sound::SetAttenuation(float rolloffFactor, float minDistance, float maxDistance) const
	{
		FranAudio::GetBackend()->SetSoundAttenuation(soundID, rolloffFactor, minDistance, maxDistance);
	}

	FRANAUDIO_API FranAudioShared::SoundAttenuation Sound::GetAttenuation() const
	{
		return FranAudio::GetBackend()->GetSoundAttenuation(soundID);
	}
}
