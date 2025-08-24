// FranticDreamer 2022-2025
#pragma once

namespace FranAudio::Decoder
{
	/// <summary>
	/// Possible decoder types.
	/// </summary>
	enum class DecoderType
	{
		None = 0,
		miniaudio,
		libnyquist,
	};
}
