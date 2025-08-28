// FranticDreamer 2022-2025
#pragma once

#include <memory>

#include "WaveData/WaveData.hpp"

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
		Sound(size_t soundID = SIZE_MAX, size_t waveDataIndex = SIZE_MAX);

		/// <summary>
		/// Destructor.
		/// </summary>
		virtual ~Sound() = default;

		/// <summary>
		/// Check if the sound is valid.
		/// </summary>
		bool IsValid() const;
	
		/// <summary>
		/// Get the sound ID.
		/// </summary>
		size_t GetSoundID() const;

		/// <summary>
		/// Get the wave data index.
		/// </summary>
		size_t GetWaveDataIndex() const;

		/// <summary>
		/// Stop and clear the sound.
		/// </summary>
		void Stop() const;

		/// <summary>
		/// Set whether the sound is paused or not.
		/// </summary>
		/// <param name="isPaused">True to pause the sound, false to resume playback</param>
		void SetPaused(bool isPaused) const;

		/// <summary>
		/// Check if the sound is paused.
		/// </summary>
		bool IsPaused() const;

		/// <summary>
		/// Set the volume of the sound.
	 	/// </summary>
 		/// <param name="volume">Volume to set the sound to (0.0 - 1.0)</param>
 		void SetVolume(float volume) const;

		/// <summary>
		/// Get the volume of the sound.
 		/// </summary>
 		/// <returns>Current volume of the sound (0.0 - 1.0)</returns>
		float GetVolume() const;

		/// <summary>
		/// Set the position of the sound.
 		/// </summary>
		void SetPosition(const float position[3]) const;

		/// <summary>
		/// Get the position of the sound.
		/// </summary>
		void GetPosition(float outPosition[3]) const;
	};
}
