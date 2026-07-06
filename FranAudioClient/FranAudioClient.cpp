// FranticDreamer 2022-2025

#include "FranAudioShared/Network/Network.hpp"
#include "FranAudioShared/Serialisation/Serialisation.hpp"
#include "FranAudioShared/Logger/Logger.hpp"

#include "FranAudioClient.hpp"


FRANAUDIO_CLIENT_API void FranAudioClient::RouteClientLoggingToConsole(FranAudioShared::Logger::ConsoleStreamBuffer* consoleBuffer)
{
	FranAudioShared::Logger::RouteToConsole(consoleBuffer);
}

namespace FranAudioClient::Wrapper
{

	FRANAUDIO_CLIENT_API bool SetBackend(FranAudio::Backend::BackendType backendType)
	{
		const auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("server-set_backend", { std::to_string(static_cast<int>(backendType)) }));
		return response != "err";
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

		FRANAUDIO_CLIENT_API std::string FranAudioClient::Wrapper::Backend::GetBackendName()
		{
			return FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-get_backend_name", {}));
		}

		// ========================
		// Decoder Management
		// ========================

		FRANAUDIO_CLIENT_API bool SetDecoder(FranAudio::Decoder::DecoderType decoderType)
		{
			const auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-set_decoder", { std::to_string(static_cast<int>(decoderType)) }));
			return response != "err";
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

		FRANAUDIO_CLIENT_API std::string GetDecoderName()
		{
			return FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-get_decoder_name", {}));
		}

		FRANAUDIO_CLIENT_API const FranAudio::Decoder::DecodeSettings GetDefaultDecodeSettings()
		{
			auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-get_default_decode_settings", {}));
			try
			{
				return FranAudioShared::Serialisation::BinarySerialiser::DeserialiseFromString<FranAudio::Decoder::DecodeSettings>(response);
			}
			catch (const std::exception& e)
			{
				FranAudioShared::Logger::LogError("Failed to get default decode settings from server!");
				static FranAudio::Decoder::DecodeSettings defaultSettings;
				return defaultSettings;
			}
		}

		FRANAUDIO_CLIENT_API void SetDecodeSettings(const FranAudio::Decoder::DecodeSettings& settings)
		{
			auto serializedSettings = FranAudioShared::Serialisation::BinarySerialiser::SerialiseToString(settings);
			FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-set_decode_settings", { serializedSettings }));
		}

		FRANAUDIO_CLIENT_API const FranAudio::Decoder::DecodeSettings GetDecodeSettings()
		{
			auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-get_decode_settings", {}));
			try
			{
				return FranAudioShared::Serialisation::BinarySerialiser::DeserialiseFromString<FranAudio::Decoder::DecodeSettings>(response);
			}
			catch (const std::exception& e)
			{
				FranAudioShared::Logger::LogError("Failed to get decode settings from server!");
				return {};
			}
		}

		// ========================
		// Listener (3D Audio)
		// ========================
	
		FRANAUDIO_CLIENT_API void SetListenerTransform(float position[3], float forward[3], float up[3])
		{
			FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-set_listener_transform", 
			{
				std::to_string(position[0]),
				std::to_string(position[1]),
				std::to_string(position[2]),
				std::to_string(forward[0]),
				std::to_string(forward[1]),
				std::to_string(forward[2]),
				std::to_string(up[0]),
				std::to_string(up[1]),
				std::to_string(up[2])
			}));
		}

		FRANAUDIO_CLIENT_API void GetListenerTransform(float outPosition[3], float outForward[3], float outUp[3])
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
				outPosition[0] = std::stof(params[0]);
				outPosition[1] = std::stof(params[1]);
				outPosition[2] = std::stof(params[2]);
				outForward[0] = std::stof(params[3]);
				outForward[1] = std::stof(params[4]);
				outForward[2] = std::stof(params[5]);
				outUp[0] = std::stof(params[6]);
				outUp[1] = std::stof(params[7]);
				outUp[2] = std::stof(params[8]);
			}
			catch (const std::exception& e)
			{
				FranAudioShared::Logger::LogError("Failed to get listener transform from server!");
				return;
			}
		}

		FRANAUDIO_CLIENT_API void SetListenerPosition(const float position[3])
		{
			FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-set_listener_position", 
			{
					std::to_string(position[0]), 
					std::to_string(position[1]), 
					std::to_string(position[2])
			}));
		}

		FRANAUDIO_CLIENT_API void GetListenerPosition(float outPosition[3])
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
				outPosition[0] = std::stof(params[0]);
				outPosition[1] = std::stof(params[1]);
				outPosition[2] = std::stof(params[2]);
			}
			catch (const std::exception& e)
			{
				FranAudioShared::Logger::LogError("Failed to get listener position from server!");
				return;
			}
		}

		FRANAUDIO_CLIENT_API void SetListenerOrientation(const float forward[3], const float up[3])
		{
			FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-set_listener_orientation", 
			{ 
				std::to_string(forward[0]), 
				std::to_string(forward[1]), 
				std::to_string(forward[2]),
				std::to_string(up[0]), 
				std::to_string(up[1]), 
				std::to_string(up[2]) 
			}));
		}

		FRANAUDIO_CLIENT_API void GetListenerOrientation(float outForward[3], float outUp[3])
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
				outForward[0] = std::stof(params[0]);
				outForward[1] = std::stof(params[1]);
				outForward[2] = std::stof(params[2]);
				outUp[0] = std::stof(params[3]);
				outUp[1] = std::stof(params[4]);
				outUp[2] = std::stof(params[5]);
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
				FranAudioShared::Logger::LogError(std::format("Failed to load audio file: {}", filename));
				return SIZE_MAX;
			}
		}

		FRANAUDIO_CLIENT_API size_t LoadAudioFile(const std::string& filename, const FranAudio::Decoder::DecodeSettings& decodeSettings)
		{
			std::string buffer = FranAudioShared::Serialisation::BinarySerialiser::SerialiseToString<FranAudio::Decoder::DecodeSettings>(decodeSettings);
			auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-load_audio_file_with_settings",{ filename, buffer }));
			try
			{
				return std::stoull(response);
			}
			catch (const std::exception& e)
			{
				FranAudioShared::Logger::LogError(std::format("Failed to load audio file with settings: {}", filename));
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
				FranAudioShared::Logger::LogError(std::format("Failed to play audio file: {}", filename));
				return SIZE_MAX;
			}
		}
		
		// ========================
		// Macro Sound Management
		// ========================

		FRANAUDIO_CLIENT_API const FranAudioShared::Containers::Vector<size_t> GetActiveSoundIDs()
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
				FranAudioShared::Logger::LogError(std::format("Failed to get volume for sound index: {}", std::to_string(soundIndex)));
				return 0.0f;
			}
		}

        FRANAUDIO_CLIENT_API void SetPitch(size_t soundID, float pitch)
        {
			FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("sound-set_pitch", { std::to_string(soundID), std::to_string(pitch) }));
        }

		FRANAUDIO_CLIENT_API float GetPitch(size_t soundID)
		{
			auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("sound-get_pitch", { std::to_string(soundID) }));
			try
			{
				return std::stof(response);
			}
			catch (const std::exception& e)
			{
				FranAudioShared::Logger::LogError(std::format("Failed to get pitch for sound index: {}", std::to_string(soundID)));
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
					FranAudioShared::Logger::LogError(std::format("Invalid response from server for get_position of sound index: {}", std::to_string(soundIndex)));
					return;
				}
				position[0] = std::stof(params[0]);
				position[1] = std::stof(params[1]);
				position[2] = std::stof(params[2]);
			}
			catch (const std::exception& e)
			{
				FranAudioShared::Logger::LogError(std::format("Failed to get position for sound index: {}", std::to_string(soundIndex)));
				return;
			}
		}

		FRANAUDIO_CLIENT_API void SetAttenuation(size_t soundID, float rolloffFactor, float minDistance, float maxDistance)
		{
			FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("sound-set_attenuation", 
			{ 
				std::to_string(soundID), 
				std::to_string(rolloffFactor), 
				std::to_string(minDistance), 
				std::to_string(maxDistance) 
			}));
		}

		FRANAUDIO_CLIENT_API void GetAttenuation(size_t soundID, float& outRolloffFactor, float& outMinDistance, float& outMaxDistance)
		{
			auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("sound-get_attenuation", { std::to_string(soundID) }));
			try
			{
				auto params = FranAudioShared::Network::NetworkFunction::ParseFunction(response).params;
				if (params.size() < 3)
				{
					FranAudioShared::Logger::LogError(std::format("Invalid response from server for get_attenuation of sound index: {}", std::to_string(soundID)));
					return;
				}
				outRolloffFactor = std::stof(params[0]);
				outMinDistance = std::stof(params[1]);
				outMaxDistance = std::stof(params[2]);
			}
			catch (const std::exception& e)
			{
				FranAudioShared::Logger::LogError(std::format("Failed to get attenuation for sound index: {}", std::to_string(soundID)));
				return;
			}
		}
	}
}