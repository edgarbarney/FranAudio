// FranticDreamer 2022-2025

#include <optional>
#include <array>
#include <algorithm>

#include "FranAudioShared/FranAudioShared.hpp"
#include "FranAudioShared/Network/Network.hpp"
#include "FranAudioShared/Serialisation/Serialisation.hpp"
#include "FranAudioShared/Logger/Logger.hpp"

#include "FranAudioClient.hpp"

namespace
{
	/// <summary>
	/// Client-side cache of the last-known server state.
	///
	/// Setters are fire-and-forget (no reply), so they record the value they sent here.
	/// Getters return the cached value when available and only do a blocking round trip
	/// on a cache miss. Cleared by Wrapper::ClearCache() on Init/Reconnect.
	/// </summary>
	struct CachedSoundState
	{
		std::optional<float> volume;
		std::optional<float> pitch;
		std::optional<bool> paused;
		std::optional<bool> looping;
		std::optional<std::array<float, 3>> position;
		std::optional<std::array<float, 3>> attenuation; // rolloffFactor, minDistance, maxDistance
		std::optional<std::string> group;
	};

	struct ClientCache
	{
		std::optional<float> masterVolume;
		std::optional<std::array<float, 3>> listenerPosition;
		std::optional<std::array<float, 6>> listenerOrientation; // forward (3), up (3)
		FranAudioShared::Containers::UnorderedMap<size_t, CachedSoundState> sounds;
		FranAudioShared::Containers::UnorderedMap<std::string, float> groupVolumes;
		FranAudioShared::Containers::UnorderedMap<std::string, bool> groupExclusive;
	};

	ClientCache cache;

	/// <summary>
	/// The server replies with "err" when a command fails.
	/// For getters, an empty reply is an error too (lost connection or timeout).
	/// </summary>
	bool IsErrorResponse(const std::string& response)
	{
		return response.empty() || response == "err";
	}

	/// <summary>
	/// Split a plain "a|b|c" server reply into its parts.
	/// Replies have no leading '$' or function name, so NetworkFunction::ParseFunction cannot parse them.
	/// </summary>
	FranAudioShared::Containers::Vector<std::string> SplitResponse(const std::string& response)
	{
		FranAudioShared::Containers::Vector<std::string> parts;

		size_t start = 0;
		while (start <= response.size())
		{
			const size_t end = response.find('|', start);
			if (end == std::string::npos)
			{
				parts.push_back(response.substr(start));
				break;
			}
			parts.push_back(response.substr(start, end - start));
			start = end + 1;
		}

		return parts;
	}

}


FRANAUDIO_CLIENT_API void FranAudioClient::RouteClientLoggingToConsole(FranAudioShared::Logger::ConsoleStreamBuffer* consoleBuffer)
{
	FranAudioShared::Logger::RouteToConsole(consoleBuffer);
}

namespace FranAudioClient::Wrapper
{
	FRANAUDIO_CLIENT_API void ClearCache()
	{
		cache = {};
	}

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
			FranAudioClient::SendNoReply(FranAudioShared::Network::NetworkFunction("backend-set_decode_settings", { serializedSettings }));
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
			cache.listenerPosition = { position[0], position[1], position[2] };
			cache.listenerOrientation = { forward[0], forward[1], forward[2], up[0], up[1], up[2] };

