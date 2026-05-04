// FranticDreamer 2022-2025

#include <iostream>
#include <filesystem>
#include <chrono>
#include <format>

#include "FranAudio.hpp"

#include "FranAudioServer.hpp"

#include "FranAudioShared/Network/Network.hpp"
#include "FranAudioShared/Serialisation/Serialisation.hpp"
#include "FranAudioShared/Logger/Logger.hpp"

namespace FranAudioServer
{
	void FranAudioServer::Init()
	{
		FranAudioShared::Logger::LogMessage("FranAudioServer::Init() Start");

		FranAudio::Init();

		FranAudioShared::Logger::LogMessage("FranAudioServer::Init() Done");
	}

	std::string FranAudioServer::Receive(const std::string& buffer)
	{
		//if (buffer == nullptr)
		//{
		//	FranAudioShared::Logger::LogError("Invalid buffer!");
		//	return {};
		//}

		if (buffer.size() == 0)
		{
			FranAudioShared::Logger::LogError("Empty buffer!");
			return {};
		}

		const auto& tempFunction = FranAudioShared::Network::NetworkFunction(buffer);

		auto it = functionsMap.find(tempFunction.functionName);
		if (it != functionsMap.end())
		{
	#if defined FRANAUDIO_SERVER_DEBUG && !defined FRANAUDIO_SERVER_DISABLE_LOGGING
			FranAudioShared::Logger::LogMessage(std::format("Executing function: {}", tempFunction.functionName));
	#endif
			return it->second(tempFunction);
		}
		else 
		{
			FranAudioShared::Logger::LogError(std::format("Function not found! {}", tempFunction.functionName));
			return {};
		}

	}

