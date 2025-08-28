// FranticDreamer 2022-2025

#include "Sound.hpp"

#include "FranAudio.hpp"

#include "Backend/Backend.hpp"

#include "FranAudioShared/Logger/Logger.hpp"

namespace FranAudio::Sound
{
	Sound::Sound(size_t soundID, size_t waveDataIndex)
		: soundID(soundID), waveDataIndex(waveDataIndex), isPaused(false), pausedFrame(0)
	{

	}

	bool Sound::IsValid() const
	{
		return FranAudio::GetBackend()->IsSoundValid(soundID);
	}

	size_t Sound::GetSoundID() const
	{
		return soundID;
	}

	size_t Sound::GetWaveDataIndex() const
	{
		return waveDataIndex;
	}

	void Sound::Stop() const
	{
		FranAudio::GetBackend()->StopPlayingSound(soundID);
	}

	void Sound::SetPaused(bool isPaused) const
	{
		FranAudio::GetBackend()->SetSoundPaused(soundID, isPaused);
	}

	bool Sound::IsPaused() const
	{
		return FranAudio::GetBackend()->IsSoundPaused(soundID);
	}

	void Sound::SetVolume(float volume) const
	{
		FranAudio::GetBackend()->SetSoundVolume(soundID, volume);
	}

	float Sound::GetVolume() const
	{
		return FranAudio::GetBackend()->GetSoundVolume(soundID);
	}

	void Sound::SetPosition(const float position[3]) const
	{
		FranAudio::GetBackend()->SetSoundPosition(soundID, position);
	}

	void Sound::GetPosition(float outPosition[3]) const
	{
		FranAudio::GetBackend()->GetSoundPosition(soundID, outPosition);
	}

	// ========================
	// INTERNAL USE ONLY
	// ========================

	void Sound::_Internal_SetPaused(bool isPaused)
	{
		this->isPaused = isPaused;
	}

	bool Sound::_Internal_GetPaused() const
	{
		return isPaused;
	}

	void Sound::_Internal_SetPausedFrame(size_t frame)
	{
		pausedFrame = frame;
	}

	size_t Sound::_Internal_GetPausedFrame() const
	{
		return pausedFrame;
	}
}