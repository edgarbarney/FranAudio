// FranticDreamer 2022-2025
#pragma once

#include "FranAudioClientAPI.hpp"

namespace FranAudioClient
{
	// Constants
	// These should match the server's values
	// TODO: Move these to a configuration file

	constexpr size_t messageBufferSize = 1024;
	constexpr const char* listenAddress = "127.0.0.1"; // localhost
	constexpr unsigned short listenPort = 8080;

	FRANAUDIO_CLIENT_API void Init(bool isTestmode = false);
	FRANAUDIO_CLIENT_API void Shutdown();

	/// <summary>
	/// Send a message to the server
	///
	/// - Message format should be like this:
	/// "[functionName]-[param1]-[param2]-...-[paramN]"
	/// </summary>
	/// 
	/// <param name="message">Message to Send</param>
	FRANAUDIO_CLIENT_API void Send(const char* message);
}