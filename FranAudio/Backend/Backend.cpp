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

FranAudio::Decoder::DecoderType FranAudio::Backend::Backend::GetDecoderType() const
{
	return currentDecoderType;
}

FranAudio::Decoder::Decoder* FranAudio::Backend::Backend::GetCurrentDecoder() const
{
	return currentDecoder;
}

void FranAudio::Backend::Backend::SetDecoder(FranAudio::Decoder::DecoderType decoderType)
{
	if (currentDecoderType == decoderType)
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
			return;
		}
	}

	if (currentDecoder == nullptr)
	{
		FranAudioShared::Logger::LogError("FranAudio: Decoder type not supported");
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