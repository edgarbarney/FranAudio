// FranticDreamer 2022-2025

#include "Decoder_libnyquist.hpp"

#include "FranAudioShared/Logger/Logger.hpp"

namespace FranAudio::Decoder
{
	bool libnyquist::Init()
	{
		return true;
	}

	void libnyquist::Reset()
	{

	}

	void libnyquist::Shutdown()
	{

	}

	DecoderType libnyquist::GetDecoderType()
	{
		return DecoderType::libnyquist;
	}

	bool libnyquist::DecodeAudioFile(const std::string& filename, FranAudio::Sound::WaveData& targetWaveData, FranAudio::Backend::Backend& caller)
	{
		nqr::AudioData audioData;
		soundLoader.Load(&audioData, filename);

		ConvertNyqDataToFranData(audioData, targetWaveData);

		return true;
	}

	const std::span<const std::string_view> libnyquist::GetSupportedAudioFormats() const
	{
		static constexpr std::array formats = 
		{
			std::string_view{".wav"},
			std::string_view{".flac"},
			std::string_view{".mp3"},
	#if defined FRANAUDIO_USE_VORBIS || defined FRANAUDIO_USE_OPUS
			std::string_view{".ogg"},
	#endif
	#if defined FRANAUDIO_USE_OPUS
			std::string_view{".opus"}
	#endif
		};
		return formats;
	}

	void libnyquist::ConvertFranDataToNyqData(const FranAudio::Sound::WaveData& waveData, nqr::AudioData& targetAudioData)
	{
		targetAudioData.channelCount = waveData.GetChannels();
		targetAudioData.sampleRate = waveData.GetSampleRate();
		targetAudioData.frameSize = waveData.GetFrameSize();
		
		const auto& frames = waveData.GetFrames();

		// If the format is not float, convert it to float
		if (std::holds_alternative<FranAudio::Sound::U8SampleContainer>(frames))
		{
			targetAudioData.samples = U8BufferToFloat(std::get<FranAudio::Sound::U8SampleContainer>(frames));
		}
		else if (std::holds_alternative<FranAudio::Sound::S16SampleContainer>(frames))
		{
			targetAudioData.samples = S16BufferToFloat(std::get<FranAudio::Sound::S16SampleContainer>(frames));
		}
		else if (std::holds_alternative<FranAudio::Sound::FloatSampleContainer>(frames))
		{
			targetAudioData.samples = std::get<FranAudio::Sound::FloatSampleContainer>(frames);
		}
		else
		{
			// Unknown format, set empty samples
			targetAudioData.samples.clear();
		}

		targetAudioData.sourceFormat = ConvertWaveFormat(waveData.GetFormat());
	}

	void libnyquist::ConvertNyqDataToFranData(const nqr::AudioData& audioData, FranAudio::Sound::WaveData& targetWaveData)
	{
		targetWaveData.SetChannels(audioData.channelCount);
		targetWaveData.SetSampleRate(audioData.sampleRate);
		targetWaveData.SetFrameSize(audioData.frameSize);

		// Convert libnyquist's format enum back into your WaveFormat
		FranAudio::Sound::WaveFormat format = ConvertWaveFormat(audioData.sourceFormat);
		targetWaveData.SetFormat(format);

		// Pick container type based on format
		auto& frames = targetWaveData.GetFramesRef();

		if (format == FranAudio::Sound::WaveFormat::PCM_8bit)
		{
			auto converted = FloatBufferToU8(audioData.samples);
			frames = std::move(converted);
		}
		else if (format == FranAudio::Sound::WaveFormat::PCM_16bit)
		{
			auto converted = FloatBufferToS16(audioData.samples);
			frames = std::move(converted);
		}
		else if (format == FranAudio::Sound::WaveFormat::IEEE_FLOAT)
		{
			frames = audioData.samples; // already float
		}
		else
		{
			// Unsupported format: just store float
			frames = audioData.samples;
			targetWaveData.SetFormat(FranAudio::Sound::WaveFormat::IEEE_FLOAT);
		}
	}

	FranAudio::Sound::U8Sample libnyquist::FloatToU8(FranAudio::Sound::FloatSample sample)
	{
		return static_cast<FranAudio::Sound::U8Sample>((std::clamp(sample, -1.0f, 1.0f) * 127.5f) + 128.0f);
	}

