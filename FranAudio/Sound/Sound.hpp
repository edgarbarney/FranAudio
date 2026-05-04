// FranticDreamer 2022-2025
#pragma once

#include <memory>

#include "FranAudioAPI.hpp"

#include "WaveData/WaveData.hpp"

/// <summary>
/// Namespace for Sound related classes and functions.
/// </summary>
namespace FranAudio::Sound
{
	/// <summary>
	/// Represents a sound instance.
	/// 
	/// Basically a struct, combined with a wrapper around
	/// sound instances and backend interaction. 
	/// </summary>
	class Sound
	{
	private:
		/// <summary>
		/// ID of the sound.
		/// </summary>
		size_t soundID;

		/// <summary>
		/// Wave data index of the sound.
		/// 
		/// This is used to identify the wave data associated with the sound.
		/// </summary>
		size_t waveDataIndex;

	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		FRANAUDIO_API Sound(size_t soundID = SIZE_MAX, size_t waveDataIndex = SIZE_MAX);

		/// <summary>
		/// Destructor.
		/// </summary>
		virtual FRANAUDIO_API ~Sound() = default;

		/// <summary>
		/// Check if the sound is valid.
		/// </summary>
		FRANAUDIO_API bool IsValid() const;
	
		/// <summary>
		/// Get the sound ID.
		/// </summary>
		FRANAUDIO_API size_t GetSoundID() const;

		/// <summary>
		/// Get the wave data index.
		/// </summary>
		FRANAUDIO_API size_t GetWaveDataIndex() const;

		/// <summary>
		/// Stop and clear the sound.
		/// </summary>
		FRANAUDIO_API void Stop() const;

		/// <summary>
		/// Set whether the sound is paused or not.
		/// </summary>
		/// <param name="isPaused">True to pause the sound, false to resume playback</param>
		FRANAUDIO_API void SetPaused(bool isPaused) const;

		/// <summary>
		/// Check if the sound is paused.
		/// </summary>
		FRANAUDIO_API bool IsPaused() const;

		/// <summary>
		/// Set the volume of the sound.
	 	/// </summary>
 		/// <param name="volume">Volume to set the sound to (0.0 - 1.0)</param>
		FRANAUDIO_API void SetVolume(float volume) const;

		/// <summary>
		/// Get the volume of the sound.
 		/// </summary>
 		/// <returns>Current volume of the sound (0.0 - 1.0)</returns>
		FRANAUDIO_API float GetVolume() const;

		/// <summary>
		/// Set the pitch of the sound.
		/// </summary>
		/// <param name="pitch">Pitch to set the sound to (1.0 = normal pitch)</param>
		FRANAUDIO_API void SetPitch(float pitch) const;

		/// <summary>
		/// Get the pitch of the sound.
		/// </summary>
		FRANAUDIO_API float GetPitch() const;

		/// <summary>
		/// Set the position of the sound.
 		/// </summary>
		FRANAUDIO_API void SetPosition(const float position[3]) const;

		/// <summary>
		/// Get the position of the sound.
		/// </summary>
		FRANAUDIO_API void GetPosition(float outPosition[3]) const;

		/// <summary>
		/// Set the attenuation parameters of the sound.
		/// </summary>
		/// <param name="rolloffFactor">Rolloff factor to set the sound attenuation to</param>
		/// <param name="minDistance">Minimum distance to set the sound attenuation to</param>
		/// <param name="maxDistance">Maximum distance to set the sound attenuation to</param>
		FRANAUDIO_API void SetAttenuation(float rolloffFactor, float minDistance, float maxDistance) const;

		/// <summary>
		/// Get the attenuation parameters of the sound.
		/// </summary>
		/// <param name="outRolloffFactor">Output rolloff factor of the sound attenuation</param>
		/// <param name="outMinDistance">Output minimum distance of the sound attenuation</param>
		/// <param name="outMaxDistance">Output maximum distance of the sound attenuation</param>
		FRANAUDIO_API void GetAttenuation(float& outRolloffFactor, float& outMinDistance, float& outMaxDistance) const;

	};
}
