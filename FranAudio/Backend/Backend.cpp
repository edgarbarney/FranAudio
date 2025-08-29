// FranticDreamer 2022-2025

#include "Backend.hpp"
#include "miniaudio/Backend_miniaudio.hpp"

#include "FranAudioShared/Logger/Logger.hpp"

FranAudio::Backend::Backend::~Backend()
{
	//Shutdown();
	DestroyDecoder();
}

constexpr FranAudio::Backend::BackendType FranAudio::Backend::Backend::GetBackendType() const noexcept
{
	return BackendType::None;
}

// ========================
// Decoder Management
// ========================

FranAudio::Decoder::DecoderType FranAudio::Backend::Backend::GetDecoderType() const
{
	return currentDecoderType;
}

FranAudio::Decoder::Decoder* FranAudio::Backend::Backend::GetCurrentDecoder() const
{
	return currentDecoder;
}

void FranAudio::Backend::Backend::SetDecoder(FranAudio::Decoder::DecoderType decoderType, bool force)
{
	if (currentDecoderType == decoderType && !force)
	{
		return; // No need to change the decoder
	}

	if (currentDecoder != nullptr)
	{
		currentDecoder->Shutdown();
		delete currentDecoder;
		currentDecoder = nullptr;
	}

	// Check if the decoder type is supported
	const auto& supportedDecoders = GetSupportedDecoders();

	for (const auto& supportedDecoder : supportedDecoders)
	{
		if (supportedDecoder == decoderType)
		{
			currentDecoder = FranAudio::Decoder::Decoder::CreateDecoder(decoderType);
			currentDecoderType = decoderType;
			FranAudioShared::Logger::LogMessage(std::format("{}: Initialised decoder type {}", FranAudio::Backend::BackendTypeNames[(size_t)GetBackendType()], FranAudio::Decoder::DecoderTypeNames[(size_t)decoderType]));
			return;
		}
	}

	if (currentDecoder == nullptr)
	{
		FranAudioShared::Logger::LogError(std::format("{}: Decoder type not supported", FranAudio::Backend::BackendTypeNames[(size_t)GetBackendType()]));
		return;
	}
}

void FranAudio::Backend::Backend::DestroyDecoder()
{
	if (currentDecoder != nullptr)
	{
		currentDecoder->Shutdown();
		delete currentDecoder;
		currentDecoder = nullptr;
	}
}

void FranAudio::Backend::Backend::SetForcedDecodeFormat(FranAudio::Sound::WaveFormat format)
{
	forcedFormat = format;
}

FranAudio::Sound::WaveFormat FranAudio::Backend::Backend::GetForcedDecodeFormat() const
{
	return forcedFormat;
}

void FranAudio::Backend::Backend::SetForcedDecodeChannels(char channels)
{
	forcedChannels = channels;
}

char FranAudio::Backend::Backend::GetForcedDecodeChannels() const
{
	return forcedChannels;
}

void FranAudio::Backend::Backend::SetForcedDecodeSampleRate(int sampleRate)
{
	forcedSampleRate = sampleRate;
}

int FranAudio::Backend::Backend::GetForcedDecodeSampleRate() const
{
	return forcedSampleRate;
}

// ========================
// Sound Management
// ========================

bool FranAudio::Backend::Backend::IsSoundValid(size_t soundIndex)
{
	if (soundIndex == SIZE_MAX)
	{
		return false;
	}

	return activeSounds.contains(soundIndex);
}

FranAudio::Sound::Sound& FranAudio::Backend::Backend::GetSound(size_t soundID)
{
	return activeSounds[soundID];
} 

const FranAudioShared::Containers::UnorderedMap<size_t, FranAudio::Sound::Sound>& FranAudio::Backend::Backend::GetActiveSounds() const
{
	return activeSounds;
}

const std::vector<size_t> FranAudio::Backend::Backend::GetActiveSoundIDs() const
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

FranAudio::Backend::Backend* FranAudio::Backend::Backend::CreateBackend(BackendType backendType)
{
	Backend* newBackend = nullptr;

	switch (backendType)
	{
	case BackendType::miniaudio:
		newBackend = new FranAudio::Backend::miniaudio();
		break;
	case BackendType::OpenALSoft:
		//newBackend = OpenALSoft();
		//break;
	default:
		return nullptr;
		break;
	}

	if (!newBackend->Init())
	{
		delete newBackend;
		return nullptr;
	}

	return newBackend;
}