// FranticDreamer 2022-2025

#include <iterator>
#include <thread>

#include "Backend_miniaudio.hpp"

#include "FranAudioShared/Logger/Logger.hpp"

namespace FranAudio::Backend
{
	FRANAUDIO_API miniaudio::~miniaudio()
	{
		if (isEngineInitialised)
		{
			Shutdown();
		}
	}

	FRANAUDIO_API bool miniaudio::Init(FranAudio::Decoder::DecoderType decoderType)
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

		isEngineInitialised = true;

		// In case we're using miniaudio decoder with custom decoder backend
		defaultDecoderConfig = ma_decoder_config_init_default();

	#if !defined(FRANAUDIO_USE_VORBIS) && !defined(FRANAUDIO_USE_OPUS)
		defaultDecoderConfig.pCustomBackendUserData = nullptr;
		defaultDecoderConfig.ppCustomBackendVTables = miniaudio_backendVTables;
		defaultDecoderConfig.customBackendCount = std::size(miniaudio_backendVTables);
	#else
		defaultDecoderConfig.pCustomBackendUserData = nullptr;
		defaultDecoderConfig.ppCustomBackendVTables = miniaudio_backendVTables;
		defaultDecoderConfig.customBackendCount = std::size(miniaudio_backendVTables);
	#endif

		bool decoderFail = false;

		if (decoderType == FranAudio::Decoder::DecoderType::None)
		{
			FranAudioShared::Logger::LogError("MiniAudio: No decoder type specified");
			decoderFail = true;
		}

		// Check if the requested decoder is supported
		const auto& supportedDecoders = GetSupportedDecoders();
		if (std::find(supportedDecoders.begin(), supportedDecoders.end(), decoderType) == supportedDecoders.end())
		{
			FranAudioShared::Logger::LogError("MiniAudio: Requested decoder is not supported by this backend");
			decoderFail = true;
		}

		if (decoderFail)
		{
			FranAudioShared::Logger::LogError("MiniAudio: Defaulting to miniaudio decoder");
			decoderType = FranAudio::Decoder::DecoderType::miniaudio;
		}

		// Decoder will be initialised by the FranAudio::Init
		currentDecoderType = decoderType;

