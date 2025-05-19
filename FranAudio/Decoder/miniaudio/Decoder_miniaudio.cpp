// FranticDreamer 2022-2024

#include <iterator>
#include <filesystem>

//Already included in FranAudio.hpp
//#include "miniaudio/miniaudio.h"

#include "FranAudio.hpp"

#include "Backend/Backend.hpp"
#include "Backend/miniaudio/Backend_miniaudio.hpp"
#include "Decoder_miniaudio.hpp"

#include "Logger/Logger.hpp"

bool FranAudio::Decoder::miniaudio::Init()
{
	if (FranAudio::gGlobals.currentBackend && FranAudio::gGlobals.currentBackend->GetBackendType() == FranAudio::Backend::BackendType::miniaudio)
	{ 
		// Our backend already initialised stuff for us, so we can just return true.
		return true;
	}

	isStandalone = true;
	Logger::LogMessage("MiniAudio S.D.: Miniaudio is initialising as a standalone decoder, without miniaudio backend");

	engineConfig = ma_engine_config_init();
	if (ma_engine_init(&engineConfig, &engine) != MA_SUCCESS)
	{
		Logger::LogError("MiniAudio S.D.: Failed to initialise engine");
		return false;
	}

	deviceConfig = ma_device_config_init(ma_device_type_playback);
	if (ma_device_init(nullptr, &deviceConfig, &device) != MA_SUCCESS)
	{
		Logger::LogError("MiniAudio S.D.: Failed to initialise device");
		ma_engine_uninit(&engine);
		return false;
	}

	// In case we're using miniaudio decoder with custom decoder backend
	defaultDecoderConfig = ma_decoder_config_init_default();
#if !defined(FRANAUDIO_USE_VORBIS) && !defined(FRANAUDIO_USE_OPUS)
	defaultDecoderConfig.pCustomBackendUserData = nullptr;
	defaultDecoderConfig.ppCustomBackendVTables = FranAudio::Backend::miniaudio_backendVTables;
	defaultDecoderConfig.customBackendCount = std::size(FranAudio::Backend::miniaudio_backendVTables);
#endif

	return true;
}

void FranAudio::Decoder::miniaudio::Reset()
{
	if (isStandalone)
	{
		ma_engine_uninit(&engine);
		ma_engine_init(&engineConfig, &engine);
	}
	else
	{
		// No need to reset anything here, as the backend will handle it.
		return;
	}
}

void FranAudio::Decoder::miniaudio::Shutdown()
{
	if (isStandalone)
	{
		ma_engine_uninit(&engine);
		ma_device_uninit(&device);
	}
	else
	{

		// No need to shutdown anything here, as the backend will handle it.
		return;
	}
}

FranAudio::Decoder::DecoderType FranAudio::Decoder::miniaudio::GetDecoderType()
{
	return DecoderType::miniaudio;
}

bool FranAudio::Decoder::miniaudio::DecodeAudioFile(const std::string& filename, FranAudio::Sound::WaveData& targetWaveData, FranAudio::Backend::Backend& caller)
{
	ma_decoder decoder;
	ma_decoder_config* temp = nullptr;

	if (isStandalone)
	{
		temp = &defaultDecoderConfig;
	}
	else
	{
		FranAudio::Backend::miniaudio& miniAudioRef = static_cast<FranAudio::Backend::miniaudio&>(caller);
		
		temp = miniAudioRef.GetDefaultDecoderConfig();
	}

	if (ma_decoder_init_file(filename.c_str(), temp, &decoder) != MA_SUCCESS)
	{
		Logger::LogError("MiniAudio: Failed to initialise decoder for file: " + filename);
		Logger::LogError("MiniAudio: Are you sure the audio file is in a supported format?");
		Logger::LogError("MiniAudio: Supported formats of current backend are: ", false);

		bool first = true;
		for (const auto& format : GetSupportedAudioFormats())
		{
			Logger::LogGeneric((first ? "" : " / ") + std::string(format), false);
			first = false;
		}
		Logger::LogNewline();
		return false;
	}

	const auto channels = decoder.outputChannels;
	const auto sampleRate = decoder.outputSampleRate;

	targetWaveData.SetFilename(filename);
	targetWaveData.SetFormat(FranAudio::Backend::miniaudio::ConvertFormat(decoder.outputFormat));
	targetWaveData.SetLength(0.0f);
	targetWaveData.SetChannels(channels);
	targetWaveData.SetSampleRate(sampleRate);
	targetWaveData.SetFrameSize(sizeof(float) * channels);

	ma_uint64 totalFrameCount = 0;
	if (ma_decoder_get_length_in_pcm_frames(&decoder, &totalFrameCount) == MA_SUCCESS && totalFrameCount > 0)
	{
		// Let's preallocate the buffer if we can get the total frame count.
		targetWaveData.GetFramesRef().resize(static_cast<size_t>(totalFrameCount) * channels);

		ma_uint64 framesRead = 0;
		if (ma_decoder_read_pcm_frames(&decoder, targetWaveData.GetFramesRef().data(), totalFrameCount, &framesRead) != MA_SUCCESS || framesRead == 0)
		{
			Logger::LogError("MiniAudio: Failed to read audio data from file: " + filename);
			ma_decoder_uninit(&decoder);
			return false;
		}

		targetWaveData.SetLength(static_cast<double>(framesRead) / sampleRate);
	}
	else
	{
		// Full file read fallback (e.g., for Vorbis/Opus)
		const size_t bufferFrames = 64u * 1024u; // 64K frames
		std::vector<float> buffer(bufferFrames * channels);
		size_t totalFrames = 0;

		while (true)
		{
			ma_uint64 framesRead = 0;
			if (ma_decoder_read_pcm_frames(&decoder, buffer.data(), bufferFrames, &framesRead) != MA_SUCCESS || framesRead == 0)
				break;

			totalFrames += static_cast<size_t>(framesRead);
			targetWaveData.GetFramesRef().insert(targetWaveData.GetFramesRef().end(), buffer.begin(), buffer.begin() + (framesRead * channels));
		}

		if (totalFrames == 0)
		{
			Logger::LogError("MiniAudio: Failed to read audio data from file: " + filename);
			ma_decoder_uninit(&decoder);
			return false;
		}

		targetWaveData.SetLength(static_cast<double>(totalFrames) / sampleRate);
	}

	ma_decoder_uninit(&decoder);

	return true;
}

const std::span<const std::string_view> FranAudio::Decoder::miniaudio::GetSupportedAudioFormats() const
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