	FranAudio::Sound::FloatSample libnyquist::U8ToFloat(FranAudio::Sound::U8Sample sample)
	{
		return (static_cast<FranAudio::Sound::FloatSample>(sample) - 128.0f) / 127.5f;
	}

	FranAudio::Sound::S16Sample libnyquist::FloatToS16(FranAudio::Sound::FloatSample sample)
	{
		return static_cast<FranAudio::Sound::S16Sample>(std::clamp(sample, -1.0f, 1.0f) * 32767.0f);
	}

	FranAudio::Sound::FloatSample libnyquist::S16ToFloat(FranAudio::Sound::S16Sample sample)
	{
		return static_cast<FranAudio::Sound::FloatSample>(sample) / 32767.0f;
	}

	FranAudio::Sound::U8SampleContainer libnyquist::FloatBufferToU8(const FranAudio::Sound::FloatSampleContainer& samples)
	{
		FranAudio::Sound::U8SampleContainer outSamples;
		outSamples.reserve(samples.size());

		for (auto floatSample : samples)
		{
			outSamples.push_back(FloatToU8(floatSample));
		}

		return outSamples;
	}

	FranAudio::Sound::FloatSampleContainer libnyquist::U8BufferToFloat(const FranAudio::Sound::U8SampleContainer& samples)
	{
		FranAudio::Sound::FloatSampleContainer outSamples;
		outSamples.reserve(samples.size());

		for (auto u8Sample : samples)
		{
			outSamples.push_back(U8ToFloat(u8Sample));
		}

		return outSamples;
	}

	FranAudio::Sound::S16SampleContainer libnyquist::FloatBufferToS16(const FranAudio::Sound::FloatSampleContainer& samples)
	{
		FranAudio::Sound::S16SampleContainer outSamples;
		outSamples.reserve(samples.size());

		for (auto floatSample : samples)
		{
			outSamples.push_back(FloatToS16(floatSample));
		}

		return outSamples;
	}


	FranAudio::Sound::FloatSampleContainer libnyquist::S16BufferToFloat(const FranAudio::Sound::S16SampleContainer& samples)
	{
		FranAudio::Sound::FloatSampleContainer outSamples;
		outSamples.reserve(samples.size());

		for (auto s16Sample : samples)
		{
			outSamples.push_back(S16ToFloat(s16Sample));
		}

		return outSamples;
	}

	FranAudio::Sound::WaveFormat libnyquist::ConvertWaveFormat(const nqr::PCMFormat& format)
	{
		switch (format)
		{
		case nqr::PCMFormat::PCM_S8: // Should we not ignore signed one?
		case nqr::PCMFormat::PCM_U8:
			return FranAudio::Sound::WaveFormat::PCM_8bit;
		case nqr::PCMFormat::PCM_16:
			return FranAudio::Sound::WaveFormat::PCM_16bit;
		case nqr::PCMFormat::PCM_24:
			return FranAudio::Sound::WaveFormat::PCM_24bit;
		case nqr::PCMFormat::PCM_32:
			return FranAudio::Sound::WaveFormat::PCM_32bit;
		case nqr::PCMFormat::PCM_FLT:
			return FranAudio::Sound::WaveFormat::IEEE_FLOAT;
		case nqr::PCMFormat::PCM_DBL:
			return FranAudio::Sound::WaveFormat::IEEE_DOUBLE;
		default:
			return FranAudio::Sound::WaveFormat::Unknown;
		}
	}

	nqr::PCMFormat libnyquist::ConvertWaveFormat(const FranAudio::Sound::WaveFormat& format)
	{
		switch (format)
		{
		case FranAudio::Sound::WaveFormat::PCM_8bit:
			return nqr::PCMFormat::PCM_U8;
		case FranAudio::Sound::WaveFormat::PCM_16bit:
			return nqr::PCMFormat::PCM_16;
		case FranAudio::Sound::WaveFormat::PCM_24bit:
			return nqr::PCMFormat::PCM_24;
		case FranAudio::Sound::WaveFormat::PCM_32bit:
			return nqr::PCMFormat::PCM_32;
		case FranAudio::Sound::WaveFormat::IEEE_FLOAT:
			return nqr::PCMFormat::PCM_FLT;
		case FranAudio::Sound::WaveFormat::IEEE_DOUBLE:
			return nqr::PCMFormat::PCM_DBL;
		default:
			return nqr::PCMFormat::PCM_END;
		}
	}
}
