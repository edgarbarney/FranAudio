// FranticDreamer 2022-2025
#pragma once

#include <string>
#include <string_view>
#include <span>
#include <array>
#include <memory>

#include "FranAudioAPI.hpp"

#include "Decoder/DecoderTypes.hpp"

#include "Sound/WaveData/WaveData.hpp"

namespace FranAudio::Backend
{ 
	class Backend;
}

/// <summary>
/// Namespace for Decoder Abstract and Implementations.
/// </summary>
namespace FranAudio::Decoder
{
	/// <summary>
	/// Settings for decoding audio wave data.
	/// </summary>
	class FRANAUDIO_API DecodeSettings
	{
	private:
		/// <summary>
		/// If true, the decoded audio data will be kept in memory after reset.
		/// Until either the backend is shutdown or the persistent data is explicitly cleared.
		/// </summary>
		bool persistent;

		/// <summary>
		/// Forced number of channels for decoding audio files.
		/// 0 means no forced number of channels.
		/// </summary>
		char forcedChannels;

		/// <summary>
		/// Forced sample rate for decoding audio files.
		/// 0 means no forced sample rate.
		/// </summary>
		int forcedSampleRate;

		/// <summary>
		/// Forced format for decoding audio files.
		/// WaveFormat::Unknown means no forced format.
		/// </summary>
		FranAudio::Sound::WaveFormat forcedFormat;

	public:
		constexpr DecodeSettings(bool persistent = false, char forcedChannels = 0, int forcedSampleRate = 0, FranAudio::Sound::WaveFormat forcedFormat = FranAudio::Sound::WaveFormat::Unknown) noexcept
			: persistent(persistent), forcedChannels(forcedChannels), forcedSampleRate(forcedSampleRate), forcedFormat(forcedFormat)
		{
		}

		/// <summary>
		/// If true, the decoded audio data will be kept in memory after reset.
		/// Until either the backend is shutdown or the persistent data is explicitly cleared.
		/// </summary>
		/// <returns>Is persistent?</returns>
		constexpr bool IsPersistent() const noexcept
		{
			return persistent;
		}

		/// <summary>
		/// Set whether the decoded audio data will be kept in memory after reset.
		/// Until either the backend is shutdown or the persistent data is explicitly cleared.
		/// </summary>
		/// <param name="isPersistent">New persistency state.</param>
		constexpr void SetPersistent(bool isPersistent) noexcept
		{
			persistent = isPersistent;
		}

		/// <summary>
		/// Get the forced number of channels for decoding audio files.
		/// 0 means no forced number of channels.
		/// </summary>
		/// <returns>Forced number of channels.</returns>
		constexpr char GetForcedChannels() const noexcept
		{
			return forcedChannels;
		}

		/// <summary>
		/// Set the forced number of channels for decoding audio files.
		/// 0 means no forced number of channels.
		/// </summary>
		/// <param name="channels">New forced number of channels.</param>
		constexpr void SetForcedChannels(char channels) noexcept
		{
			forcedChannels = channels;
		}

		/// <summary>
		/// Get the forced sample rate for decoding audio files. 0 means no forced sample rate.
		/// </summary>
		/// <returns>Forced sample rate for decoding audio files.</returns>
		constexpr int GetForcedSampleRate() const noexcept
		{
			return forcedSampleRate;
		}

		/// <summary>
		/// Set the forced sample rate for decoding audio files. 0 means no forced sample rate.
		/// </summary>
		/// <param name="sampleRate">New forced sample rate for decoding audio files.</param>
		constexpr void SetForcedSampleRate(int sampleRate) noexcept
		{
			forcedSampleRate = sampleRate;
		}

		/// <summary>
		/// Get the forced format for decoding audio files. WaveFormat::Unknown means no forced format.
		/// </summary>
		/// <returns>Forced format for decoding audio files.</returns>
		constexpr FranAudio::Sound::WaveFormat GetForcedFormat() const noexcept
		{
			return forcedFormat;
		}

		/// <summary>
		/// Set the forced format for decoding audio files. WaveFormat::Unknown means no forced format.
		/// </summary>
		constexpr void SetForcedFormat(FranAudio::Sound::WaveFormat format) noexcept
		{
			forcedFormat = format;
		}
	};


	// ========================
	// Decoder
	// ========================

	/// <summary>
	/// Abstract for decoder implementations.
	/// </summary>
	/// <remarks>
	/// A decoder is used to decode audio files into a format that can be played by the backend.
	/// A decoder is always managed by a backend.
	/// </remarks>
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
		virtual constexpr FRANAUDIO_API DecoderType GetDecoderType() const noexcept;

		/// <summary>
		/// Returns the name of the audio decoder.
		/// </summary>
		/// <returns>Name of the decoder.</returns>
		virtual constexpr FRANAUDIO_API const char* GetDecoderName() const noexcept;

		/// <summary>
		/// Decode an audio file.
		/// This is used to decode an audio file and store the result in the target WaveData.
		/// 
		/// <para>Important: Audio file MUST exist.</para>
		/// </summary>
		/// <returns>
		/// True if the decoding was successful, false otherwise.
		/// </returns>
		/// <param name="filename">Path to an existing audio file</param>
		/// <param name="targetWaveData">Target WaveData to store the decoded audio</param>
		/// <param name="caller">Caller backend</param>
		virtual bool DecodeAudioFile(const std::string& filename, FranAudio::Sound::WaveData& targetWaveData, FranAudio::Backend::Backend& caller, const DecodeSettings& settings = {}) = 0;

