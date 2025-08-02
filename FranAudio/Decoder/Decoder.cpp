// FranticDreamer 2022-2025

#include "Decoder.hpp"
#include "miniaudio/Decoder_miniaudio.hpp"
#include "libnyquist/Decoder_libnyquist.hpp"

FranAudio::Decoder::Decoder* FranAudio::Decoder::Decoder::CreateDecoder(DecoderType decoderType)
{
	Decoder* newDecoder = nullptr;

	switch (decoderType)
	{
	case DecoderType::miniaudio:
		newDecoder = new FranAudio::Decoder::miniaudio();
		break;
	case DecoderType::libnyquist:
		newDecoder = new FranAudio::Decoder::libnyquist();
		break;
	default:
		return nullptr;
		break;
	}

	if (!newDecoder->Init())
	{
		delete newDecoder;
		return nullptr;
	}

	return newDecoder;
}
