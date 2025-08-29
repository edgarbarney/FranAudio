// FranticDreamer 2022-2025

#include "FranAudioShared/Serialisation/Serialisation.hpp"
#include "FranAudioShared/Logger/Logger.hpp"

#include "FranAudioClient.hpp"


FRANAUDIO_CLIENT_API void FranAudioClient::RouteClientLoggingToConsole(FranAudioShared::Logger::ConsoleStreamBuffer* consoleBuffer)
{
	FranAudioShared::Logger::RouteToConsole(consoleBuffer);
}

namespace FranAudioClient::Wrapper
{

	FRANAUDIO_CLIENT_API void SetBackend(FranAudio::Backend::BackendType backendType)
	{

	}
	
	namespace Backend
	{
		FRANAUDIO_CLIENT_API FranAudio::Backend::BackendType FranAudioClient::Wrapper::Backend::GetBackendType()
		{
			auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-get_backend_type", {}));
			try
			{
				return static_cast<FranAudio::Backend::BackendType>(std::stoi(response));
			}
			catch (const std::exception& e)
			{
				FranAudioShared::Logger::LogError("Failed to get backend type from server!");
				return FranAudio::Backend::BackendType::None;
			}
			
		}

		// ========================
		// Decoder Management
		// ========================

		FRANAUDIO_CLIENT_API void SetDecoder(FranAudio::Decoder::DecoderType decoderType)
		{
			FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-set_decoder", { std::to_string(static_cast<int>(decoderType)) }));
		}

		FRANAUDIO_CLIENT_API FranAudio::Decoder::DecoderType GetDecoderType()
		{
			auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-get_decoder_type", {}));
			try
			{
				return static_cast<FranAudio::Decoder::DecoderType>(std::stoi(response));
			}
			catch (const std::exception& e)
			{
				FranAudioShared::Logger::LogError("Failed to get decoder type from server!");
				return FranAudio::Decoder::DecoderType::None;
			}
		}

		FRANAUDIO_CLIENT_API void SetForcedDecodeFormat(FranAudio::Sound::WaveFormat format)
		{
			FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-set_forced_decode_format", { std::to_string(static_cast<int>(format)) }));
		}

		FRANAUDIO_CLIENT_API FranAudio::Sound::WaveFormat GetForcedDecodeFormat()
		{
			auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-get_forced_decode_format", {}));
			try
			{
				return static_cast<FranAudio::Sound::WaveFormat>(std::stoi(response));
			}
			catch (const std::exception& e)
			{
				FranAudioShared::Logger::LogError("Failed to get forced decode format from server!");
				return FranAudio::Sound::WaveFormat::Unknown;
			}
		}

		FRANAUDIO_CLIENT_API void SetForcedDecodeChannels(char channels)
		{
			FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-set_forced_decode_channels", { std::to_string(static_cast<int>(channels)) }));
		}

		FRANAUDIO_CLIENT_API char GetForcedDecodeChannels()
		{
			auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-get_forced_decode_channels", {}));
			try
			{
				return static_cast<char>(std::stoi(response));
			}
			catch (const std::exception& e)
			{
				FranAudioShared::Logger::LogError("Failed to get forced decode channels from server!");
				return 0;
			}
		}

		FRANAUDIO_CLIENT_API void SetForcedDecodeSampleRate(int sampleRate)
		{
			FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-set_forced_decode_sample_rate", { std::to_string(sampleRate) }));
		}

		FRANAUDIO_CLIENT_API int GetForcedDecodeSampleRate()
		{
			auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-get_forced_decode_sample_rate", {}));
			try
			{
				return std::stoi(response);
			}
			catch (const std::exception& e)
			{
				FranAudioShared::Logger::LogError("Failed to get forced decode sample rate from server!");
				return 0;
			}
		}

		// ========================
		// Listener (3D Audio)
		// ========================
	
