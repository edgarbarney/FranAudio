// FranticDreamer 2022-2024
#pragma once

namespace FranAudioServer
{
	// Constants
	// These should match the client's values
	// TODO: Move these to a configuration file
	constexpr size_t messageBufferSize = 1024;
	constexpr const char* listenAddress = "127.0.0.1"; // localhost
	constexpr unsigned short listenPort = 8080;

	void Init();
	void Shutdown();

	void Receive(char* buffer);
}