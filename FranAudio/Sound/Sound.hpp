// FranticDreamer 2022-2025
#pragma once

#include <memory>

#include "WaveData/WaveData.hpp"

namespace FranAudio::Sound
{
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
		/// Get the sound ID.
		/// </summary>
		size_t GetSoundID() const;

		/// <summary>
		/// Get the wave data index.
		/// </summary>
		size_t GetWaveDataIndex() const;
	};
}