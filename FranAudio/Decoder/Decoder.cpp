// FranticDreamer 2022-2025

#include "Decoder.hpp"
#include "miniaudio/Decoder_miniaudio.hpp"
#include "libnyquist/Decoder_libnyquist.hpp"

namespace FranAudio::Decoder
{
	constexpr FRANAUDIO_API DecoderType Decoder::GetDecoderType() const noexcept
	{ 
		 return DecoderType::None;
	}

FRANAUDIO_API FranAudio::Decoder::Decoder* FranAudio::Decoder::Decoder::CreateDecoder(DecoderType decoderType)
{
	Decoder* newDecoder = nullptr;
	FRANAUDIO_API std::unique_ptr<Decoder> Decoder::CreateDecoder(DecoderType decoderType)
	{
		std::unique_ptr<Decoder> newDecoder = nullptr;

		switch (decoderType)
		{
		case DecoderType::miniaudio:
			newDecoder = std::make_unique<miniaudio>();
			break;
		case DecoderType::libnyquist:
			newDecoder = std::make_unique<libnyquist>();
			break;
		default:
			return nullptr;
			break;
		}

		if (!newDecoder->Init())
		{
			return nullptr;
		}

		return newDecoder;
	}

}
