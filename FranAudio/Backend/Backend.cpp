// FranticDreamer 2022-2025

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

	constexpr FRANAUDIO_API BackendType Backend::GetBackendType() const noexcept
	{
		return BackendType::None;
	}

	// ========================
	// Decoder Management
	// ========================

	FRANAUDIO_API FranAudio::Decoder::DecoderType Backend::GetDecoderType() const
	{
		return currentDecoderType;
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
				FranAudioShared::Logger::LogMessage(std::format("{}: Initialised decoder type {}", BackendTypeNames[(size_t)GetBackendType()], FranAudio::Decoder::DecoderTypeNames[(size_t)decoderType]));
				return;
			}
		}

		if (currentDecoder == nullptr)
		{
			FranAudioShared::Logger::LogError(std::format("{}: Decoder type not supported", BackendTypeNames[(size_t)GetBackendType()]));
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

	FRANAUDIO_API void Backend::SetForcedDecodeFormat(FranAudio::Sound::WaveFormat format)
	{
		forcedFormat = format;
	}

	FRANAUDIO_API FranAudio::Sound::WaveFormat Backend::GetForcedDecodeFormat() const
	{
		return forcedFormat;
	}

	FRANAUDIO_API void Backend::SetForcedDecodeChannels(char channels)
	{
		forcedChannels = channels;
	}

	FRANAUDIO_API char Backend::GetForcedDecodeChannels() const
	{
		return forcedChannels;
	}

	FRANAUDIO_API void Backend::SetForcedDecodeSampleRate(int sampleRate)
	{
		forcedSampleRate = sampleRate;
	}

	FRANAUDIO_API int Backend::GetForcedDecodeSampleRate() const
	{
		return forcedSampleRate;
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
