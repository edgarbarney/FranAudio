// FranticDreamer 2022-2025
#pragma once

#include <vector>
#include <string>

#include "FranAudioAPI.hpp"

#include "WaveFormats.hpp"

namespace FranAudio::Sound
{
	/// <summary>
	/// Contains decoded audio data.
	/// </summary>
	class WaveData
	{
	private:
		std::string filename;			///<summary> Filename of the audio file. </summary>
		size_t waveDataIndex;			///<summary> ID of the wave data. </summary>

		WaveFormat format;				///<summary> Audio format. </summary>
		double length;					///<summary> Length of the audio in seconds. </summary>
		char channels;					///<summary> Number of channels. </summary>
		int sampleRate;					///<summary> Sample rate. </summary>

		// =========
		// Frames
		// =========

		SampleFrameContainer frames;	///<summary> Audio samples. </summary>
		unsigned char frameSize;		///<summary> Bits per frame * channels. </summary>

	public:
		/// <summary>
		/// Default constructor.
		/// </summary>
		FRANAUDIO_API WaveData(const std::string& filename = "", size_t waveDataIndex = 0, WaveFormat format = WaveFormat::Unknown, double length = 0.0, int channels = 0, int sampleRate = 0);

		FRANAUDIO_API void SetFilename(const std::string& filename);
		FRANAUDIO_API void SetWaveDataIndex(size_t index);
		FRANAUDIO_API void SetFormat(WaveFormat format);
		FRANAUDIO_API void SetLength(double length);
		FRANAUDIO_API void SetChannels(char channels);
		FRANAUDIO_API void SetSampleRate(int sampleRate);

		[[nodiscard]] FRANAUDIO_API const std::string& GetFilename() const;
		[[nodiscard]] FRANAUDIO_API size_t GetWaveDataIndex() const;
		[[nodiscard]] FRANAUDIO_API WaveFormat GetFormat() const;
		[[nodiscard]] FRANAUDIO_API double GetLength() const;
		[[nodiscard]] FRANAUDIO_API char GetChannels() const;
		[[nodiscard]] FRANAUDIO_API int GetSampleRate() const;

		// =========
		// Frame Stuff
		// =========

		/// <summary>
		/// Size in frames.
		/// Aka: Total Frames / Channels.
		/// </summary>
		[[nodiscard]] const FRANAUDIO_API size_t SizeInFrames() const;

		/// <summary>
		/// Set size per frame.
		/// Aka: Bits per frame * channels.
		/// </summary>
		FRANAUDIO_API void SetFrameSize(unsigned char frameSize);

		/// <summary>
		/// Size per frame.
		/// Aka: Bits per frame * channels.
		/// </summary>
		[[nodiscard]] FRANAUDIO_API unsigned char GetFrameSize() const;

		/// <summary>
		/// Set the frames manually.
		/// This will set frame vector.
		/// </summary>
		/// <param name="frames"></param>
		FRANAUDIO_API void SetFrames(const SampleFrameContainer& frames);

		/// <summary>
		/// Get a reference to frames vector.
		/// </summary>
		FRANAUDIO_API SampleFrameContainer& GetFramesRef();

		/// <summary>
		/// Get frames.
		/// </summary>
		[[nodiscard]] const FRANAUDIO_API SampleFrameContainer& GetFrames() const;
	};
}