		/// <summary>
		/// Gets the supported audio formats in lower case.
		/// </summary>
		/// <returns>List of supported audio formats in lower case.</returns>
		virtual const std::span<const std::string_view> GetSupportedAudioFormats() const = 0;

		/// <summary>
		/// Create a new decoder instance.
		/// </summary>
		/// <param name="decoderType">Type of the decoder to create</param>
		/// <returns>New decoder instance</returns>
		static FRANAUDIO_API std::unique_ptr<FranAudio::Decoder::Decoder> CreateDecoder(DecoderType decoderType);

		// ========================
		// Sample Conversion
		// ========================

		/// <summary>
		/// Convert a float sample to an unsigned 8-bit integer sample.
		/// </summary>
		/// <param name="sample">Float sample to convert</param>
		/// <returns>Converted unsigned 8-bit integer sample</returns>
		static FranAudio::Sound::U8Sample FloatToU8(FranAudio::Sound::FloatSample sample);

		/// <summary>
		/// Convert an unsigned 8-bit integer sample to a float sample.
		/// </summary>
		/// <param name="sample">Unsigned 8-bit integer sample to convert</param>
		/// <returns>Converted float sample</returns>
		static FranAudio::Sound::FloatSample U8ToFloat(FranAudio::Sound::U8Sample sample);

		/// <summary>
		/// Convert a float sample to a signed 16-bit integer sample.
		/// </summary>
		/// <param name="sample">Float sample to convert</param>
		/// <returns>Converted signed 16-bit integer sample</returns>
		static FranAudio::Sound::S16Sample FloatToS16(FranAudio::Sound::FloatSample sample);

		/// <summary>
		/// Convert a signed 16-bit integer sample to a float sample.
		/// </summary>
		/// <param name="sample">Signed 16-bit integer sample to convert</param>
		/// <returns>Converted float sample</returns>
		static FranAudio::Sound::FloatSample S16ToFloat(FranAudio::Sound::S16Sample sample);

		/// <summary>
		/// Converts a 24-bit signed integer audio sample to a floating-point sample.
		/// Contains precision loss.
		/// </summary>
		/// <param name="sample">The 24-bit signed integer audio sample to convert.</param>
		/// <returns>The converted audio sample as a floating-point value.</returns>
		static FranAudio::Sound::FloatSample S24ToFloat(FranAudio::Sound::FloatSample sample);

		/// <summary>
		/// Converts a floating-point audio sample to 24-bit signed integer format.
		/// Contains precision loss.
		/// </summary>
		/// <param name="sample">The floating-point audio sample to convert.</param>
		/// <returns>The converted audio sample in 24-bit signed integer format.</returns>
		static FranAudio::Sound::FloatSample FloatToS24(FranAudio::Sound::FloatSample sample);

		/// <summary>
		/// Convert a buffer of float samples to a buffer of unsigned 8-bit integer samples.
		/// </summary>
		/// <param name="samples">Buffer of float samples to convert</param>
		/// <returns>Converted buffer of unsigned 8-bit integer samples</returns>
		static FranAudio::Sound::U8SampleContainer FloatBufferToU8(const FranAudio::Sound::FloatSampleContainer& samples);

		/// <summary>
		/// Convert a buffer of unsigned 8-bit integer samples to a buffer of float samples.
		/// </summary>
		/// <param name="samples">Buffer of unsigned 8-bit integer samples to convert</param>
		/// <returns>Converted buffer of float samples</returns>
		static FranAudio::Sound::FloatSampleContainer U8BufferToFloat(const FranAudio::Sound::U8SampleContainer& samples);

		/// <summary>
		/// Convert a buffer of float samples to a buffer of signed 16-bit integer samples.
		/// </summary>
		/// <param name="samples">Buffer of float samples to convert</param>
		/// <returns>Converted buffer of signed 16-bit integer samples</returns>
		static FranAudio::Sound::S16SampleContainer FloatBufferToS16(const FranAudio::Sound::FloatSampleContainer& samples);

		/// <summary>
		/// Convert a buffer of signed 16-bit integer samples to a buffer of float samples.
		/// </summary>
		/// <param name="samples">Buffer of signed 16-bit integer samples to convert</param>
		/// <returns>Converted buffer of float samples</returns>
		static FranAudio::Sound::FloatSampleContainer S16BufferToFloat(const FranAudio::Sound::S16SampleContainer& samples);

		/// <summary>
		/// Converts a container of 24-bit signed integer audio samples to a container of floating-point samples.
		/// Contains precision loss.
		/// </summary>
		/// <param name="samples">Container to be modified, holding 24-bit signed integer audio samples to be converted.</param>
		static void S24BufferToFloat(FranAudio::Sound::FloatSampleContainer& samples);

		/// <summary>
		/// Converts a container of floating-point audio samples to 24-bit signed integer format.
		/// Contains precision loss.
		/// </summary>
		/// <param name="samples">Container to be modified, holding the input floating-point audio samples.</param>
		static void FloatBufferToS24(FranAudio::Sound::FloatSampleContainer& samples);
	};
}