		return Backend::Init();
	}

	FRANAUDIO_API void miniaudio::Reset()
	{
		Backend::Reset();
	}

	FRANAUDIO_API void miniaudio::Shutdown(bool forReset)
	{
		if (isEngineInitialised)
		{
			// Stop and uninit all active sounds
			for (auto& [soundID, soundData] : miniaudioSoundData)
			{
				ma_sound_stop(&soundData->sound);
				ma_sound_uninit(&soundData->sound);
				if (!soundData->isStreamed)
				{
					ma_audio_buffer_uninit(&soundData->audioBuffer);
				}
			}

			miniaudioSoundData.clear();
			ma_device_uninit(&device);
			ma_engine_uninit(&engine);

			isEngineInitialised = false;
		}

		Backend::Shutdown(forReset);
	}

	// ========================
	// Decoder Management
	// ========================

	const FRANAUDIO_API FranAudioShared::Containers::Vector<FranAudio::Decoder::DecoderType>& miniaudio::GetSupportedDecoders() const
	{
		static const FranAudioShared::Containers::Vector<FranAudio::Decoder::DecoderType> supportedDecoders =
		{
			FranAudio::Decoder::DecoderType::miniaudio,
			FranAudio::Decoder::DecoderType::libnyquist,
		};

		return supportedDecoders;
	}

	// ========================
	// Listener (3D Audio)
	// ========================

	FRANAUDIO_API void miniaudio::SetListenerTransform(const FranAudioShared::Vector3& position, const FranAudioShared::Vector3& forward, const FranAudioShared::Vector3& up)
	{
		SetListenerPosition(position);
		SetListenerOrientation(forward, up);
	}

	FRANAUDIO_API FranAudioShared::ListenerTransform miniaudio::GetListenerTransform()
	{
		const FranAudioShared::ListenerOrientation orientation = GetListenerOrientation();
		return { GetListenerPosition(), orientation.forward, orientation.up };
	}

	FRANAUDIO_API void miniaudio::SetListenerPosition(const FranAudioShared::Vector3& position)
	{
		ma_engine_listener_set_position(&engine, 0, position.x, position.y, position.z);
	}

	FRANAUDIO_API FranAudioShared::Vector3 miniaudio::GetListenerPosition()
	{
		ma_vec3f result = ma_engine_listener_get_position(&engine, 0);
		return { result.x, result.y, result.z };
	}

	FRANAUDIO_API void miniaudio::SetListenerOrientation(const FranAudioShared::Vector3& forward, const FranAudioShared::Vector3& up)
	{
		ma_engine_listener_set_direction(&engine, 0, forward.x, forward.y, forward.z);
		ma_engine_listener_set_world_up(&engine, 0, up.x, up.y, up.z);
	}

	FRANAUDIO_API FranAudioShared::ListenerOrientation miniaudio::GetListenerOrientation()
	{
		ma_vec3f fwd = ma_engine_listener_get_direction(&engine, 0);
		ma_vec3f u = ma_engine_listener_get_world_up(&engine, 0);
		return { { fwd.x, fwd.y, fwd.z }, { u.x, u.y, u.z } };
	}

	FRANAUDIO_API void miniaudio::SetMasterVolume(float volume)
	{
		ma_engine_set_volume(&engine, volume);
	}

	FRANAUDIO_API float miniaudio::GetMasterVolume()
	{
		return ma_engine_get_volume(&engine);
	}

	// ========================
	// Audio File Management
	// ========================

	FRANAUDIO_API size_t miniaudio::PlayAudioWave(const FranAudio::Sound::WaveData& waveData)
	{
		auto miniaudioSound = std::make_unique<MiniaudioSound>();

		std::visit([&waveData, &miniaudioSound](auto&& formattedWaveData)
		{
			miniaudioSound->audioBufferConfig = ma_audio_buffer_config_init(ConvertFormat(waveData.GetFormat()), waveData.GetChannels(), waveData.SizeInFrames(), formattedWaveData.data(), nullptr);
		}, waveData.GetFrames());

	
		miniaudioSound->audioBufferConfig.sampleRate = waveData.GetSampleRate(); // Why is this not set in the config init function?
		ma_audio_buffer_init(&miniaudioSound->audioBufferConfig, &miniaudioSound->audioBuffer);
		ma_sound_init_from_data_source(&engine, &miniaudioSound->audioBuffer, 0, nullptr, &miniaudioSound->sound);

		// Generate our unique ID
		const size_t soundID = nextSoundID++;

		activeSounds[soundID] = FranAudio::Sound::Sound(soundID, waveData.GetWaveDataID());
		ma_sound_set_volume(&miniaudioSound->sound, 1.0f);
		ma_sound_start(&miniaudioSound->sound);

		ma_sound_set_attenuation_model(&miniaudioSound->sound, ma_attenuation_model_inverse);

		ma_sound_set_min_distance(&miniaudioSound->sound, FranAudioShared::defaultSoundAttenuation.minDistance); // Non-attenuated distance. Within this distance, the sound is at full volume.
		ma_sound_set_max_distance(&miniaudioSound->sound, FranAudioShared::defaultSoundAttenuation.maxDistance); // Distance at which attenuation stops changing
		ma_sound_set_rolloff(&miniaudioSound->sound, FranAudioShared::defaultSoundAttenuation.rolloffFactor); // How fast it fades after the min distance

		miniaudioSoundData[soundID] = std::move(miniaudioSound);

		return soundID;
	}

	FRANAUDIO_API size_t miniaudio::LoadAudioFile(const std::string& filename)
	{
		return Backend::LoadAudioFile(filename);
	}

	FRANAUDIO_API size_t miniaudio::LoadAudioFile(const std::string& filename, const FranAudio::Decoder::DecodeSettings& decodeSettings)
	{
		return Backend::LoadAudioFile(filename, decodeSettings);
	}

	FRANAUDIO_API size_t miniaudio::PlayAudioFile(const std::string& filename)
	{
		return Backend::PlayAudioFile(filename);
	}

	FRANAUDIO_API size_t miniaudio::PlayAudioFileStream(const std::string& filename, bool looping)
	{
		auto miniaudioSound = std::make_unique<MiniaudioSound>();

		if (ma_sound_init_from_file(&engine, filename.c_str(), MA_SOUND_FLAG_STREAM, nullptr, nullptr, &miniaudioSound->sound) != MA_SUCCESS)
		{
			FranAudioShared::Logger::LogError(std::format("MiniAudio: Failed to open audio file for streaming: {}", filename));
			return SIZE_MAX;
		}

		miniaudioSound->isStreamed = true;

		// Generate our unique ID
		const size_t soundID = nextSoundID++;

		// Streamed sounds are not in the wave data cache, so they have no wave data index.
		activeSounds[soundID] = FranAudio::Sound::Sound(soundID, SIZE_MAX);

		ma_sound_set_volume(&miniaudioSound->sound, 1.0f);
		ma_sound_set_looping(&miniaudioSound->sound, looping ? MA_TRUE : MA_FALSE);

		ma_sound_set_attenuation_model(&miniaudioSound->sound, ma_attenuation_model_inverse);

		ma_sound_set_min_distance(&miniaudioSound->sound, FranAudioShared::defaultSoundAttenuation.minDistance); // Non-attenuated distance. Within this distance, the sound is at full volume.
		ma_sound_set_max_distance(&miniaudioSound->sound, FranAudioShared::defaultSoundAttenuation.maxDistance); // Distance at which attenuation stops changing
		ma_sound_set_rolloff(&miniaudioSound->sound, FranAudioShared::defaultSoundAttenuation.rolloffFactor); // How fast it fades after the min distance

		ma_sound_start(&miniaudioSound->sound);

		miniaudioSoundData[soundID] = std::move(miniaudioSound);

		return soundID;
	}

	// ========================
	// Sound Management
	// ========================

	FRANAUDIO_API bool miniaudio::IsSoundValid(size_t soundID)
	{
		return Backend::IsSoundValid(soundID) && miniaudioSoundData.contains(soundID);
	}

	FRANAUDIO_API void miniaudio::StopPlayingSound(size_t soundID)
	{
		if (!IsSoundValid(soundID))
		{
			FranAudioShared::Logger::LogError("MiniAudio: Tried to stop an invalid sound.");
			return;
		}

		auto& soundPtr = miniaudioSoundData[soundID];

		if (!miniaudioSoundData[soundID]->isPaused)
		{
			ma_sound_stop(&soundPtr->sound);
		}

		ma_sound_uninit(&soundPtr->sound);
		if (!soundPtr->isStreamed)
		{
			ma_audio_buffer_uninit(&soundPtr->audioBuffer);
		}

		soundPtr.reset();
		miniaudioSoundData.erase(soundID);
		activeSounds.erase(soundID);
	}

	FRANAUDIO_API void miniaudio::SetSoundPaused(size_t soundID, bool isPaused)
	{
		if (!IsSoundValid(soundID))
		{
			FranAudioShared::Logger::LogError("MiniAudio: Tried to pause/resume an invalid sound.");
			return;
		}

		auto& activeSound = activeSounds[soundID];
		auto* maSoundData = &miniaudioSoundData[soundID]->sound;

		//if (IsSoundPaused(soundID) == isPaused)
		if(miniaudioSoundData[soundID]->isPaused == isPaused)
		{
			return;
		}

		if (isPaused)
		{
			miniaudioSoundData[soundID]->pausedTime = ma_sound_get_time_in_milliseconds(maSoundData);
			ma_sound_stop(maSoundData);
		}
		else
		{
			ma_sound_start(maSoundData);
			ma_sound_seek_to_second(maSoundData, miniaudioSoundData[soundID]->pausedTime);
			miniaudioSoundData[soundID]->pausedTime = 0;
		}

		miniaudioSoundData[soundID]->isPaused = isPaused;
	}

	FRANAUDIO_API bool miniaudio::IsSoundPaused(size_t soundID)
	{
		if (!IsSoundValid(soundID))
		{
			FranAudioShared::Logger::LogError("MiniAudio: Tried to check paused state of an invalid sound.");
			return false;
		}

		return miniaudioSoundData[soundID]->isPaused;
	}

	FRANAUDIO_API void miniaudio::SetSoundVolumeRaw(size_t soundID, float volume)
	{
		if (!IsSoundValid(soundID))
		{
			FranAudioShared::Logger::LogError("MiniAudio: Tried to set volume of an invalid sound.");
			return;
		}

		ma_sound_set_volume(&miniaudioSoundData[soundID]->sound, volume);
	}

	FRANAUDIO_API float miniaudio::GetSoundVolumeRaw(size_t soundID)
	{
		if (!IsSoundValid(soundID))
		{
			FranAudioShared::Logger::LogError("MiniAudio: Tried to get volume of an invalid sound.");
			return 0.0f;
		}

		return ma_sound_get_volume(&miniaudioSoundData[soundID]->sound);
	}

    FRANAUDIO_API void miniaudio::SetSoundPitch(size_t soundID, float pitch)
    {
		if (!IsSoundValid(soundID))
		{
			FranAudioShared::Logger::LogError("MiniAudio: Tried to set pitch of an invalid sound.");
			return;
		}

		ma_sound_set_pitch(&miniaudioSoundData[soundID]->sound, pitch);
    }

	FRANAUDIO_API float miniaudio::GetSoundPitch(size_t soundID)
	{
		if (!IsSoundValid(soundID))
		{
			FranAudioShared::Logger::LogError("MiniAudio: Tried to get pitch of an invalid sound.");
			return 0.0f;
		}

		return ma_sound_get_pitch(&miniaudioSoundData[soundID]->sound);
	}

	FRANAUDIO_API void miniaudio::SetSoundLooping(size_t soundID, bool looping)
	{
		if (!IsSoundValid(soundID))
		{
			FranAudioShared::Logger::LogError("MiniAudio: Tried to set looping of an invalid sound.");
			return;
		}

		ma_sound_set_looping(&miniaudioSoundData[soundID]->sound, looping ? MA_TRUE : MA_FALSE);
	}

	FRANAUDIO_API bool miniaudio::IsSoundLooping(size_t soundID)
	{
		if (!IsSoundValid(soundID))
		{
			FranAudioShared::Logger::LogError("MiniAudio: Tried to check looping of an invalid sound.");
			return false;
		}

		return ma_sound_is_looping(&miniaudioSoundData[soundID]->sound) == MA_TRUE;
	}

	FRANAUDIO_API void miniaudio::SetSoundPosition(size_t soundID, const FranAudioShared::Vector3& position)
	{
		if (!IsSoundValid(soundID))
		{
			FranAudioShared::Logger::LogError("MiniAudio: Tried to set position of an invalid sound.");
			return;
		}

		ma_sound_set_position(&miniaudioSoundData[soundID]->sound, position.x, position.y, position.z);
	}

	FRANAUDIO_API FranAudioShared::Vector3 miniaudio::GetSoundPosition(size_t soundID)
	{
		if (!IsSoundValid(soundID))
		{
			FranAudioShared::Logger::LogError("MiniAudio: Tried to get position of an invalid sound.");
			return {};
		}

		ma_vec3f pos = ma_sound_get_position(&miniaudioSoundData[soundID]->sound);
		return { pos.x, pos.y, pos.z };
	}

	FRANAUDIO_API void miniaudio::SetSoundAttenuation(size_t soundID, float rolloffFactor, float minDistance, float maxDistance)
	{
		if (!IsSoundValid(soundID))
		{
			FranAudioShared::Logger::LogError("MiniAudio: Tried to set attenuation of an invalid sound.");
			return;
		}

		auto& sound = miniaudioSoundData[soundID]->sound;
		//ma_sound_set_attenuation_model(&sound, ma_attenuation_model_inverse);
		ma_sound_set_rolloff(&sound, rolloffFactor);
		ma_sound_set_min_distance(&sound, minDistance);
		ma_sound_set_max_distance(&sound, maxDistance);
	}

	FRANAUDIO_API FranAudioShared::SoundAttenuation miniaudio::GetSoundAttenuation(size_t soundID)
	{
		if (!IsSoundValid(soundID))
		{
			FranAudioShared::Logger::LogError("MiniAudio: Tried to get attenuation of an invalid sound.");
			return {};
		}

		auto& sound = miniaudioSoundData[soundID]->sound;
		return { ma_sound_get_rolloff(&sound), ma_sound_get_min_distance(&sound), ma_sound_get_max_distance(&sound) };
	}

	// ========================
	// Miniaudio Specific
	// ========================

	ma_decoder_config* miniaudio::GetDefaultDecoderConfig()
	{
		return &defaultDecoderConfig;
	}

	FranAudio::Sound::WaveFormat miniaudio::ConvertFormat(ma_format format)
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

	ma_format miniaudio::ConvertFormat(Sound::WaveFormat format)
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
}