		FRANAUDIO_CLIENT_API void SetListenerTransform(float position[3], float forward[3], float up[3])
		{
			FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-set_listener_transform", {	std::to_string(position[0]), std::to_string(position[1]), std::to_string(position[2]),
																												std::to_string(forward[0]), std::to_string(forward[1]), std::to_string(forward[2]),
																												std::to_string(up[0]), std::to_string(up[1]), std::to_string(up[2]) }));
		}

		FRANAUDIO_CLIENT_API void GetListenerTransform(float position[3], float forward[3], float up[3])
		{
			auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-get_listener_transform", {}));
			try
			{
				auto params = FranAudioShared::Network::NetworkFunction::ParseFunction(response).params;
				if (params.size() < 9)
				{
					FranAudioShared::Logger::LogError("Invalid response from server for get_listener_transform");
					return;
				}
				position[0] = std::stof(params[0]);
				position[1] = std::stof(params[1]);
				position[2] = std::stof(params[2]);
				forward[0] = std::stof(params[3]);
				forward[1] = std::stof(params[4]);
				forward[2] = std::stof(params[5]);
				up[0] = std::stof(params[6]);
				up[1] = std::stof(params[7]);
				up[2] = std::stof(params[8]);
			}
			catch (const std::exception& e)
			{
				FranAudioShared::Logger::LogError("Failed to get listener transform from server!");
				return;
			}
		}

		FRANAUDIO_CLIENT_API void SetListenerPosition(const float position[3])
		{
			FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-set_listener_position", { std::to_string(position[0]), std::to_string(position[1]), std::to_string(position[2]) }));
		}

		FRANAUDIO_CLIENT_API void GetListenerPosition(float position[3])
		{
			auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-get_listener_position", {}));
			try
			{
				auto params = FranAudioShared::Network::NetworkFunction::ParseFunction(response).params;
				if (params.size() < 3)
				{
					FranAudioShared::Logger::LogError("Invalid response from server for get_listener_position");
					return;
				}
				position[0] = std::stof(params[0]);
				position[1] = std::stof(params[1]);
				position[2] = std::stof(params[2]);
			}
			catch (const std::exception& e)
			{
				FranAudioShared::Logger::LogError("Failed to get listener position from server!");
				return;
			}
		}

		FRANAUDIO_CLIENT_API void SetListenerOrientation(const float forward[3], const float up[3])
		{
			FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-set_listener_orientation", { std::to_string(forward[0]), std::to_string(forward[1]), std::to_string(forward[2]),
																												  std::to_string(up[0]), std::to_string(up[1]), std::to_string(up[2]) }));
		}

		FRANAUDIO_CLIENT_API void GetListenerOrientation(float forward[3], float up[3])
		{
			auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-get_listener_orientation", {}));
			try
			{
				auto params = FranAudioShared::Network::NetworkFunction::ParseFunction(response).params;
				if (params.size() < 6)
				{
					FranAudioShared::Logger::LogError("Invalid response from server for get_listener_orientation");
					return;
				}
				forward[0] = std::stof(params[0]);
				forward[1] = std::stof(params[1]);
				forward[2] = std::stof(params[2]);
				up[0] = std::stof(params[3]);
				up[1] = std::stof(params[4]);
				up[2] = std::stof(params[5]);
			}
			catch (const std::exception& e)
			{
				FranAudioShared::Logger::LogError("Failed to get listener orientation from server!");
				return;
			}
		}

		FRANAUDIO_CLIENT_API void SetMasterVolume(float volume)
		{
			FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-set_master_volume", { std::to_string(volume) }));
			
		}

		FRANAUDIO_CLIENT_API float GetMasterVolume()
		{
			auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-get_master_volume", {}));
			try
			{
				return std::stof(response);
			}
			catch (const std::exception& e)
			{
				FranAudioShared::Logger::LogError("Failed to get master volume from server!");
				return 0.0f;
			}
		}

		// ========================
		// Audio File Management
		// ========================

		FRANAUDIO_CLIENT_API size_t LoadAudioFile(const std::string& filename)
		{
			auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-load_audio_file", { filename }));
			try
			{
				return std::stoull(response);
			}
			catch (const std::exception& e)
			{
				FranAudioShared::Logger::LogError("Failed to load audio file: " + filename);
				return SIZE_MAX;
			}
		}

		FRANAUDIO_CLIENT_API size_t PlayAudioFile(const std::string& filename)
		{
			auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-play_audio_file", { filename }));
			try
			{
				return std::stoull(response);
			}
			catch (const std::exception& e)
			{
				FranAudioShared::Logger::LogError("Failed to play audio file stream: " + filename);
				return SIZE_MAX;
			}
		}

		FRANAUDIO_CLIENT_API size_t PlayAudioFileStream(const std::string& filename)
		{
			auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-play_audio_file_stream", { filename }));
			try
			{
				return std::stoull(response);
			}
			catch (const std::exception& e)
			{
				FranAudioShared::Logger::LogError("Failed to play audio file stream: " + filename);
				return SIZE_MAX;
			}
		}
		
		// ========================
		// Macro Sound Management
		// ========================

		FRANAUDIO_CLIENT_API const std::vector<size_t> GetActiveSoundIDs()
		{
			std::string response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-get_active_sound_ids", {}));
			return FranAudioShared::Serialisation::BinarySerialiser::DeserialiseVector<size_t>(response);
		}
	}

	namespace Sound
	{
		// ========================
		// Sound Management
		// ========================

		FRANAUDIO_CLIENT_API bool IsValid(size_t soundIndex)
		{
			auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("sound-is_valid", { std::to_string(soundIndex) }));
			return response == "1";
		}

		FRANAUDIO_CLIENT_API void Stop(size_t soundIndex)
		{
			FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("sound-stop", { std::to_string(soundIndex) }));
		}

		FRANAUDIO_CLIENT_API void SetPaused(size_t soundID, bool isPaused)
		{
			FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("sound-set_paused", { std::to_string(soundID), isPaused ? "1" : "0" }));
		}

		FRANAUDIO_CLIENT_API bool IsPaused(size_t soundID)
		{
			return FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("sound-is_paused", { std::to_string(soundID) })) != "0";
		}

		FRANAUDIO_CLIENT_API void SetVolume(size_t soundIndex, float volume)
		{
			FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("sound-set_volume", { std::to_string(soundIndex), std::to_string(volume) }));
		}

		FRANAUDIO_CLIENT_API float GetVolume(size_t soundIndex)
		{
			auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("sound-get_volume", { std::to_string(soundIndex) }));
			try
			{
				return std::stof(response);
			}
			catch (const std::exception& e)
			{
				FranAudioShared::Logger::LogError("Failed to get volume for sound index: " + std::to_string(soundIndex));
				return 0.0f;
			}
		}

		FRANAUDIO_CLIENT_API void SetPosition(size_t soundIndex, float position[3])
		{
			FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("sound-set_position", { std::to_string(soundIndex), std::to_string(position[0]), std::to_string(position[1]), std::to_string(position[2]) }));
		}

		FRANAUDIO_CLIENT_API void GetPosition(size_t soundIndex, float position[3])
		{
			auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("sound-get_position", { std::to_string(soundIndex) }));
			try
			{
				auto params = FranAudioShared::Network::NetworkFunction::ParseFunction(response).params;
				if (params.size() < 3)
				{
					FranAudioShared::Logger::LogError("Invalid response from server for get_position of sound index: " + std::to_string(soundIndex));
					return;
				}
				position[0] = std::stof(params[0]);
				position[1] = std::stof(params[1]);
				position[2] = std::stof(params[2]);
			}
			catch (const std::exception& e)
			{
				FranAudioShared::Logger::LogError("Failed to get position for sound index: " + std::to_string(soundIndex));
				return;
			}
		}
	}
}