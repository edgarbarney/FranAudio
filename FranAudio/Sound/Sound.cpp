// FranticDreamer 2022-2025

#include "Sound.hpp"

#include "FranAudio.hpp"

#include "Backend/Backend.hpp"

#include "FranAudioShared/Logger/Logger.hpp"

namespace FranAudio::Sound
{
	FRANAUDIO_API Sound::Sound(size_t soundID, size_t waveDataIndex)
		: soundID(soundID), waveDataIndex(waveDataIndex)
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

	FRANAUDIO_API size_t Sound::GetWaveDataIndex() const
	{
		return waveDataIndex;
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

	FRANAUDIO_API void Sound::SetPosition(std::span<const float, 3> position) const
	{
		FranAudio::GetBackend()->SetSoundPosition(soundID, position);
	}

	FRANAUDIO_API void Sound::GetPosition(std::span<const float, 3> outPosition) const
	{
		FranAudio::GetBackend()->GetSoundPosition(soundID, outPosition);
	}

	FRANAUDIO_API void Sound::SetAttenuation(float rolloffFactor, float minDistance, float maxDistance) const
	{
		FranAudio::GetBackend()->SetSoundAttenuation(soundID, rolloffFactor, minDistance, maxDistance);
	}

	FRANAUDIO_API void Sound::GetAttenuation(float& outRolloffFactor, float& outMinDistance, float& outMaxDistance) const
	{
		FranAudio::GetBackend()->GetSoundAttenuation(soundID, outRolloffFactor, outMinDistance, outMaxDistance);
	}
}
