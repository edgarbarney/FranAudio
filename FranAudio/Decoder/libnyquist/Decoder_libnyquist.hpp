// FranticDreamer 2022-2024
#pragma once

#include "Decoder/Decoder.hpp"

#include "libnyquist/Common.h"
#include "libnyquist/Decoders.h"

namespace FranAudio::Decoder
{
	class libnyquist : public Decoder
	{
	private:
		nqr::NyquistIO soundLoader;

	public:
		//Decoder();
		//~Decoder();

		/// <summary>
		/// Initialise the Decoder.
		/// This is used to initialise the Decoder and set it up for use.
		/// </summary>
		virtual bool Init() override;

		/// <summary>
		/// Reset the decoder. 
		/// This is used to reset the decoder to its initial state.
		/// </summary>
		virtual void Reset() override;

		/// <summary>
		/// Shutdown the decoder.
		/// This is used to shutdown the decoder and clean up any resources.
		/// </summary>
		virtual void Shutdown() override;

		/// <summary>
		/// Get the decoder type.
		/// </summary>
		/// <returns>Type of this decoder instance</returns>
		virtual DecoderType GetDecoderType() override;

		/// <summary>
		/// Decode an audio file.
		/// This is used to decode an audio file and store the result in the target WaveData.
		/// </summary>
		/// <remarks>
		/// Important: Audio file MUST exist.
		/// </remarks>
		/// <returns>
		/// True if the decoding was successful, false otherwise.
		/// </returns>
		/// <param name="filename">Path to an existing audio file</param>
		/// <param name="targetWaveData">Target WaveData to store the decoded audio</param>
		/// <param name="caller">Caller backend</param>
		virtual bool DecodeAudioFile(const std::string& filename, FranAudio::Sound::WaveData& targetWaveData, FranAudio::Backend::Backend& caller) override;

		/// <summary>
		/// Gets the supported audio formats in lower case.
		/// </summary>
		/// <returns>List of supported audio formats in lower case.</returns>
		virtual const std::span<const std::string_view> GetSupportedAudioFormats() const override;

		/// <summary>
		/// Converts the WaveData to libnyquist AudioData.
		/// </summary>
		/// <param name="waveData">WaveData to convert into libnyquist AudioData</param>
		static void ConvertFranDataToNyqData(const FranAudio::Sound::WaveData& waveData, nqr::AudioData& targetAudioData);
	
		/// <summary>
		/// Converts the libnyquist AudioData to WaveData.
		/// </summary>
		/// <param name="audioData">libnyquist AudioData to convert into WaveData</param>
		static void ConvertNyqDataToFranData(const nqr::AudioData& audioData, FranAudio::Sound::WaveData& targetWaveData);
	
		static FranAudio::Sound::WaveFormat ConvertWaveFormat(const nqr::PCMFormat& format);

		static nqr::PCMFormat ConvertWaveFormat(const FranAudio::Sound::WaveFormat& format);
	};
}
