// FranticDreamer 2022-2025
#pragma once

#include <vector>
#include <string>

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
		WaveData(const std::string& filename = "", size_t waveDataIndex = 0, WaveFormat format = WaveFormat::Unknown, double length = 0.0, int channels = 0, int sampleRate = 0);

		void SetFilename(const std::string& filename);
		void SetWaveDataIndex(size_t index);
		void SetFormat(WaveFormat format);
		void SetLength(double length);
		void SetChannels(char channels);
		void SetSampleRate(int sampleRate);

		[[nodiscard]] const std::string& GetFilename() const;
		[[nodiscard]] size_t GetWaveDataIndex() const;
		[[nodiscard]] WaveFormat GetFormat() const;
		[[nodiscard]] double GetLength() const;
		[[nodiscard]] char GetChannels() const;
		[[nodiscard]] int GetSampleRate() const;

		// =========
		// Frame Stuff
		// =========

		/// <summary>
		/// Size in frames.
		/// Aka: Total Frames / Channels.
		/// </summary>
		[[nodiscard]] const size_t SizeInFrames() const;

		/// <summary>
		/// Set size per frame.
		/// Aka: Bits per frame * channels.
		/// </summary>
		void SetFrameSize(unsigned char frameSize);

		/// <summary>
		/// Size per frame.
		/// Aka: Bits per frame * channels.
		/// </summary>
		[[nodiscard]] unsigned char GetFrameSize() const;

		/// <summary>
		/// Set the frames manually.
		/// This will set frame vector.
		/// </summary>
		/// <param name="frames"></param>
		void SetFrames(const SampleFrameContainer& frames);

		/// <summary>
		/// Get a reference to frames vector.
		/// </summary>
		SampleFrameContainer& GetFramesRef();

		/// <summary>
		/// Get frames.
		/// </summary>
		[[nodiscard]] const SampleFrameContainer& GetFrames() const;
	};
}