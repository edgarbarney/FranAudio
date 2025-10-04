// FranticDreamer 2022-2025

#include "Decoder.hpp"
#include "miniaudio/Decoder_miniaudio.hpp"
#include "libnyquist/Decoder_libnyquist.hpp"

namespace FranAudio::Decoder
{
	// ========================
	// Decoder
	// ========================

	constexpr FRANAUDIO_API DecoderType Decoder::GetDecoderType() const noexcept
	{ 
		 return DecoderType::None;
	}

	constexpr FRANAUDIO_API const char* Decoder::GetDecoderName() const noexcept
	{
		return DecoderTypeNames[(size_t)GetDecoderType()];
	}

	FRANAUDIO_API std::unique_ptr<Decoder> Decoder::CreateDecoder(DecoderType decoderType)
	{
		std::unique_ptr<Decoder> newDecoder = nullptr;

		switch (decoderType)
		{
		case DecoderType::miniaudio:
			newDecoder = std::make_unique<miniaudio>();
			break;
		case DecoderType::libnyquist:
			newDecoder = std::make_unique<libnyquist>();
			break;
		default:
			return nullptr;
			break;
		}

		if (!newDecoder->Init())
		{
			return nullptr;
		}

		return newDecoder;
	}

	// ========================
	// Sample Conversion
	// ========================

	FranAudio::Sound::U8Sample Decoder::FloatToU8(FranAudio::Sound::FloatSample sample)
	{
		return static_cast<FranAudio::Sound::U8Sample>((std::clamp(sample, -1.0f, 1.0f) * 127.5f) + 128.0f);
	}

	FranAudio::Sound::FloatSample Decoder::U8ToFloat(FranAudio::Sound::U8Sample sample)
	{
		return (static_cast<FranAudio::Sound::FloatSample>(sample) - 128.0f) / 127.5f;
	}

	FranAudio::Sound::S16Sample Decoder::FloatToS16(FranAudio::Sound::FloatSample sample)
	{
		return static_cast<FranAudio::Sound::S16Sample>(std::clamp(sample, -1.0f, 1.0f) * 32767.0f);
	}

	FranAudio::Sound::FloatSample Decoder::S16ToFloat(FranAudio::Sound::S16Sample sample)
	{
		return static_cast<FranAudio::Sound::FloatSample>(sample) / 32767.0f;
	}

	FranAudio::Sound::FloatSample Decoder::S24ToFloat(FranAudio::Sound::FloatSample sample)
	{
		return std::clamp(sample / 8388607.0f, -1.0f, 1.0f);
	}

	FranAudio::Sound::FloatSample Decoder::FloatToS24(FranAudio::Sound::FloatSample sample)
	{
		return std::clamp(sample * 8388607.0f, -8388608.0f, 8388607.0f);
	}

	FranAudio::Sound::U8SampleContainer Decoder::FloatBufferToU8(const FranAudio::Sound::FloatSampleContainer& samples)
	{
		FranAudio::Sound::U8SampleContainer outSamples;
		outSamples.reserve(samples.size());

		for (auto floatSample : samples)
		{
			outSamples.push_back(FloatToU8(floatSample));
		}

		return outSamples;
	}

	FranAudio::Sound::FloatSampleContainer Decoder::U8BufferToFloat(const FranAudio::Sound::U8SampleContainer& samples)
	{
		FranAudio::Sound::FloatSampleContainer outSamples;
		outSamples.reserve(samples.size());

		for (auto u8Sample : samples)
		{
			outSamples.push_back(U8ToFloat(u8Sample));
		}

		return outSamples;
	}

	FranAudio::Sound::S16SampleContainer Decoder::FloatBufferToS16(const FranAudio::Sound::FloatSampleContainer& samples)
	{
		FranAudio::Sound::S16SampleContainer outSamples;
		outSamples.reserve(samples.size());

		for (auto floatSample : samples)
		{
			outSamples.push_back(FloatToS16(floatSample));
		}

		return outSamples;
	}


	FranAudio::Sound::FloatSampleContainer Decoder::S16BufferToFloat(const FranAudio::Sound::S16SampleContainer& samples)
	{
		FranAudio::Sound::FloatSampleContainer outSamples;
		outSamples.reserve(samples.size());

		for (auto s16Sample : samples)
		{
			outSamples.push_back(S16ToFloat(s16Sample));
		}

		return outSamples;
	}

	void Decoder::S24BufferToFloat(FranAudio::Sound::FloatSampleContainer& samples)
	{
		for (auto& sample : samples)
		{
			sample = S24ToFloat(sample);
		}
	}

	void Decoder::FloatBufferToS24(FranAudio::Sound::FloatSampleContainer& samples)
	{
		for (auto& sample : samples)
		{
			sample = FloatToS24(sample);
		}
	}
}