	const FranAudioShared::Containers::UnorderedMap<std::string, std::function<std::string(const FranAudioShared::Network::NetworkFunction&)>> FranAudioServer::functionsMap =
	{
		// Server::Init
		// Params: none
		// Returns: nothing
		{
			"server-init",
			[](const FranAudioShared::Network::NetworkFunction& fn)
			{
				Init();

				if (FranAudio::GetBackend() == nullptr)
				{
					FranAudioShared::Logger::LogError("Failed to initialise backend!");
					return std::string("err");
				}

				return std::string();
			}
		},

		// Server::SetBackend
		// Params: backendType
		// Returns: nothing
		{
			"server-set_backend",
			[](const FranAudioShared::Network::NetworkFunction& fn)
			{
				if (fn.params.size() < 1)
				{
					FranAudioShared::Logger::LogError("Missing parameters for set_backend");
					return std::string("err");
				}
				try
				{
					const auto backendType = static_cast<FranAudio::Backend::BackendType>(std::stoi(fn.params[0]));
					if (!FranAudio::SetBackend(backendType))
					{
						return std::string("err");
					}
				}
				catch (const std::exception& e)
				{
					FranAudioShared::Logger::LogError("Failed to set backend: {}", e.what());
					return std::string("err");
				}
				return std::string();
			}
		},

		// Backend::GetBackendType
		// Params: none
		// Returns: backendType
		{
			"backend-get_backend_type",
			[](const FranAudioShared::Network::NetworkFunction& fn)
			{
				if (FranAudio::GetBackend() == nullptr)
				{
					FranAudioShared::Logger::LogError("Backend is not initialised!");
					return std::string("err");
				}
				return std::to_string(static_cast<int>(FranAudio::GetBackend()->GetBackendType()));
			}
		},

		// Backend::GetBackendName
		// Params: none
		// Returns: backendName
		{
			"backend-get_backend_name",
			[](const FranAudioShared::Network::NetworkFunction& fn)
			{
				if (FranAudio::GetBackend() == nullptr)
				{
					FranAudioShared::Logger::LogError("Backend is not initialised!");
					return std::string("err");
				}
				return std::string(FranAudio::GetBackend()->GetBackendName());
			}
		},

		// ========================
		// Decoder Management
		// ========================

		// Backend::SetDecoder
		// Params: decoderType
		// Returns: nothing
		{
			"backend-set_decoder",
			[](const FranAudioShared::Network::NetworkFunction& fn)
			{
				if (fn.params.size() < 1)
				{
					FranAudioShared::Logger::LogError("Missing parameters for set_decoder");
					return std::string("err");
				}
				try
				{
					const auto decoderType = static_cast<FranAudio::Decoder::DecoderType>(std::stoi(fn.params[0]));
					if (!FranAudio::GetBackend()->SetDecoder(decoderType));
					{
						return std::string("err");
					}
				}
				catch (const std::exception& e)
				{
					FranAudioShared::Logger::LogError("Failed to set decoder: {}", e.what());
					return std::string("err");
				}
				return std::string();
			}
		},

		// Backend::GetDecoderType
		// Params: none
		// Returns: decoderType
		{
			"backend-get_decoder_type",
			[](const FranAudioShared::Network::NetworkFunction& fn)
			{
				if (FranAudio::GetBackend() == nullptr)
				{
					FranAudioShared::Logger::LogError("Backend is not initialised!");
					return std::string("err");
				}
				return std::to_string(static_cast<int>(FranAudio::GetBackend()->GetDecoderType()));
			}
		},

		// Backend::GetDecoderName
		// Params: none
		// Returns: decoderName
		{
			"backend-get_decoder_name",
			[](const FranAudioShared::Network::NetworkFunction& fn)
			{
				if (FranAudio::GetBackend() == nullptr)
				{
					FranAudioShared::Logger::LogError("Backend is not initialised!");
					return std::string("err");
				}
				return std::string(FranAudio::GetBackend()->GetDecoderName());
			}
		},

		// Backend::GetDefaultDecodeSettings
		// Params: none
		// Returns: serialized DecodeSettings
		{
			"backend-get_default_decode_settings",
			[](const FranAudioShared::Network::NetworkFunction& fn)
			{
				auto defaultSettings = FranAudio::GetBackend()->GetDefaultDecodeSettings();
				return FranAudioShared::Serialisation::BinarySerialiser::SerialiseToString(defaultSettings);
			}
		},

		// Backend::SetDecodeSettings
		// Params: serialized DecodeSettings
		// Returns: nothing
		{
			"backend-set_decode_settings",
			[](const FranAudioShared::Network::NetworkFunction& fn)
			{
				if (fn.params.size() < 1)
				{
					FranAudioShared::Logger::LogError("Missing parameters for set_decode_settings");
					return std::string("err");
				}
				try
				{
					auto settings = FranAudioShared::Serialisation::BinarySerialiser::DeserialiseFromString<FranAudio::Decoder::DecodeSettings>(fn.params[0]);
					FranAudio::GetBackend()->SetDecodeSettings(settings);
				}
				catch (const std::exception& e)
				{
					FranAudioShared::Logger::LogError("Failed to set decode settings: {}", e.what());
					return std::string("err");
				}
				return std::string();
			}
		},

		// Backend::GetDecodeSettings
		// Params: none
		// Returns: serialized DecodeSettings
		{
			"backend-get_decode_settings",
			[](const FranAudioShared::Network::NetworkFunction& fn)
			{
				auto settings = FranAudio::GetBackend()->GetDecodeSettings();
				return FranAudioShared::Serialisation::BinarySerialiser::SerialiseToString(settings);
			}
		},

		// ========================
		// Listener (3D Audio)
		// ========================

		// Backend::SetListenerTransform
		// Params: posX, posY, posZ, forwardX, forwardY, forwardZ, upX, upY, upZ
		// Returns: nothing
		{
			"backend-set_listener_transform",
			[](const FranAudioShared::Network::NetworkFunction& fn)
			{
				if (fn.params.size() < 9)
				{
					FranAudioShared::Logger::LogError("Missing parameters for set_listener_transform");
					return std::string("err");
				}

				try
				{
					const float position[3] = { std::stof(fn.params[0]), std::stof(fn.params[1]), std::stof(fn.params[2]) };
					const float forward[3] = { std::stof(fn.params[3]), std::stof(fn.params[4]), std::stof(fn.params[5]) };
					const float up[3] = { std::stof(fn.params[6]), std::stof(fn.params[7]), std::stof(fn.params[8]) };

					FranAudio::GetBackend()->SetListenerTransform(position, forward, up);
				}
				catch (const std::exception& e)
				{
					FranAudioShared::Logger::LogError("Failed to set master volume: {}", e.what());
					return std::string("err");
				}

				return std::string();
			}
		},

		// Backend::GetListenerTransform
		// Params: none
		// Returns: posX, posY, posZ, forwardX, forwardY, forwardZ, upX, upY, upZ
		{
			"backend-get_listener_transform",
			[](const FranAudioShared::Network::NetworkFunction& fn)
			{
				float position[3] = { 0.0f, 0.0f, 0.0f };
				float forward[3] = { 0.0f, 0.0f, 0.0f };
				float up[3] = { 0.0f, 0.0f, 0.0f };
				FranAudio::GetBackend()->GetListenerTransform(position, forward, up);

				return std::format
				(	"{}|{}|{}|{}|{}|{}|{}|{}|{}", 
					position[0], position[1], position[2],
					forward[0], forward[1], forward[2],
					up[0], up[1], up[2]
				);
			}
		},

		// Backend::SetListenerPosition
		// Params: posX, posY, posZ
		// Returns: nothing
		{
			"backend-set_listener_position",
			[](const FranAudioShared::Network::NetworkFunction& fn)
			{
				if (fn.params.size() < 3)
				{
					FranAudioShared::Logger::LogError("Missing parameters for set_listener_position");
					return std::string("err");
				}
				try
				{
					const float position[3] = { std::stof(fn.params[0]), std::stof(fn.params[1]), std::stof(fn.params[2]) };
					FranAudio::GetBackend()->SetListenerPosition(position);
				}
				catch (const std::exception& e)
				{
					FranAudioShared::Logger::LogError("Failed to set listener position: {}", e.what());
					return std::string("err");
				}
				return std::string();
			}
		},

		// Backend::GetListenerPosition
		// Params: none
		// Returns: posX, posY, posZ
		{
			"backend-get_listener_position",
			[](const FranAudioShared::Network::NetworkFunction& fn)
			{
				float position[3] = { 0.0f, 0.0f, 0.0f };
				FranAudio::GetBackend()->GetListenerPosition(position);

				return std::format("{}|{}|{}", position[0], position[1], position[2]);
			}
		},

		// Backend::SetListenerOrientation
		// Params: forwardX, forwardY, forwardZ, upX, upY, upZ
		// Returns: nothing
		{
			"backend-set_listener_orientation",
			[](const FranAudioShared::Network::NetworkFunction& fn)
			{
				if (fn.params.size() < 6)
				{
					FranAudioShared::Logger::LogError("Missing parameters for set_listener_orientation");
					return std::string("err");
				}
				try
				{
					const float forward[3] = { std::stof(fn.params[0]), std::stof(fn.params[1]), std::stof(fn.params[2]) };
					const float up[3] = { std::stof(fn.params[3]), std::stof(fn.params[4]), std::stof(fn.params[5]) };
					FranAudio::GetBackend()->SetListenerOrientation(forward, up);
				}
				catch (const std::exception& e)
				{
					FranAudioShared::Logger::LogError(std::format("Failed to set listener orientation: {}", e.what()));
					return std::string("err");
				}
				return std::string();
			}
		},

		// Backend::GetListenerOrientation
		// Params: none
		// Returns: forwardX, forwardY, forwardZ, upX, upY, upZ
		{
			"backend-get_listener_orientation",
			[](const FranAudioShared::Network::NetworkFunction& fn)
			{
				float forward[3] = { 0.0f, 0.0f, 0.0f };
				float up[3] = { 0.0f, 0.0f, 0.0f };
				FranAudio::GetBackend()->GetListenerOrientation(forward, up);

				return std::format("{}|{}|{}|{}|{}|{}", forward[0], forward[1], forward[2], up[0], up[1], up[2]);
			}
		},

		// Backend::SetMasterVolume
		// Params: volume
		// Returns: nothing
		{
			"backend-set_master_volume",
			[](const FranAudioShared::Network::NetworkFunction& fn)
			{
				if (fn.params.size() < 1)
				{
					FranAudioShared::Logger::LogError("Missing volume parameter for set_master_volume");
					return std::string("err");
				}

				try
				{
					FranAudio::GetBackend()->SetMasterVolume(std::stof(fn.params[0]));
				}
				catch (const std::exception& e)
				{
					FranAudioShared::Logger::LogError(std::format("Failed to set master volume: {}", e.what()));
					return std::string("err");
				}

				return std::string();
			}
		},

		// Backend::GetMasterVolume
		// Params: none
		// Returns: volume
		{
			"backend-get_master_volume",
			[](const FranAudioShared::Network::NetworkFunction& fn)
			{
				return std::to_string(FranAudio::GetBackend()->GetMasterVolume());
			}
		},

		// ========================
		// Audio File Management
		// ========================

		// Backend::LoadAudioFile
		// Params: filename
		// Returns: wave data index
		{
			"backend-load_audio_file",
			[](const FranAudioShared::Network::NetworkFunction& fn)
			{
				if (fn.params.size() < 1)
				{
					FranAudioShared::Logger::LogError("Missing filename parameter for load_audio_file");
					return std::string("err");
				}

				return std::to_string(FranAudio::GetBackend()->LoadAudioFile(fn.params[0]));
			}
		},

		// Backend::LoadAudioFile
		// Params: filename, serialized DecodeSettings
		// Returns: wave data index
		{
			"backend-load_audio_file_with_settings",
			[](const FranAudioShared::Network::NetworkFunction& fn)
			{
				if (fn.params.size() < 2)
				{
					FranAudioShared::Logger::LogError("Missing parameters for load_audio_file_with_settings");
					return std::string("err");
				}
				try
				{
					auto settings = FranAudioShared::Serialisation::BinarySerialiser::DeserialiseFromString<FranAudio::Decoder::DecodeSettings>(fn.params[1]);
					return std::to_string(FranAudio::GetBackend()->LoadAudioFile(fn.params[0], settings));
				}
				catch (const std::exception& e)
				{
					FranAudioShared::Logger::LogError("Failed to load audio file with settings: {}", e.what());
					return std::string("err");
				}
			}
		},

		// Backend::PlayAudioFile
		// Params: filename
		// Returns: sound index
		{
			"backend-play_audio_file", 
			[](const FranAudioShared::Network::NetworkFunction& fn)
			{
				if (fn.params.size() < 1) 
				{
					FranAudioShared::Logger::LogError("Missing filename parameter");
					return std::string("err");
				}

				return std::to_string(FranAudio::GetBackend()->PlayAudioFile(fn.params[0]));
			}
		},

		// Sound::GetActiveSoundIDs
		// Params: none
		// Returns: Active sound ids as a binary serialised vector
		{
			"backend-get_active_sound_ids",
			[](const FranAudioShared::Network::NetworkFunction& fn)
			{
				return FranAudio::GetBackend()->GetActiveSounds().size() < 1 ? std::string() : FranAudioShared::Serialisation::BinarySerialiser::SerialiseVector(FranAudio::GetBackend()->GetActiveSoundIDs());
			}
		},

		// ========================
		// Sound Management
		// ========================

		// Sound::IsValid
		// Params: soundIndex
		// Returns: "1" if valid, "0" if not valid, "err" on error
		{
			"sound-is_valid",
			[](const FranAudioShared::Network::NetworkFunction& fn)
			{
				if (fn.params.size() < 1)
				{
					FranAudioShared::Logger::LogError("Missing sound ID parameter");
					return std::string("err");
				}
				if (fn.params[0] == std::to_string(SIZE_MAX))
				{
					return std::string("0");
				}
				try
				{
					const size_t soundId = std::stoull(fn.params[0]);
					return FranAudio::GetBackend()->IsSoundValid(soundId) ? std::string("1") : std::string("0");
				}
				catch (const std::exception& e)
				{
					FranAudioShared::Logger::LogError(std::format("Failed to check if sound with ID {} is valid: {}", fn.params[0], e.what()));
					return std::string("err");
				}
			}
		},

		// Sound::Stop
		// Params: soundIndex
		// Returns: nothing
		{
			"sound-stop",
			[](const FranAudioShared::Network::NetworkFunction& fn)
			{
				if (fn.params.size() < 1)
				{
					FranAudioShared::Logger::LogError("Missing sound ID parameter");
					return std::string("err");
				}

				if (fn.params[0] == std::to_string(SIZE_MAX))
				{
					FranAudioShared::Logger::LogError("Tried to stop an invalid sound.");
					return std::string("err");
				}

				try
				{
					FranAudio::GetBackend()->StopPlayingSound(std::stoi(fn.params[0]));
				}
				catch (const std::exception& e)
				{
					FranAudioShared::Logger::LogError(std::format("Failed to stop sound with ID {}: {}", fn.params[0], e.what()));
					return std::string("err");
				}

				return std::string();
			}
		},

		// Sound::SetPaused
		// Params: soundIndex, isPaused (1 or 0)
		// Returns: nothing
		{
			"sound-set_paused",
			[](const FranAudioShared::Network::NetworkFunction& fn)
			{
				if (fn.params.size() < 2)
				{
					FranAudioShared::Logger::LogError("Missing parameters for sound-set_paused");
					return std::string("err");
				}
				if (fn.params[0] == std::to_string(SIZE_MAX))
				{
					FranAudioShared::Logger::LogError("Tried to set paused state of an invalid sound.");
					return std::string("err");
				}
				try
				{
					const size_t soundId = std::stoull(fn.params[0]);
					const bool isPaused = fn.params[1] == "1";
					FranAudio::GetBackend()->SetSoundPaused(soundId, isPaused);
				}
				catch (const std::exception& e)
				{
					FranAudioShared::Logger::LogError(std::format("Failed to set sound paused state: {}", e.what()));
					return std::string("err");
				}
				return std::string();
			}
		},

		// Sound::IsPaused
		// Params: soundIndex
		// Returns: "1" if paused, "0" if not paused, "err" on error
		{
			"sound-is_paused",
			[](const FranAudioShared::Network::NetworkFunction& fn)
			{
				if (fn.params.size() < 1)
				{
					FranAudioShared::Logger::LogError("Missing sound ID parameter");
					return std::string("err");
				}
				if (fn.params[0] == std::to_string(SIZE_MAX))
				{
					FranAudioShared::Logger::LogError("Tried to get paused state of an invalid sound.");
					return std::string("err");
				}
				try
				{
					const size_t soundId = std::stoull(fn.params[0]);
					return FranAudio::GetBackend()->IsSoundPaused(soundId) ? std::string("1") : std::string("0");
				}
				catch (const std::exception& e)
				{
					FranAudioShared::Logger::LogError(std::format("Failed to check if sound with ID {} is paused: {}", fn.params[0], e.what()));
					return std::string("err");
				}
			}
		},

		// Sound::SetVolume
		// Params: soundIndex, volume
		// Returns: nothing
		{
			"sound-set_volume",
			[](const FranAudioShared::Network::NetworkFunction& fn)
			{
				if (fn.params.size() < 2)
				{
					FranAudioShared::Logger::LogError("Missing parameters for sound-set_volume");
					return std::string("err");
				}
				if (fn.params[0] == std::to_string(SIZE_MAX))
				{
					FranAudioShared::Logger::LogError("Tried to set volume of an invalid sound.");
					return std::string("err");
				}
				try
				{
					const size_t soundId = std::stoull(fn.params[0]);
					const float volume = std::stof(fn.params[1]);
					FranAudio::GetBackend()->SetSoundVolume(soundId, volume);
				}
				catch (const std::exception& e)
				{
					FranAudioShared::Logger::LogError(std::format("Failed to set sound volume: {}", e.what()));
					return std::string("err");
				}
				return std::string();
			}
		},

		// Sound::GetVolume
		// Params: soundIndex
		// Returns: volume, or "err" on error
		{
			"sound-get_volume",
			[](const FranAudioShared::Network::NetworkFunction& fn)
			{
				if (fn.params.size() < 1)
				{
					FranAudioShared::Logger::LogError("Missing sound ID parameter");
					return std::string("err");
				}
				if (fn.params[0] == std::to_string(SIZE_MAX))
				{
					FranAudioShared::Logger::LogError("Tried to get volume of an invalid sound.");
					return std::string("err");
				}
				try
				{
					const size_t soundId = std::stoull(fn.params[0]);
					return std::to_string(FranAudio::GetBackend()->GetSoundVolume(soundId));
				}
				catch (const std::exception& e)
				{
					FranAudioShared::Logger::LogError(std::format("Failed to get volume of sound with ID {}: {}", fn.params[0], e.what()));
					return std::string("err");
				}
			}
		},

		// Sound::SetPitch
		// Params: soundIndex, pitch
		// Returns: nothing
		{
			"sound-set_pitch",
			[](const FranAudioShared::Network::NetworkFunction& fn)
			{
				if (fn.params.size() < 2)
				{
					FranAudioShared::Logger::LogError("Missing parameters for sound-set_pitch");
					return std::string("err");
				}
				if (fn.params[0] == std::to_string(SIZE_MAX))
				{
					FranAudioShared::Logger::LogError("Tried to set pitch of an invalid sound.");
					return std::string("err");
				}
				try
				{
					const size_t soundId = std::stoull(fn.params[0]);
					const float pitch = std::stof(fn.params[1]);
					FranAudio::GetBackend()->SetSoundPitch(soundId, pitch);
				}
				catch (const std::exception& e)
				{
					FranAudioShared::Logger::LogError(std::format("Failed to set sound pitch: {}", e.what()));
					return std::string("err");
				}
				return std::string();
			}
		},

		// Sound::GetPitch
		// Params: soundIndex
		// Returns: pitch, or "err" on error
		{
			"sound-get_pitch",
			[](const FranAudioShared::Network::NetworkFunction& fn)
			{
				if (fn.params.size() < 1)
				{
					FranAudioShared::Logger::LogError("Missing sound ID parameter");
					return std::string("err");
				}
				if (fn.params[0] == std::to_string(SIZE_MAX))
				{
					FranAudioShared::Logger::LogError("Tried to get pitch of an invalid sound.");
					return std::string("err");
				}
				try
				{
					const size_t soundId = std::stoull(fn.params[0]);
					return std::to_string(FranAudio::GetBackend()->GetSoundPitch(soundId));
				}
				catch (const std::exception& e)
				{
					FranAudioShared::Logger::LogError(std::format("Failed to get pitch of sound with ID {}: {}", fn.params[0], e.what()));
					return std::string("err");
				}
			}
		},

		// Sound::SetPosition
		// Params: soundIndex, posX, posY, posZ
		// Returns: nothing
		{
			"sound-set_position",
			[](const FranAudioShared::Network::NetworkFunction& fn)
			{
				if (fn.params.size() < 4)
				{
					FranAudioShared::Logger::LogError("Missing parameters for sound-set_position");
					return std::string("err");
				}
				try
				{
					const size_t soundId = std::stoull(fn.params[0]);
					const float position[3] = { std::stof(fn.params[1]), std::stof(fn.params[2]), std::stof(fn.params[3]) };
					FranAudio::GetBackend()->SetSoundPosition(soundId, position);
				}
				catch (const std::exception& e)
				{
					FranAudioShared::Logger::LogError(std::format("Failed to set sound position: {}", e.what()));
					return std::string("err");
				}
				return std::string();
			}
		},

		// Sound::GetPosition
		// Params: soundIndex
		// Returns: posX, posY, posZ, or "err" on error
		{
			"sound-get_position",
			[](const FranAudioShared::Network::NetworkFunction& fn)
			{
				if (fn.params.size() < 1)
				{
					FranAudioShared::Logger::LogError("Missing sound ID parameter");
					return std::string("err");
				}
				if (fn.params[0] == std::to_string(SIZE_MAX))
				{
					FranAudioShared::Logger::LogError("Tried to get position of an invalid sound.");
					return std::string("err");
				}
				try
				{
					const size_t soundId = std::stoull(fn.params[0]);
					float position[3] = { 0.0f, 0.0f, 0.0f };
					FranAudio::GetBackend()->GetSoundPosition(soundId, position);

					return std::format("{}|{}|{}", position[0], position[1], position[2]);
				}
				catch (const std::exception& e)
				{
					FranAudioShared::Logger::LogError(std::format("Failed to get position of sound with ID {}: {}", fn.params[0], e.what()));
					return std::string("err");
				}
			}
		},

		// Sound::SetAttenuation
		// Params: soundIndex, rolloffFactor, minDistance, maxDistance
		// Returns: nothing
		{
			"sound-set_attenuation",
			[](const FranAudioShared::Network::NetworkFunction& fn)
			{
				if (fn.params.size() < 4)
				{
					FranAudioShared::Logger::LogError("Missing parameters for sound-set_attenuation");
					return std::string("err");
				}
				try
				{
					const size_t soundId = std::stoull(fn.params[0]);
					const float rolloffFactor = std::stof(fn.params[1]);
					const float minDistance = std::stof(fn.params[2]);
					const float maxDistance = std::stof(fn.params[3]);
					FranAudio::GetBackend()->SetSoundAttenuation(soundId, rolloffFactor, minDistance, maxDistance);
				}
				catch (const std::exception& e)
				{
					FranAudioShared::Logger::LogError(std::format("Failed to set sound attenuation: {}", e.what()));
					return std::string("err");
				}
				return std::string();
			}
		},

		// Sound::GetAttenuation
		// Params: soundIndex
		// Returns: rolloffFactor, minDistance, maxDistance, or "err" on error
		{
			"sound-get_attenuation",
			[](const FranAudioShared::Network::NetworkFunction& fn)
			{
				if (fn.params.size() < 1)
				{
					FranAudioShared::Logger::LogError("Missing sound ID parameter");
					return std::string("err");
				}
				if (fn.params[0] == std::to_string(SIZE_MAX))
				{
					FranAudioShared::Logger::LogError("Tried to get attenuation of an invalid sound");
					return std::string("err");
				}
				try
				{
					const size_t soundId = std::stoull(fn.params[0]);
					float rolloffFactor = 0.0f;
					float minDistance = 0.0f;
					float maxDistance = 0.0f;
					FranAudio::GetBackend()->GetSoundAttenuation(soundId, rolloffFactor, minDistance, maxDistance);
					return std::format("{}|{}|{}", rolloffFactor, minDistance, maxDistance);
				}
				catch (const std::exception& e)
				{
					FranAudioShared::Logger::LogError(std::format("Failed to get attenuation of sound with ID {}: {}", fn.params[0], e.what()));
					return std::string("err");
				}
			}
		},
	};
}
