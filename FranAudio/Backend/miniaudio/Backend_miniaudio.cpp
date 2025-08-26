// FranticDreamer 2022-2025

#include <iterator>
#include <filesystem>
#include <thread>

#include "Backend_miniaudio.hpp"

#include "FranAudioShared/Logger/Logger.hpp"

bool FranAudio::Backend::miniaudio::Init(FranAudio::Decoder::DecoderType decoderType)
{
	engineConfig = ma_engine_config_init();
	if (ma_engine_init(&engineConfig, &engine) != MA_SUCCESS)
	{
		FranAudioShared::Logger::LogError("MiniAudio: Failed to initialise engine");
		return false;
	}

	deviceConfig = ma_device_config_init(ma_device_type_playback);
	if (ma_device_init(nullptr, &deviceConfig, &device) != MA_SUCCESS)
	{
		FranAudioShared::Logger::LogError("MiniAudio: Failed to initialise device");
		ma_engine_uninit(&engine);
		return false;
	}

	// In case we're using miniaudio decoder with custom decoder backend
	defaultDecoderConfig = ma_decoder_config_init_default();

#if !defined(FRANAUDIO_USE_VORBIS) && !defined(FRANAUDIO_USE_OPUS)
	defaultDecoderConfig.pCustomBackendUserData = nullptr;
	defaultDecoderConfig.ppCustomBackendVTables = miniaudio_backendVTables;
	defaultDecoderConfig.customBackendCount = std::size(miniaudio_backendVTables);
#endif

	if(decoderType == FranAudio::Decoder::DecoderType::None)
	{
		FranAudioShared::Logger::LogError("MiniAudio: No decoder type specified");
		FranAudioShared::Logger::LogError("MiniAudio: Defaulting to miniaudio decoder");
		decoderType = FranAudio::Decoder::DecoderType::miniaudio;
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

// ========================
// Decoder Management
// ========================

const std::vector<FranAudio::Decoder::DecoderType>& FranAudio::Backend::miniaudio::GetSupportedDecoders() const
{
	static const std::vector<FranAudio::Decoder::DecoderType> supportedDecoders = 
	{
		FranAudio::Decoder::DecoderType::miniaudio,
		FranAudio::Decoder::DecoderType::libnyquist,
	};

	return supportedDecoders;
}

// ========================
// Listener (3D Audio)
// ========================

void FranAudio::Backend::miniaudio::SetListenerTransform(const float position[3], const float forward[3], const float up[3])
{
	SetListenerPosition(position);
	SetListenerOrientation(forward, up);
}

void FranAudio::Backend::miniaudio::GetListenerTransform(float position[3], float forward[3], float up[3])
{
	GetListenerPosition(position);
	GetListenerOrientation(forward, up);
}

void FranAudio::Backend::miniaudio::SetListenerPosition(const float position[3])
{
	ma_engine_listener_set_position(&engine, 0, position[0], position[1], position[2]);
}

void FranAudio::Backend::miniaudio::GetListenerPosition(float position[3])
{
	ma_vec3f result = ma_engine_listener_get_position(&engine, 0);
	position[0] = result.x;
	position[1] = result.y;
	position[2] = result.z;
}

void FranAudio::Backend::miniaudio::SetListenerOrientation(const float forward[3], const float up[3])
{
	ma_engine_listener_set_direction(&engine, 0, forward[0], forward[1], forward[2]);
	ma_engine_listener_set_world_up(&engine, 0, up[0], up[1], up[2]);
}

void FranAudio::Backend::miniaudio::GetListenerOrientation(float forward[3], float up[3])
{
	ma_vec3f fwd = ma_engine_listener_get_direction(&engine, 0);
	ma_vec3f u = ma_engine_listener_get_world_up(&engine, 0);
	forward[0] = fwd.x;
	forward[1] = fwd.y;
	forward[2] = fwd.z;
	up[0] = u.x;
	up[1] = u.y;
	up[2] = u.z;
}

void FranAudio::Backend::miniaudio::SetMasterVolume(float volume)
{
	ma_engine_set_volume(&engine, volume);
}

float FranAudio::Backend::miniaudio::GetMasterVolume()
{
	return ma_engine_get_volume(&engine);
}

// ========================
// Audio File Management
// ========================

size_t FranAudio::Backend::miniaudio::LoadAudioFile(const std::string& filename)
{
	std::filesystem::path filePath(filename);

	if (!std::filesystem::exists(filePath))
	{
		FranAudioShared::Logger::LogError("MiniAudio: File does not exist: " + filename);
		return SIZE_MAX;
	}

	if (std::filesystem::is_directory(filePath))
	{
		FranAudioShared::Logger::LogError("MiniAudio: File is a directory: " + filename);
		return SIZE_MAX;
	}

	if (std::filesystem::is_empty(filePath))
	{
		FranAudioShared::Logger::LogError("MiniAudio: File is empty: " + filename);
		return SIZE_MAX;
	}

	FranAudio::Sound::WaveData waveData;
	bool result = currentDecoder->DecodeAudioFile(filename, waveData, *this);

	if (!result)
	{
		FranAudioShared::Logger::LogError("MiniAudio: Failed to decode audio file: " + filename);
		return SIZE_MAX;
	}

	waveDataCache.emplace_back(waveData);
	size_t index = waveDataCache.size() - 1;
	filenameWaveMap[filename] = index;

	// Debug
	// TODO: Remove this
	//PlayAudioFileNoChecks(filename);

	FranAudioShared::Logger::LogSuccess("MiniAudio: Loaded audio file: " + filename);

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
		FranAudioShared::Logger::LogError("MiniAudio: Audio file not loaded: " + filename);
		return SIZE_MAX;
	}

	const auto& waveData = waveDataCache[it->second];
	auto miniaudioSound = std::make_unique<MiniaudioSound>();

	miniaudioSound->audioBufferConfig = ma_audio_buffer_config_init(ConvertFormat(waveData.GetFormat()), waveData.GetChannels(), waveData.SizeInFrames(), waveData.GetFrames().data(), nullptr);
	miniaudioSound->audioBufferConfig.sampleRate = waveData.GetSampleRate(); // Why is this not set in the config init function?
	ma_audio_buffer_init(&miniaudioSound->audioBufferConfig, &miniaudioSound->audioBuffer);
	ma_sound_init_from_data_source(&engine, &miniaudioSound->audioBuffer, 0, nullptr, &miniaudioSound->sound);

	// Generate our unique ID
	const size_t soundID = nextSoundID++;

	activeSounds[soundID] = FranAudio::Sound::Sound(soundID, it->second);
	ma_sound_set_volume(&miniaudioSound->sound, 1.0f);
	ma_sound_start(&miniaudioSound->sound);

	miniaudioSoundData[soundID] = std::move(miniaudioSound);

	return soundID;
}

size_t FranAudio::Backend::miniaudio::PlayAudioFileStream(const std::string& filename)
{
	return SIZE_MAX;
}

// ========================
// Sound Management
// ========================

void FranAudio::Backend::miniaudio::StopPlayingSound(size_t soundID)
{
	if (!IsSoundValid(soundID))
	{
		FranAudioShared::Logger::LogError("MiniAudio: Tried to stop an invalid sound.");
		return;
	}
	if (!activeSounds.contains(soundID))
	{
		FranAudioShared::Logger::LogError("MiniAudio: Sound ID is not playing: " + std::to_string(soundID));
		return;
	}
	if (!miniaudioSoundData.contains(soundID))
	{
		FranAudioShared::Logger::LogError("MiniAudio: Sound ID has invalid data: " + std::to_string(soundID));
		return;
	}

	auto& soundPtr = miniaudioSoundData[soundID];
	ma_sound_stop(&soundPtr->sound);
	ma_sound_uninit(&soundPtr->sound);

	miniaudioSoundData.erase(soundID);
	activeSounds.erase(soundID);
}

void FranAudio::Backend::miniaudio::SetSoundVolume(size_t soundID, float volume)
{
	if (!IsSoundValid(soundID))
	{
		FranAudioShared::Logger::LogError("MiniAudio: Tried to set volume of an invalid sound.");
		return;
	}
	ma_sound_set_volume(&miniaudioSoundData[soundID]->sound, volume);
}

float FranAudio::Backend::miniaudio::GetSoundVolume(size_t soundID)
{
	if (!IsSoundValid(soundID))
	{
		FranAudioShared::Logger::LogError("MiniAudio: Tried to get volume of an invalid sound.");
		return 0.0f;
	}
	return ma_sound_get_volume(&miniaudioSoundData[soundID]->sound);
}

void FranAudio::Backend::miniaudio::SetSoundPosition(size_t soundID, const float position[3])
{
	if (!IsSoundValid(soundID))
	{
		FranAudioShared::Logger::LogError("MiniAudio: Tried to set position of an invalid sound.");
		return;
	}

	ma_sound_set_position(&miniaudioSoundData[soundID]->sound, position[0], position[1], position[2]);
}

void FranAudio::Backend::miniaudio::GetSoundPosition(size_t soundID, float outPosition[3])
{
	if (!IsSoundValid(soundID))
	{
		FranAudioShared::Logger::LogError("MiniAudio: Tried to get position of an invalid sound.");
		return;
	}
	ma_vec3f pos = ma_sound_get_position(&miniaudioSoundData[soundID]->sound);
	outPosition[0] = pos.x;
	outPosition[1] = pos.y;
	outPosition[2] = pos.z;
}

// ========================
// Miniaudio Specific
// ========================

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