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
		//targetAudioData.samples = waveData.GetFrames();
		// TODO: Test the format stuff
		targetAudioData.sourceFormat = ConvertWaveFormat(waveData.GetFormat());
	}

	void libnyquist::ConvertNyqDataToFranData(const nqr::AudioData& audioData, FranAudio::Sound::WaveData& targetWaveData)
	{
		targetWaveData.SetChannels(audioData.channelCount);
		targetWaveData.SetSampleRate(audioData.sampleRate);
		targetWaveData.SetFrameSize(audioData.frameSize);
		targetWaveData.SetFrames(audioData.samples);
		// Format is always float in libnyquist
		targetWaveData.SetFormat(FranAudio::Sound::WaveFormat::IEEE_FLOAT);
		//targetWaveData.SetFormat(ConvertWaveFormat(audioData.sourceFormat));
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
