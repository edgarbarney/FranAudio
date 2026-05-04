// FranticDreamer 2022-2025

#include <array>

#include "Backend_OpenALSoft.hpp"

#include "FranAudioShared/Logger/Logger.hpp"

constexpr const float zeroArr[3] = { 0.0f, 0.0f, 0.0f }; // Empty position

namespace FranAudio::Backend
{
	FRANAUDIO_API bool OpenALSoft::Init(FranAudio::Decoder::DecoderType decoderType)
	{
		defaultDecodeSettings = { false, 1 }; // Mono by default for 3D audio. OpenALSoft can't do stereo 3D audio.
		SetDecodeSettings(defaultDecodeSettings);

		openALDevice = alcOpenDevice(nullptr);
		if (!openALDevice)
		{
			FranAudioShared::Logger::LogError(std::format("{}: Init - open device error!!", GetBackendName()));
			return false;
		}

		ALCcontext* mainContext = alcCreateContext(openALDevice, nullptr);

		if (!mainContext)
		{
			FranAudioShared::Logger::LogError(std::format("{}: Init - create context error!!", GetBackendName()));
			return false;
		}

		if (!alcMakeContextCurrent(mainContext) || alcGetError(openALDevice) != ALC_NO_ERROR)
		{
			FranAudioShared::Logger::LogError(std::format("{}: Init - make context current error!!", GetBackendName()));
			return false;
		}

		al_isFloatSupported = alIsExtensionPresent("AL_EXT_float32") == AL_TRUE;
		al_isDoubleSupported = alIsExtensionPresent("AL_EXT_double") == AL_TRUE;
		alc_isHRTFSupported = alcIsExtensionPresent(openALDevice, "ALC_SOFT_HRTF") == AL_TRUE;

		alListenerf(AL_GAIN, 1.0f);
		alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);

