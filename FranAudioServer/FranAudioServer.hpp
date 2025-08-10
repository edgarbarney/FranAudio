// FranticDreamer 2022-2025
#pragma once

#include <string>
#include <unordered_map>
#include <functional>

#include "FranAudioShared/Network.hpp"

namespace FranAudioServer
{
	void Init();
	void Shutdown();

	std::string Receive(char* buffer);

	/// <summary>
	/// Map of function names to their corresponding handler functions.
	/// Each function takes a NetworkFunction object and returns a string response.
	/// 
	/// This is used for client-server communication in FranAudio.
	/// Very Similar to the library's funtions.
	extern const std::unordered_map<std::string, std::function<std::string(const FranAudioShared::Network::NetworkFunction&)>> functionsMap;
}