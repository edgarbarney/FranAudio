// FranticDreamer 2022-2024

#include <iterator>
#include <filesystem>
#include <thread>

#include "Backend_miniaudio.hpp"

#include "Logger/Logger.hpp"

bool FranAudio::Backend::miniaudio::Init(FranAudio::Decoder::DecoderType decoderType)
{
	engineConfig = ma_engine_config_init();
	if (ma_engine_init(&engineConfig, &engine) != MA_SUCCESS)
	{
		Logger::LogError("MiniAudio: Failed to initialise engine");
		return false;
	}

	deviceConfig = ma_device_config_init(ma_device_type_playback);
	if (ma_device_init(nullptr, &deviceConfig, &device) != MA_SUCCESS)
	{
		Logger::LogError("MiniAudio: Failed to initialise device");
		ma_engine_uninit(&engine);
		return false;
	}

	// In case we're using miniaudio decoder with custom decoder backend
	defaultDecoderConfig = ma_decoder_config_init_default();
	defaultDecoderConfig.pCustomBackendUserData = nullptr;
	defaultDecoderConfig.ppCustomBackendVTables = miniaudio_backendVTables;
	defaultDecoderConfig.customBackendCount = std::size(miniaudio_backendVTables);

	if(decoderType == FranAudio::Decoder::DecoderType::None)
	{
		Logger::LogError("MiniAudio: No decoder type specified");
		Logger::LogError("MiniAudio: Defaulting to miniaudio decoder");
		decoderType = FranAudio::Decoder::DecoderType::libnyquist;
		//return false;
	}

	SetDecoder(decoderType);

	return true;
}

void FranAudio::Backend::miniaudio::Reset()
{
	ma_engine_uninit(&engine);
	ma_engine_init(&engineConfig, &engine);
}

void FranAudio::Backend::miniaudio::Shutdown()
{
	ma_engine_uninit(&engine);
}

const std::vector<FranAudio::Decoder::DecoderType>& FranAudio::Backend::miniaudio::GetSupportedDecoders() const
{
	static const std::vector<FranAudio::Decoder::DecoderType> supportedDecoders = 
	{
		FranAudio::Decoder::DecoderType::miniaudio,
		FranAudio::Decoder::DecoderType::libnyquist,
	};

	return supportedDecoders;
}

void FranAudio::Backend::miniaudio::SetListenerTransform(const float position[3], const float forward[3], const float up[3])
{
	ma_engine_listener_set_position(&engine, 0, position[0], position[1], position[2]);
	ma_engine_listener_set_direction(&engine, 0, forward[0], forward[1], forward[2]);
	ma_engine_listener_set_world_up(&engine, 0, up[0], up[1], up[2]);
}

void FranAudio::Backend::miniaudio::SetMasterVolume(float volume)
{
	ma_engine_set_volume(&engine, 1.0f);
}

size_t FranAudio::Backend::miniaudio::LoadAudioFile(const std::string& filename)
{
	std::filesystem::path filePath(filename);

	if (!std::filesystem::exists(filePath))
	{
		Logger::LogError("MiniAudio: File does not exist: " + filename);
		return SIZE_MAX;
	}

	if (std::filesystem::is_directory(filePath))
	{
		Logger::LogError("MiniAudio: File is a directory: " + filename);
		return SIZE_MAX;
	}

	if (std::filesystem::is_empty(filePath))
	{
		Logger::LogError("MiniAudio: File is empty: " + filename);
		return SIZE_MAX;
	}

	FranAudio::Sound::WaveData waveData;
	bool result = currentDecoder->DecodeAudioFile(filename, waveData, *this);

	if (!result)
	{
		Logger::LogError("MiniAudio: Failed to decode audio file: " + filename);
		return SIZE_MAX;
	}

	waveDataCache.emplace_back(waveData);
	size_t index = waveDataCache.size() - 1;
	filenameWaveMap[filename] = index;

	// Debug
	// TODO: Remove this
	//PlayAudioFileNoChecks(filename);

	Logger::LogSuccess("MiniAudio: Loaded audio file: " + filename);

	return index;
}

size_t FranAudio::Backend::miniaudio::PlayAudioFile(const std::string& filename)
{
	return PlayAudioFileNoChecks(filename);
}

size_t FranAudio::Backend::miniaudio::PlayAudioFileNoChecks(const std::string& filename)
{
	auto it = filenameWaveMap.find(filename); // Filename - Wave data cache index
	if (it == filenameWaveMap.end())
	{
		Logger::LogError("MiniAudio: Audio file not loaded: " + filename);
		return SIZE_MAX;
	}

	const FranAudio::Sound::WaveData& waveData = waveDataCache[it->second];

	MiniaudioSound* miniaudioSound = new MiniaudioSound;

	miniaudioSound->audioBufferConfig = ma_audio_buffer_config_init(ConvertFormat(waveData.GetFormat()), waveData.GetChannels(), waveData.SizeInFrames(), waveData.GetFrames().data(), nullptr);
	miniaudioSound->audioBufferConfig.sampleRate = waveData.GetSampleRate(); // Why is this not set in the config init function?
	ma_audio_buffer_init(&miniaudioSound->audioBufferConfig, &miniaudioSound->audioBuffer);

	ma_sound_init_from_data_source(&engine, &miniaudioSound->audioBuffer, 0, nullptr, &miniaudioSound->sound);
	//ma_sound_init_from_file(&engine, filename.c_str(), 0, nullptr, nullptr, &sound);

	activeSounds.emplace_back(activeSounds.size(), it->second);
	miniaudioSoundData.emplace_back(miniaudioSound);

	ma_sound_set_volume(&miniaudioSound->sound, 1.0f);
	ma_sound_start(&miniaudioSound->sound);

	// Debug
	// TODO: Remove this
	/*
	while (ma_sound_is_playing(&miniaudioSound->sound))
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	*/
	return activeSounds.size() - 1;
}

size_t FranAudio::Backend::miniaudio::PlayAudioFileStream(const std::string& filename)
{
	return SIZE_MAX;
}

ma_decoder_config* FranAudio::Backend::miniaudio::GetDefaultDecoderConfig()
{
	return &defaultDecoderConfig;
}

FranAudio::Sound::WaveFormat FranAudio::Backend::miniaudio::ConvertFormat(ma_format format)
{
	switch (format)
	{
	case ma_format_u8:
		return Sound::WaveFormat::PCM_8bit;
	case ma_format_s16:
		return Sound::WaveFormat::PCM_16bit;
	case ma_format_s24:
		return Sound::WaveFormat::PCM_24bit;
	case ma_format_s32:
		return Sound::WaveFormat::PCM_32bit;
	case ma_format_f32:
		return Sound::WaveFormat::IEEE_FLOAT;
	default:
		return Sound::WaveFormat::Unknown;
	}
}

ma_format FranAudio::Backend::miniaudio::ConvertFormat(Sound::WaveFormat format)
{
	switch (format)
	{
	case Sound::WaveFormat::PCM_8bit:
		return ma_format_u8;
	case Sound::WaveFormat::PCM_16bit:
		return ma_format_s16;
	case Sound::WaveFormat::PCM_24bit:
		return ma_format_s24;
	case Sound::WaveFormat::PCM_32bit:
		return ma_format_s32;
	case Sound::WaveFormat::IEEE_FLOAT:
		return ma_format_f32;
	default:
		return ma_format_unknown;
	}
}