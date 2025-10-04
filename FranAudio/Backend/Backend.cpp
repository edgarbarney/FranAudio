// FranticDreamer 2022-2025

#include <filesystem>

#include "Backend.hpp"
#include "miniaudio/Backend_miniaudio.hpp"

#include "FranAudioShared/Logger/Logger.hpp"

namespace FranAudio::Backend
{
	FRANAUDIO_API Backend::~Backend()
	{
		//Shutdown();
		DestroyDecoder();
	}

	FRANAUDIO_API bool Backend::Init(FranAudio::Decoder::DecoderType decoderType)
	{
		bool decoderFail = false;
		SetDecodeSettings(GetDefaultDecodeSettings());

		if (decoderType == FranAudio::Decoder::DecoderType::None)
		{
			FranAudioShared::Logger::LogError(std::format("{}: No decoder type specified", GetBackendName()));
			decoderFail = true;
		}

		// Check if the requested decoder is supported
		const auto& supportedDecoders = GetSupportedDecoders();
		if (std::find(supportedDecoders.begin(), supportedDecoders.end(), decoderType) == supportedDecoders.end())
		{
			FranAudioShared::Logger::LogError(std::format("{}: Requested decoder is not supported by this backend", GetBackendName()));
			decoderFail = true;
		}

		if (decoderFail)
		{
			FranAudioShared::Logger::LogError(std::format("{}: Defaulting to miniaudio decoder", GetBackendName()));
			decoderType = FranAudio::Decoder::DecoderType::miniaudio;
		}

		// Decoder will be initialised by the FranAudio::Init
		currentDecoderType = decoderType;

		FranAudioShared::Logger::LogError(std::format("{}: Initialised Backend.", GetBackendName()));
		return true; // Default backend does nothing
	}

	FRANAUDIO_API void Backend::Reset()
	{
		Shutdown(true);
		currentDecoder->Reset();
		Init(currentDecoderType);
	}

	FRANAUDIO_API void Backend::Shutdown(bool forReset)
	{
		FranAudioShared::Logger::LogError(std::format("{}: Shutting down backend...", GetBackendName()));

		nextSoundID = 0;
		activeSounds.clear();

		if (!forReset)
		{
			DestroyDecoder();
		}

		FranAudioShared::Logger::LogError(std::format("{}: Backend shut down.", GetBackendName()));
	}

	constexpr FRANAUDIO_API BackendType Backend::GetBackendType() const noexcept
	{
		return BackendType::None;
	}

	constexpr FRANAUDIO_API const char* Backend::GetBackendName() const noexcept
	{
		return BackendTypeNames[(size_t)GetBackendType()];
	}

	// ========================
	// Decoder Management
	// ========================

	FRANAUDIO_API FranAudio::Decoder::DecoderType Backend::GetDecoderType() const
	{
		return currentDecoderType;
	}

	constexpr FRANAUDIO_API const char* Backend::GetDecoderName() const noexcept
	{
		return currentDecoder->GetDecoderName();
	}

	FRANAUDIO_API FranAudio::Decoder::Decoder* Backend::GetCurrentDecoder() const
	{
		return currentDecoder.get();
	}

	FRANAUDIO_API void Backend::SetDecoder(FranAudio::Decoder::DecoderType decoderType, bool force)
	{
		if (currentDecoderType == decoderType && !force)
		{
			return; // No need to change the decoder
		}

		if (currentDecoder != nullptr)
		{
			currentDecoder->Shutdown();
			currentDecoder.reset();
		}

		// Check if the decoder type is supported
		const auto& supportedDecoders = GetSupportedDecoders();

		for (const auto& supportedDecoder : supportedDecoders)
		{
			if (supportedDecoder == decoderType)
			{
				currentDecoder = FranAudio::Decoder::Decoder::CreateDecoder(decoderType);
				currentDecoderType = decoderType;
				FranAudioShared::Logger::LogMessage(std::format("{}: Initialised decoder type {}", GetBackendName(), GetDecoderName()));
				return;
			}
		}

		if (currentDecoder == nullptr)
		{
			FranAudioShared::Logger::LogError(std::format("{}: Decoder type not supported", GetBackendName()));
			return;
		}
	}

	FRANAUDIO_API void Backend::DestroyDecoder()
	{
		if (currentDecoder != nullptr)
		{
			currentDecoder->Shutdown();
			currentDecoder.reset();
		}
	}

	const FRANAUDIO_API FranAudio::Decoder::DecodeSettings& Backend::GetDefaultDecodeSettings() const noexcept
	{
		return defaultDecodeSettings;
	}