			FranAudioClient::SendNoReply(FranAudioShared::Network::NetworkFunction("backend-set_listener_transform",
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
			if (cache.listenerPosition && cache.listenerOrientation)
			{
				const auto& pos = *cache.listenerPosition;
				const auto& orientation = *cache.listenerOrientation;
				outPosition[0] = pos[0]; outPosition[1] = pos[1]; outPosition[2] = pos[2];
				outForward[0] = orientation[0]; outForward[1] = orientation[1]; outForward[2] = orientation[2];
				outUp[0] = orientation[3]; outUp[1] = orientation[4]; outUp[2] = orientation[5];
				return;
			}

			auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-get_listener_transform", {}));
			if (IsErrorResponse(response))
			{
				FranAudioShared::Logger::LogError("Server returned an error for get_listener_transform");
				return;
			}
			try
			{
				auto params = SplitResponse(response);
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

				cache.listenerPosition = { outPosition[0], outPosition[1], outPosition[2] };
				cache.listenerOrientation = { outForward[0], outForward[1], outForward[2], outUp[0], outUp[1], outUp[2] };
			}
			catch (const std::exception& e)
			{
				FranAudioShared::Logger::LogError("Failed to get listener transform from server!");
				return;
			}
		}

		FRANAUDIO_CLIENT_API void SetListenerPosition(const float position[3])
		{
			cache.listenerPosition = { position[0], position[1], position[2] };

			FranAudioClient::SendNoReply(FranAudioShared::Network::NetworkFunction("backend-set_listener_position",
			{
					std::to_string(position[0]), 
					std::to_string(position[1]), 
					std::to_string(position[2])
			}));
		}

		FRANAUDIO_CLIENT_API void GetListenerPosition(float outPosition[3])
		{
			if (cache.listenerPosition)
			{
				const auto& pos = *cache.listenerPosition;
				outPosition[0] = pos[0]; outPosition[1] = pos[1]; outPosition[2] = pos[2];
				return;
			}

			auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-get_listener_position", {}));
			if (IsErrorResponse(response))
			{
				FranAudioShared::Logger::LogError("Server returned an error for get_listener_position");
				return;
			}
			try
			{
				auto params = SplitResponse(response);
				if (params.size() < 3)
				{
					FranAudioShared::Logger::LogError("Invalid response from server for get_listener_position");
					return;
				}
				outPosition[0] = std::stof(params[0]);
				outPosition[1] = std::stof(params[1]);
				outPosition[2] = std::stof(params[2]);

				cache.listenerPosition = { outPosition[0], outPosition[1], outPosition[2] };
			}
			catch (const std::exception& e)
			{
				FranAudioShared::Logger::LogError("Failed to get listener position from server!");
				return;
			}
		}

		FRANAUDIO_CLIENT_API void SetListenerOrientation(const float forward[3], const float up[3])
		{
			cache.listenerOrientation = { forward[0], forward[1], forward[2], up[0], up[1], up[2] };

			FranAudioClient::SendNoReply(FranAudioShared::Network::NetworkFunction("backend-set_listener_orientation",
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
			if (cache.listenerOrientation)
			{
				const auto& orientation = *cache.listenerOrientation;
				outForward[0] = orientation[0]; outForward[1] = orientation[1]; outForward[2] = orientation[2];
				outUp[0] = orientation[3]; outUp[1] = orientation[4]; outUp[2] = orientation[5];
				return;
			}

			auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-get_listener_orientation", {}));
			if (IsErrorResponse(response))
			{
				FranAudioShared::Logger::LogError("Server returned an error for get_listener_orientation");
				return;
			}
			try
			{
				auto params = SplitResponse(response);
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

				cache.listenerOrientation = { outForward[0], outForward[1], outForward[2], outUp[0], outUp[1], outUp[2] };
			}
			catch (const std::exception& e)
			{
				FranAudioShared::Logger::LogError("Failed to get listener orientation from server!");
				return;
			}
		}

		FRANAUDIO_CLIENT_API void SetMasterVolume(float volume)
		{
			cache.masterVolume = volume;
			FranAudioClient::SendNoReply(FranAudioShared::Network::NetworkFunction("backend-set_master_volume", { std::to_string(volume) }));
		}

		FRANAUDIO_CLIENT_API float GetMasterVolume()
		{
			if (cache.masterVolume)
			{
				return *cache.masterVolume;
			}

			auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-get_master_volume", {}));
			if (IsErrorResponse(response))
			{
				FranAudioShared::Logger::LogError("Server returned an error for get_master_volume");
				return 0.0f;
			}
			try
			{
				const float volume = std::stof(response);
				cache.masterVolume = volume;
				return volume;
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
			if (IsErrorResponse(response))
			{
				FranAudioShared::Logger::LogError(std::format("Server failed to load audio file: {}", filename));
				return SIZE_MAX;
			}
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
			if (IsErrorResponse(response))
			{
				FranAudioShared::Logger::LogError(std::format("Server failed to load audio file with settings: {}", filename));
				return SIZE_MAX;
			}
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

		FRANAUDIO_CLIENT_API bool UnloadAudioFile(const std::string& filename)
		{
			auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-unload_audio_file", { filename }));
			if (IsErrorResponse(response))
			{
				FranAudioShared::Logger::LogError(std::format("Server returned an error for unload_audio_file: {}", filename));
				return false;
			}
			return response == "1";
		}

		FRANAUDIO_CLIENT_API size_t PlayAudioFile(const std::string& filename, bool looping)
		{
			auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-play_audio_file", { filename, looping ? "1" : "0" }));
			if (IsErrorResponse(response))
			{
				FranAudioShared::Logger::LogError(std::format("Server failed to play audio file: {}", filename));
				return SIZE_MAX;
			}
			try
			{
				const size_t soundID = std::stoull(response);
				if (soundID != SIZE_MAX)
				{
					cache.sounds[soundID].looping = looping;
				}
				return soundID;
			}
			catch (const std::exception& e)
			{
				FranAudioShared::Logger::LogError(std::format("Failed to play audio file: {}", filename));
				return SIZE_MAX;
			}
		}

		FRANAUDIO_CLIENT_API size_t PlayAudioFileStream(const std::string& filename, bool looping)
		{
			auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-play_audio_file_stream", { filename, looping ? "1" : "0" }));
			if (IsErrorResponse(response))
			{
				FranAudioShared::Logger::LogError(std::format("Server failed to play audio file stream: {}", filename));
				return SIZE_MAX;
			}
			try
			{
				const size_t soundID = std::stoull(response);
				if (soundID != SIZE_MAX)
				{
					cache.sounds[soundID].looping = looping;
				}
				return soundID;
			}
			catch (const std::exception& e)
			{
				FranAudioShared::Logger::LogError(std::format("Failed to play audio file stream: {}", filename));
				return SIZE_MAX;
			}
		}

		FRANAUDIO_CLIENT_API const FranAudioShared::Containers::Vector<std::pair<size_t, std::string>> GetLoadedAudioFiles()
		{
			FranAudioShared::Containers::Vector<std::pair<size_t, std::string>> loadedFiles;

			const auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-get_loaded_files", {}));

			// Empty reply just means nothing is loaded.
			if (response.empty() || response == "err")
			{
				return loadedFiles;
			}

			const auto parts = SplitResponse(response);
			for (size_t i = 0; i + 1 < parts.size(); i += 2)
			{
				try
				{
					loadedFiles.emplace_back(static_cast<size_t>(std::stoull(parts[i])), parts[i + 1]);
				}
				catch (const std::exception&)
				{
					FranAudioShared::Logger::LogError("Invalid response from server for get_loaded_files");
					break;
				}
			}

			return loadedFiles;
		}

		FRANAUDIO_CLIENT_API void SetGroupVolume(const std::string& groupName, float volume)
		{
			cache.groupVolumes[groupName] = volume;
			FranAudioClient::SendNoReply(FranAudioShared::Network::NetworkFunction("backend-set_group_volume", { groupName, std::to_string(volume) }));
		}

		FRANAUDIO_CLIENT_API float GetGroupVolume(const std::string& groupName)
		{
			if (auto it = cache.groupVolumes.find(groupName); it != cache.groupVolumes.end())
			{
				return it->second;
			}

			auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-get_group_volume", { groupName }));
			if (IsErrorResponse(response))
			{
				FranAudioShared::Logger::LogError(std::format("Server returned an error for get_group_volume of group: {}", groupName));
				return 1.0f;
			}
			try
			{
				const float volume = std::stof(response);
				cache.groupVolumes[groupName] = volume;
				return volume;
			}
			catch (const std::exception& e)
			{
				FranAudioShared::Logger::LogError(std::format("Failed to get volume for group: {}", groupName));
				return 1.0f;
			}
		}

		FRANAUDIO_CLIENT_API void SetGroupExclusive(const std::string& groupName, bool exclusive)
		{
			cache.groupExclusive[groupName] = exclusive;
			FranAudioClient::SendNoReply(FranAudioShared::Network::NetworkFunction("backend-set_group_exclusive", { groupName, exclusive ? "1" : "0" }));
		}

		FRANAUDIO_CLIENT_API bool IsGroupExclusive(const std::string& groupName)
		{
			if (const auto it = cache.groupExclusive.find(groupName); it != cache.groupExclusive.end())
			{
				return it->second;
			}

			const auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-is_group_exclusive", { groupName }));
			if (IsErrorResponse(response))
			{
				FranAudioShared::Logger::LogError(std::format("Server returned an error for is_group_exclusive of group: {}", groupName));
				return false;
			}

			const bool exclusive = response == "1";
			cache.groupExclusive[groupName] = exclusive;
			return exclusive;
		}
		
		// ========================
		// Macro Sound Management
		// ========================

		FRANAUDIO_CLIENT_API const FranAudioShared::Containers::Vector<size_t> GetActiveSoundIDs()
		{
			std::string response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-get_active_sound_ids", {}));
			const auto soundIDs = FranAudioShared::Serialisation::BinarySerialiser::DeserialiseVector<size_t>(response);

			for (auto it = cache.sounds.begin(); it != cache.sounds.end();)
			{
				if (std::find(soundIDs.begin(), soundIDs.end(), it->first) == soundIDs.end())
				{
					it = cache.sounds.erase(it);
				}
				else
				{
					++it;
				}
			}

			return soundIDs;
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
			const bool valid = response == "1";
			if (!valid)
			{
				cache.sounds.erase(soundIndex);
			}
			return valid;
		}

		FRANAUDIO_CLIENT_API void Stop(size_t soundIndex)
		{
			cache.sounds.erase(soundIndex);
			FranAudioClient::SendNoReply(FranAudioShared::Network::NetworkFunction("sound-stop", { std::to_string(soundIndex) }));
		}

		FRANAUDIO_CLIENT_API void SetPaused(size_t soundID, bool isPaused)
		{
			cache.sounds[soundID].paused = isPaused;
			FranAudioClient::SendNoReply(FranAudioShared::Network::NetworkFunction("sound-set_paused", { std::to_string(soundID), isPaused ? "1" : "0" }));
		}

		FRANAUDIO_CLIENT_API bool IsPaused(size_t soundID)
		{
			if (auto it = cache.sounds.find(soundID); it != cache.sounds.end() && it->second.paused)
			{
				return *it->second.paused;
			}

			const bool paused = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("sound-is_paused", { std::to_string(soundID) })) == "1";
			cache.sounds[soundID].paused = paused;
			return paused;
		}

		FRANAUDIO_CLIENT_API void SetVolume(size_t soundIndex, float volume)
		{
			cache.sounds[soundIndex].volume = volume;
			FranAudioClient::SendNoReply(FranAudioShared::Network::NetworkFunction("sound-set_volume", { std::to_string(soundIndex), std::to_string(volume) }));
		}

		FRANAUDIO_CLIENT_API float GetVolume(size_t soundIndex)
		{
			if (auto it = cache.sounds.find(soundIndex); it != cache.sounds.end() && it->second.volume)
			{
				return *it->second.volume;
			}

			auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("sound-get_volume", { std::to_string(soundIndex) }));
			if (IsErrorResponse(response))
			{
				FranAudioShared::Logger::LogError(std::format("Server returned an error for get_volume of sound index: {}", std::to_string(soundIndex)));
				return 0.0f;
			}
			try
			{
				const float volume = std::stof(response);
				cache.sounds[soundIndex].volume = volume;
				return volume;
			}
			catch (const std::exception& e)
			{
				FranAudioShared::Logger::LogError(std::format("Failed to get volume for sound index: {}", std::to_string(soundIndex)));
				return 0.0f;
			}
		}

		FRANAUDIO_CLIENT_API void SetPitch(size_t soundID, float pitch)
		{
			cache.sounds[soundID].pitch = pitch;
			FranAudioClient::SendNoReply(FranAudioShared::Network::NetworkFunction("sound-set_pitch", { std::to_string(soundID), std::to_string(pitch) }));
		}

		FRANAUDIO_CLIENT_API float GetPitch(size_t soundID)
		{
			if (auto it = cache.sounds.find(soundID); it != cache.sounds.end() && it->second.pitch)
			{
				return *it->second.pitch;
			}

			auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("sound-get_pitch", { std::to_string(soundID) }));
			if (IsErrorResponse(response))
			{
				FranAudioShared::Logger::LogError(std::format("Server returned an error for get_pitch of sound index: {}", std::to_string(soundID)));
				return 0.0f;
			}
			try
			{
				const float pitch = std::stof(response);
				cache.sounds[soundID].pitch = pitch;
				return pitch;
			}
			catch (const std::exception& e)
			{
				FranAudioShared::Logger::LogError(std::format("Failed to get pitch for sound index: {}", std::to_string(soundID)));
				return 0.0f;
			}
		}

		FRANAUDIO_CLIENT_API void SetLooping(size_t soundID, bool looping)
		{
			cache.sounds[soundID].looping = looping;
			FranAudioClient::SendNoReply(FranAudioShared::Network::NetworkFunction("sound-set_looping", { std::to_string(soundID), looping ? "1" : "0" }));
		}

		FRANAUDIO_CLIENT_API bool IsLooping(size_t soundID)
		{
			if (auto it = cache.sounds.find(soundID); it != cache.sounds.end() && it->second.looping)
			{
				return *it->second.looping;
			}

			auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("sound-is_looping", { std::to_string(soundID) }));
			if (IsErrorResponse(response))
			{
				FranAudioShared::Logger::LogError(std::format("Server returned an error for is_looping of sound index: {}", std::to_string(soundID)));
				return false;
			}

			const bool looping = response == "1";
			cache.sounds[soundID].looping = looping;
			return looping;
		}

		FRANAUDIO_CLIENT_API void SetGroup(size_t soundID, const std::string& groupName)
		{
			cache.sounds[soundID].group = groupName;
			FranAudioClient::SendNoReply(FranAudioShared::Network::NetworkFunction("sound-set_group", { std::to_string(soundID), groupName }));
		}

		FRANAUDIO_CLIENT_API std::string GetGroup(size_t soundID)
		{
			if (auto it = cache.sounds.find(soundID); it != cache.sounds.end() && it->second.group)
			{
				return *it->second.group;
			}

			auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("sound-get_group", { std::to_string(soundID) }));
			if (response == "err")
			{
				FranAudioShared::Logger::LogError(std::format("Server returned an error for get_group of sound index: {}", std::to_string(soundID)));
				return {};
			}

			// Every sound belongs to a group; map a (legacy) empty reply to the default group.
			if (response.empty())
			{
				response = FranAudioShared::defaultSoundGroupName;
			}

			cache.sounds[soundID].group = response;
			return response;
		}

