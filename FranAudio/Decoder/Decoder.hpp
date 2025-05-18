// FranticDreamer 2022-2024
#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <span>
#include <array>

#include "Sound/WaveData/WaveData.hpp"

namespace FranAudio::Backend
{ 
	class Backend;
}
	
namespace FranAudio::Decoder
{
	/// <summary>
	/// Possible decoder types.
	/// </summary>
	enum class DecoderType
	{
		None = 0,
		miniaudio,
		libnyquist,
	};

	/// <summary>
	/// Interface for decoder implementations.
	/// 
	/// A decoder is used to decode audio files into a format that can be played by the backend.
	/// </summary>
	class Decoder
	{
	public:
		Decoder() = default;
		~Decoder() = default;

		/// <summary>
		/// Initialise the Decoder.
		/// This is used to initialise the Decoder and set it up for use.
		/// </summary>
		virtual bool Init() = 0;

		/// <summary>
		/// Reset the decoder. 
		/// This is used to reset the decoder to its initial state.
		/// </summary>
		virtual void Reset() = 0;

		/// <summary>
		/// Shutdown the decoder.
		/// This is used to shutdown the decoder and clean up any resources.
		/// </summary>
		virtual void Shutdown() = 0;

		/// <summary>
		/// Get the decoder type.
		/// </summary>
		/// <returns>Type of this decoder instance</returns>
		virtual DecoderType GetDecoderType() { return DecoderType::None; }

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
		virtual bool DecodeAudioFile(const std::string& filename, FranAudio::Sound::WaveData& targetWaveData, FranAudio::Backend::Backend& caller) = 0;

		/// <summary>
		/// Gets the supported audio formats in lower case.
		/// </summary>
		/// <returns>List of supported audio formats in lower case.</returns>
		virtual const std::span<const std::string_view> GetSupportedAudioFormats() const = 0;
		//virtual const std::vector<std::string>& GetSupportedAudioFormats() const = 0;

		/// <summary>
		/// Create a new decoder instance.
		/// </summary>
		/// <param name="decoderType">Type of the decoder to create</param>
		/// <returns>New decoder instance</returns>
		static Decoder* CreateDecoder(DecoderType decoderType);
	};
}