	FRANAUDIO_API void Backend::SetDecodeSettings(const FranAudio::Decoder::DecodeSettings& settings) noexcept
	{
		currentDecodeSettings = settings;
	}

	constexpr FRANAUDIO_API const FranAudio::Decoder::DecodeSettings& Backend::GetDecodeSettings() const noexcept
	{
		return currentDecodeSettings;
	}

	// ========================
	// Audio File Management
	// ========================

	FRANAUDIO_API size_t Backend::LoadAudioFile(const std::string& filename)
	{
		return LoadAudioFile(filename, currentDecodeSettings);
	}

	FRANAUDIO_API size_t Backend::LoadAudioFile(const std::string& filename, const FranAudio::Decoder::DecodeSettings& decodeSettings)
	{
		std::filesystem::path filePath(filename);

		if (!std::filesystem::exists(filePath))
		{
			FranAudioShared::Logger::LogError(std::format("{}: File does not exist: {}", GetBackendName(), filename));
			return SIZE_MAX;
		}

		if (std::filesystem::is_directory(filePath))
		{
			FranAudioShared::Logger::LogError(std::format("{}: File is a directory: {}", GetBackendName(), filename));
			return SIZE_MAX;
		}

		if (std::filesystem::is_empty(filePath))
		{
			FranAudioShared::Logger::LogError(std::format("{}: File is empty: {}", GetBackendName(), filename));
			return SIZE_MAX;
		}

		FranAudio::Sound::WaveData waveData;
		bool result = currentDecoder->DecodeAudioFile(filename, waveData, *this, decodeSettings);

		if (!result)
		{
			FranAudioShared::Logger::LogError(std::format("{}: Failed to decode audio file: {}", GetBackendName(), filename));
			return SIZE_MAX;
		}

		const size_t index = waveDataCache.size();
		waveData.SetWaveDataIndex(index);
		waveDataCache.emplace_back(waveData);
		filenameWaveMap[filename] = index;
		FranAudioShared::Logger::LogMessage(std::format("{}: Loaded audio file: {} ({}s, {} channels, {}Hz, Format: {})", GetBackendName(), filename, waveData.GetLength(), (int)waveData.GetChannels(), waveData.GetSampleRate(), FranAudio::Sound::WaveFormatNames[(size_t)waveData.GetFormat()]));

		return index;
	}

	FRANAUDIO_API size_t Backend::PlayAudioFile(const std::string& filename)
	{
		auto it = filenameWaveMap.find(filename); // Filename - Wave data cache index
		if (it == filenameWaveMap.end())
		{
			FranAudioShared::Logger::LogError(std::format("{}: Audio file not loaded: {}", GetBackendName(), filename));
			return SIZE_MAX;
		}

		const auto& waveData = waveDataCache[it->second];
		return PlayAudioWave(waveData);
	}

	// ========================
	// Sound Management
	// ========================

	FRANAUDIO_API bool Backend::IsSoundValid(size_t soundIndex)
	{
		if (soundIndex == SIZE_MAX)
		{
			return false;
		}

		return activeSounds.contains(soundIndex);
	}

	FRANAUDIO_API FranAudio::Sound::Sound& Backend::GetSound(size_t soundID)
	{
		return activeSounds[soundID];
	} 

	const FRANAUDIO_API FranAudioShared::Containers::UnorderedMap<size_t, FranAudio::Sound::Sound>& Backend::GetActiveSounds() const
	{
		return activeSounds;
	}

	const FRANAUDIO_API std::vector<size_t> Backend::GetActiveSoundIDs() const
	{
		// No need to reallocate every time
		static std::vector<size_t> soundIDs;

		soundIDs.clear();
		soundIDs.reserve(activeSounds.size());

		for (const auto& [soundID, sound] : activeSounds)
		{
			soundIDs.push_back(soundID);
		}

		return soundIDs;
	}

	FRANAUDIO_API std::unique_ptr<Backend> Backend::CreateBackend(BackendType backendType)
	{
		std::unique_ptr<Backend> newBackend = nullptr;

		switch (backendType)
		{
		case BackendType::miniaudio:
			newBackend = std::make_unique<FranAudio::Backend::miniaudio>();
			break;
		case BackendType::OpenALSoft:
			newBackend = std::make_unique<FranAudio::Backend::OpenALSoft>();
			break;
		default:
			return nullptr;
			break;
		}

		if (!newBackend->Init())
		{
			return nullptr;
		}

		return newBackend;
	}
}
