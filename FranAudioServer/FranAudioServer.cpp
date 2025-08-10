// FranticDreamer 2022-2025

#include <iostream>
#include <filesystem>
#include <chrono>

#include "FranAudio.hpp"

#include "FranAudioServer.hpp"

#include "FranAudioShared/Logger/Logger.hpp"

void FranAudioServer::Init()
{
	FranAudioShared::Logger::LogMessage("FranAudioServer::Init() Start");

	FranAudio::Init();

	FranAudioShared::Logger::LogMessage("FranAudioServer::Init() Done");
}

std::string FranAudioServer::Receive(char* buffer)
{
	if (buffer == nullptr)
	{
		FranAudioShared::Logger::LogError("Invalid buffer!");
		return {};
	}

	if (strlen(buffer) == 0)
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
		FranAudioShared::Logger::LogError("Function not found!");
		return {};
	}

}

const std::unordered_map<std::string, std::function<std::string(const FranAudioShared::Network::NetworkFunction&)>> FranAudioServer::functionsMap =
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
				FranAudioShared::Logger::LogError("Failed to set master volume: {}", e.what());
				return std::string("err");
			}

			return std::string();
		}
	},

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

	// Backend::StopPlayingSound
	// Params: soundIndex
	// Returns: nothing
	{
		"backend-stop_playing_sound",
		[](const FranAudioShared::Network::NetworkFunction& fn)
		{
			if (fn.params.size() < 1)
			{
				FranAudioShared::Logger::LogError("Missing sound ID parameter");
				return std::string("err");
			}

			if (fn.params[0] == std::to_string(SIZE_MAX))
			{
				FranAudioShared::Logger::LogError("Tried to play an unloaded audio file.");
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
};