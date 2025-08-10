// FranticDreamer 2022-2025
#pragma once

#include "FranAudioClientAPI.hpp"
#include "FranAudioShared/Network.hpp"

namespace FranAudioClient
{
	FRANAUDIO_CLIENT_API void Init(bool isTestmode = false);
	FRANAUDIO_CLIENT_API void Shutdown();

	/// <summary>
	/// Send a message to the server, and wait for a response.
	/// Response will be returned as a string, and will be empty if no response is received.
	///
	/// - Message format should be like this:
	/// "$[functionName]|[param1]|[param2]|...|[paramN]"
	/// </summary>
	/// 
	/// <param name="message">Message to Send</param>
	/// <returns>Response from the server, empty if no response is received</returns>
	FRANAUDIO_CLIENT_API std::string Send(const char* message);

	/// <summary>
	/// Send a message to the server, and wait for a response.
	/// Response will be returned as a string, and will be empty if no response is received.
	/// 
	/// <param name="message">Message to Send</param>
	/// <returns>Response from the server, empty if no response is received</returns>
	FRANAUDIO_CLIENT_API std::string Send(const FranAudioShared::Network::NetworkFunction& message);

}