		return Backend::Init();
	}

	FRANAUDIO_API void OpenALSoft::Reset()
	{
		Backend::Reset();
	}

	FRANAUDIO_API void OpenALSoft::Shutdown(bool forReset)
	{
		if (mainContext)
		{
			alcMakeContextCurrent(nullptr);
			ALCErrorCheck(openALDevice);
			alcDestroyContext(mainContext);
			ALCErrorCheck(openALDevice);

			mainContext = nullptr;
		}
		if (openALDevice)
		{
			alcCloseDevice(openALDevice);
			//ALCErrorCheck(openALDevice);

			openALDevice = nullptr;
		}

		Backend::Shutdown(forReset);
	}

	// ========================
	// Decoder Management
	// ========================

	const FRANAUDIO_API FranAudioShared::Containers::Vector<FranAudio::Decoder::DecoderType>& OpenALSoft::GetSupportedDecoders() const
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

	FRANAUDIO_API void OpenALSoft::SetListenerTransform(const float position[3], const float forward[3], const float up[3])
	{
		//float orientationCache[6];

		positionCache[0] = position[0];
		positionCache[1] = position[1];
		positionCache[2] = position[2];
		alListenerfv(AL_POSITION, position);
		ALErrorCheck();

		orientationCache[0] = forward[0];
		orientationCache[1] = forward[1];
		orientationCache[2] = forward[2];
		orientationCache[3] = up[0];
		orientationCache[4] = up[1];
		orientationCache[5] = up[2];

		alListenerfv(AL_ORIENTATION, orientationCache);
		ALErrorCheck();
	}

	FRANAUDIO_API void OpenALSoft::GetListenerTransform(float outPosition[3], float outForward[3], float outUp[3])
	{
		/*
		float orientation_vec[6];

		alGetListenerfv(AL_POSITION, position);

		alGetListenerfv(AL_ORIENTATION, orientation_vec);
		forward[0] = orientation_vec[0];
		forward[1] = orientation_vec[1];
		forward[2] = orientation_vec[2];
		up[0] = orientation_vec[3];
		up[1] = orientation_vec[4];
		up[2] = orientation_vec[5];
		*/

		std::copy(std::begin(positionCache), std::end(positionCache), outPosition);
		std::copy(std::begin(orientationCache), std::begin(orientationCache) + 3, outForward);
		std::copy(std::begin(orientationCache) + 3, std::end(orientationCache), outUp);
	}

	FRANAUDIO_API void OpenALSoft::SetListenerPosition(const float position[3])
	{
		positionCache[0] = position[0];
		positionCache[1] = position[1];
		positionCache[2] = position[2];
		alListenerfv(AL_POSITION, position);
		ALErrorCheck();
	}

	FRANAUDIO_API void OpenALSoft::GetListenerPosition(float outPosition[3])
	{
		//alGetListenerfv(AL_POSITION, position);
		std::copy(std::begin(positionCache), std::end(positionCache), outPosition);
	}

	FRANAUDIO_API void OpenALSoft::SetListenerOrientation(const float forward[3], const float up[3])
	{
		orientationCache[0] = forward[0];
		orientationCache[1] = forward[1];
		orientationCache[2] = forward[2];
		orientationCache[3] = up[0];
		orientationCache[4] = up[1];
		orientationCache[5] = up[2];

		alListenerfv(AL_ORIENTATION, orientationCache);
		ALErrorCheck();
	}

	FRANAUDIO_API void OpenALSoft::GetListenerOrientation(float outForward[3], float outUp[3])
	{
		/*
		float orientation_vec[6];

		alGetListenerfv(AL_ORIENTATION, orientation_vec);
		ALErrorCheck();
		forward[0] = orientation_vec[0];
		forward[1] = orientation_vec[1];
		forward[2] = orientation_vec[2];
		up[0] = orientation_vec[3];
		up[1] = orientation_vec[4];
		up[2] = orientation_vec[5];
		*/

		std::copy(std::begin(orientationCache), std::begin(orientationCache) + 3, outForward);
		std::copy(std::begin(orientationCache) + 3, std::end(orientationCache), outUp);
	}

	FRANAUDIO_API void OpenALSoft::SetMasterVolume(float volume)
	{
		alListenerf(AL_GAIN, volume);
		ALErrorCheck();
	}

	FRANAUDIO_API float OpenALSoft::GetMasterVolume()
	{
		float vol;
		alGetListenerf(AL_GAIN, &vol);
		ALErrorCheck();
		return vol;
	}

	// ========================
	// Audio File Management
	// ========================

	FRANAUDIO_API size_t OpenALSoft::PlayAudioWave(const FranAudio::Sound::WaveData& waveData)
	{
		auto openalSound = std::make_unique<OpenALSound>();

		alGenBuffers(1, &(openalSound->sourceBuffer));
		std::visit([&waveData, &openalSound](auto&& formattedWaveData)
		{
			alBufferData(openalSound->sourceBuffer, ConvertWaveFormat(waveData.GetFormat(), waveData.GetChannels()), formattedWaveData.data(), static_cast<ALsizei>(formattedWaveData.size() * sizeof(formattedWaveData[0])), waveData.GetSampleRate());
		}, waveData.GetFrames());

	
		alGenSources(1, &(openalSound->sourceHandle));

		alSourcef(openalSound->sourceHandle, AL_PITCH, 1.0f);
		alSourcef(openalSound->sourceHandle, AL_GAIN, 1.0f);

		alSourcei(openalSound->sourceHandle, AL_SOURCE_RELATIVE, false);

		alSourcefv(openalSound->sourceHandle, AL_POSITION, zeroArr);
		alSourcefv(openalSound->sourceHandle, AL_VELOCITY, zeroArr);
		alSourcei(openalSound->sourceHandle, AL_LOOPING, AL_FALSE);
		alSourcei(openalSound->sourceHandle, AL_BUFFER, openalSound->sourceBuffer);

		alSourcef(openalSound->sourceHandle, AL_REFERENCE_DISTANCE, 0.02f); // Non-attenuated distance. Within this distance, the sound is at full volume.
		alSourcef(openalSound->sourceHandle, AL_MAX_DISTANCE, 50.0f); // Distance at which attenuation stops changing
		alSourcef(openalSound->sourceHandle, AL_ROLLOFF_FACTOR, 1.0f); // How fast it fades after the reference distance

		alSourcePlay(openalSound->sourceHandle);

		ALErrorCheck();

		openalSound->sourceState = AL_PLAYING;

		// Generate our unique ID
		const size_t soundID = nextSoundID++;
		activeSounds[soundID] = FranAudio::Sound::Sound(soundID, waveData.GetWaveDataIndex());
		openalSoundData[soundID] = std::move(openalSound);

		return soundID;
	}

	FRANAUDIO_API size_t OpenALSoft::LoadAudioFile(const std::string& filename)
	{
		return Backend::LoadAudioFile(filename);
	}

	FRANAUDIO_API size_t OpenALSoft::LoadAudioFile(const std::string& filename, const FranAudio::Decoder::DecodeSettings& decodeSettings)
	{
		return Backend::LoadAudioFile(filename, decodeSettings);
	}

	FRANAUDIO_API size_t OpenALSoft::PlayAudioFile(const std::string& filename)
	{
		return Backend::PlayAudioFile(filename);
	}

	// ========================
	// Sound Management
	// ========================

	FRANAUDIO_API bool OpenALSoft::IsSoundValid(size_t soundID)
	{
		return Backend::IsSoundValid(soundID) && openalSoundData.contains(soundID);
	}

	FRANAUDIO_API void OpenALSoft::StopPlayingSound(size_t soundID)
	{
		if (!IsSoundValid(soundID))
		{
			FranAudioShared::Logger::LogError(std::format("{}: Tried to stop an invalid sound.", GetBackendName()));
			return;
		}

		alSourceStop(openalSoundData[soundID]->sourceHandle);
		ALErrorCheck();
		alDeleteSources(1, &(openalSoundData[soundID]->sourceHandle));
		ALErrorCheck();
		alDeleteBuffers(1, &(openalSoundData[soundID]->sourceBuffer));
		ALErrorCheck();

		openalSoundData.erase(soundID);
		activeSounds.erase(soundID);
	}

	FRANAUDIO_API void OpenALSoft::SetSoundPaused(size_t soundID, bool isPaused)
	{
		if (!IsSoundValid(soundID))
		{
			FranAudioShared::Logger::LogError(std::format("{}: Tried to pause/resume an invalid sound.", GetBackendName()));
			return;
		}

		alGetSourcei(openalSoundData[soundID]->sourceHandle, AL_SOURCE_STATE, &openalSoundData[soundID]->sourceState);
		ALErrorCheck();

		if (isPaused && openalSoundData[soundID]->sourceState == AL_PLAYING)
		{
			alSourcePause(openalSoundData[soundID]->sourceHandle);
			ALErrorCheck();
			openalSoundData[soundID]->sourceState = AL_PAUSED;
		}
		else if (!isPaused && openalSoundData[soundID]->sourceState == AL_PAUSED)
		{
			alSourcePlay(openalSoundData[soundID]->sourceHandle);
			ALErrorCheck();
			openalSoundData[soundID]->sourceState = AL_PLAYING;
		}
	}

	FRANAUDIO_API bool OpenALSoft::IsSoundPaused(size_t soundID)
	{
		if (!IsSoundValid(soundID))
		{
			FranAudioShared::Logger::LogError(std::format("{}: Tried to check pause state of an invalid sound.", GetBackendName()));
			return false;
		}

		return openalSoundData[soundID]->sourceState == AL_PAUSED;
	}

	FRANAUDIO_API void OpenALSoft::SetSoundVolume(size_t soundID, float volume)
	{
		if (!IsSoundValid(soundID))
		{
			FranAudioShared::Logger::LogError(std::format("{}: Tried to set volume of an invalid sound.", GetBackendName()));
			return;
		}

		alSourcef(openalSoundData[soundID]->sourceHandle, AL_GAIN, volume);
		ALErrorCheck();
	}

	FRANAUDIO_API float OpenALSoft::GetSoundVolume(size_t soundID)
	{
		if (!IsSoundValid(soundID))
		{
			FranAudioShared::Logger::LogError(std::format("{}: Tried to get volume of an invalid sound.", GetBackendName()));
			return 0.0f;
		}

		float vol;
		alGetSourcef(openalSoundData[soundID]->sourceHandle, AL_GAIN, &vol);
		ALErrorCheck();
		return vol;
	}

    FRANAUDIO_API void OpenALSoft::SetSoundPitch(size_t soundID, float pitch)
    {
		if (!IsSoundValid(soundID))
		{
			FranAudioShared::Logger::LogError(std::format("{}: Tried to set pitch of an invalid sound.", GetBackendName()));
			return;
		}

		alSourcef(openalSoundData[soundID]->sourceHandle, AL_PITCH, pitch);
		ALErrorCheck();
    }

	FRANAUDIO_API float OpenALSoft::GetSoundPitch(size_t soundID)
	{
		if (!IsSoundValid(soundID))
		{
			FranAudioShared::Logger::LogError(std::format("{}: Tried to get pitch of an invalid sound.", GetBackendName()));
			return 0.0f;
		}

		float pitch;
		alGetSourcef(openalSoundData[soundID]->sourceHandle, AL_PITCH, &pitch);
		ALErrorCheck();
		return pitch;
	}

	FRANAUDIO_API void OpenALSoft::SetSoundPosition(size_t soundID, const float position[3])
	{
		if (!IsSoundValid(soundID))
		{
			FranAudioShared::Logger::LogError(std::format("{}: Tried to set position of an invalid sound.", GetBackendName()));
			return;
		}

		alSourcefv(openalSoundData[soundID]->sourceHandle, AL_POSITION, position);
		ALErrorCheck();
	}

	FRANAUDIO_API void OpenALSoft::GetSoundPosition(size_t soundID, float outPosition[3])
	{
		if (!IsSoundValid(soundID))
		{
			FranAudioShared::Logger::LogError(std::format("{}: Tried to get position of an invalid sound.", GetBackendName()));
			return;
		}

		alGetSourcefv(openalSoundData[soundID]->sourceHandle, AL_POSITION, outPosition);
		ALErrorCheck();
	}

	FRANAUDIO_API void OpenALSoft::SetSoundAttenuation(size_t soundID, float rolloffFactor, float minDistance, float maxDistance)
	{
		if (!IsSoundValid(soundID))
		{
			FranAudioShared::Logger::LogError(std::format("{}: Tried to set attenuation of an invalid sound.", GetBackendName()));
			return;
		}

		alSourcef(openalSoundData[soundID]->sourceHandle, AL_ROLLOFF_FACTOR, rolloffFactor);
		ALErrorCheck();
		alSourcef(openalSoundData[soundID]->sourceHandle, AL_REFERENCE_DISTANCE, minDistance);
		ALErrorCheck();
		alSourcef(openalSoundData[soundID]->sourceHandle, AL_MAX_DISTANCE, maxDistance);
		ALErrorCheck();
	}


	FRANAUDIO_API void OpenALSoft::GetSoundAttenuation(size_t soundID, float& outRolloffFactor, float& outMinDistance, float& outMaxDistance)
	{
		if (!IsSoundValid(soundID))
		{
			FranAudioShared::Logger::LogError(std::format("{}: Tried to get attenuation of an invalid sound.", GetBackendName()));
			return;
		}

		alGetSourcef(openalSoundData[soundID]->sourceHandle, AL_ROLLOFF_FACTOR, &outRolloffFactor);
		ALErrorCheck();
		alGetSourcef(openalSoundData[soundID]->sourceHandle, AL_REFERENCE_DISTANCE, &outMinDistance);
		ALErrorCheck();
		alGetSourcef(openalSoundData[soundID]->sourceHandle, AL_MAX_DISTANCE, &outMaxDistance);
		ALErrorCheck();
	}

	// ========================
	// OpenALSoft Specific
	// ========================

	void OpenALSoft::ALErrorCheck()
	{
		ALenum error = alGetError();
		if (error != AL_NO_ERROR)
		{
			FranAudioShared::Logger::LogError(std::format("{}: AL Error : \"{}\"", GetBackendName(), alGetString(error)));
		}
	}

	void OpenALSoft::ALCErrorCheck(ALCdevice* device)
	{
		ALCenum error = alcGetError(device);
		if (alcGetError(device) != ALC_NO_ERROR)
		{
			FranAudioShared::Logger::LogError(std::format("{}: ALC Error : \"{}\"", GetBackendName(), alcGetString(device, error)));
		}
	}

	void OpenALSoft::ConvertWaveFormat(ALenum format, FranAudio::Sound::WaveFormat& outFormat, char& outChannel)
	{
		switch (format)
		{
		case AL_FORMAT_MONO8:
			outFormat = FranAudio::Sound::WaveFormat::PCM_8bit;
			outChannel = 1;
			break;
		case AL_FORMAT_MONO16:
			outFormat = FranAudio::Sound::WaveFormat::PCM_16bit;
			outChannel = 1;
			break;
		case AL_FORMAT_STEREO8:
			outFormat = FranAudio::Sound::WaveFormat::PCM_8bit;
			outChannel = 2;
			break;
		case AL_FORMAT_STEREO16:
			outFormat = FranAudio::Sound::WaveFormat::PCM_16bit;
			outChannel = 2;
			break;
		case AL_FORMAT_MONO_FLOAT32:
			outFormat = FranAudio::Sound::WaveFormat::IEEE_FLOAT;
			outChannel = 1;
			break;
		case AL_FORMAT_STEREO_FLOAT32:
			outFormat = FranAudio::Sound::WaveFormat::IEEE_FLOAT;
			outChannel = 2;
			break;
		case AL_FORMAT_MONO_DOUBLE_EXT:
			outFormat = FranAudio::Sound::WaveFormat::IEEE_DOUBLE;
			outChannel = 1;
			break;
		case AL_FORMAT_STEREO_DOUBLE_EXT:
			outFormat = FranAudio::Sound::WaveFormat::IEEE_DOUBLE;
			outChannel = 2;
			break;
		default:
			outFormat = FranAudio::Sound::WaveFormat::Unknown;
			outChannel = 0;
			break;
		}
	}

	ALenum OpenALSoft::ConvertWaveFormat(FranAudio::Sound::WaveFormat format, char channels)
	{
		switch (format)
		{
		case FranAudio::Sound::WaveFormat::PCM_8bit:
			if (channels == 1)
				return AL_FORMAT_MONO8;
			else if (channels == 2)
				return AL_FORMAT_STEREO8;
			break;
		case FranAudio::Sound::WaveFormat::PCM_16bit:
			if (channels == 1)
				return AL_FORMAT_MONO16;
			else if (channels == 2)
				return AL_FORMAT_STEREO16;
			break;
		case FranAudio::Sound::WaveFormat::PCM_24bit:
		case FranAudio::Sound::WaveFormat::IEEE_FLOAT:
			if (channels == 1 && al_isFloatSupported)
				return AL_FORMAT_MONO_FLOAT32;
			else if (channels == 2 && al_isFloatSupported)
				return AL_FORMAT_STEREO_FLOAT32;
			break;
		case FranAudio::Sound::WaveFormat::IEEE_DOUBLE:
			if (channels == 1 && al_isDoubleSupported)
				return AL_FORMAT_MONO_DOUBLE_EXT;
			else if (channels == 2 && al_isDoubleSupported)
				return AL_FORMAT_STEREO_DOUBLE_EXT;
			break;
		default:
			break;
		}
		return AL_NONE; // Invalid or unsupported format
	}
}