		FRANAUDIO_CLIENT_API void SetPosition(size_t soundIndex, float position[3])
		{
			cache.sounds[soundIndex].position = { position[0], position[1], position[2] };
			FranAudioClient::SendNoReply(FranAudioShared::Network::NetworkFunction("sound-set_position", { std::to_string(soundIndex), std::to_string(position[0]), std::to_string(position[1]), std::to_string(position[2]) }));
		}

		FRANAUDIO_CLIENT_API void SetPositions(std::span<const FranAudioShared::SoundPositionUpdate> positions)
		{
			if (positions.empty())
			{
				return;
			}

			FranAudioShared::Containers::Vector<std::string> params;
			params.reserve(positions.size() * 4);

			for (const auto& update : positions)
			{
				cache.sounds[update.soundID].position = { update.position.x, update.position.y, update.position.z };

				params.push_back(std::to_string(update.soundID));
				params.push_back(std::to_string(update.position.x));
				params.push_back(std::to_string(update.position.y));
				params.push_back(std::to_string(update.position.z));
			}

			FranAudioClient::SendNoReply(FranAudioShared::Network::NetworkFunction("sound-set_position_multi", std::move(params)));
		}

		FRANAUDIO_CLIENT_API void GetPosition(size_t soundIndex, float position[3])
		{
			if (auto it = cache.sounds.find(soundIndex); it != cache.sounds.end() && it->second.position)
			{
				const auto& pos = *it->second.position;
				position[0] = pos[0]; position[1] = pos[1]; position[2] = pos[2];
				return;
			}

			auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("sound-get_position", { std::to_string(soundIndex) }));
			if (IsErrorResponse(response))
			{
				FranAudioShared::Logger::LogError(std::format("Server returned an error for get_position of sound index: {}", std::to_string(soundIndex)));
				return;
			}
			try
			{
				auto params = SplitResponse(response);
				if (params.size() < 3)
				{
					FranAudioShared::Logger::LogError(std::format("Invalid response from server for get_position of sound index: {}", std::to_string(soundIndex)));
					return;
				}
				position[0] = std::stof(params[0]);
				position[1] = std::stof(params[1]);
				position[2] = std::stof(params[2]);

				cache.sounds[soundIndex].position = { position[0], position[1], position[2] };
			}
			catch (const std::exception& e)
			{
				FranAudioShared::Logger::LogError(std::format("Failed to get position for sound index: {}", std::to_string(soundIndex)));
				return;
			}
		}

		FRANAUDIO_CLIENT_API void SetAttenuation(size_t soundID, float rolloffFactor, float minDistance, float maxDistance)
		{
			cache.sounds[soundID].attenuation = { rolloffFactor, minDistance, maxDistance };
			FranAudioClient::SendNoReply(FranAudioShared::Network::NetworkFunction("sound-set_attenuation",
			{ 
				std::to_string(soundID), 
				std::to_string(rolloffFactor), 
				std::to_string(minDistance), 
				std::to_string(maxDistance) 
			}));
		}

		FRANAUDIO_CLIENT_API void GetAttenuation(size_t soundID, float& outRolloffFactor, float& outMinDistance, float& outMaxDistance)
		{
			if (auto it = cache.sounds.find(soundID); it != cache.sounds.end() && it->second.attenuation)
			{
				const auto& attenuation = *it->second.attenuation;
				outRolloffFactor = attenuation[0];
				outMinDistance = attenuation[1];
				outMaxDistance = attenuation[2];
				return;
			}

			auto response = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("sound-get_attenuation", { std::to_string(soundID) }));
			if (IsErrorResponse(response))
			{
				FranAudioShared::Logger::LogError(std::format("Server returned an error for get_attenuation of sound index: {}", std::to_string(soundID)));
				return;
			}
			try
			{
				auto params = SplitResponse(response);
				if (params.size() < 3)
				{
					FranAudioShared::Logger::LogError(std::format("Invalid response from server for get_attenuation of sound index: {}", std::to_string(soundID)));
					return;
				}
				outRolloffFactor = std::stof(params[0]);
				outMinDistance = std::stof(params[1]);
				outMaxDistance = std::stof(params[2]);

				cache.sounds[soundID].attenuation = { outRolloffFactor, outMinDistance, outMaxDistance };
			}
			catch (const std::exception& e)
			{
				FranAudioShared::Logger::LogError(std::format("Failed to get attenuation for sound index: {}", std::to_string(soundID)));
				return;
			}
		}
	}
}
