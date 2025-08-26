// FranticDreamer 2022-2025

#ifdef _WIN32

#include <iostream>
#include <string>

#include "FranAudio.hpp"
#include "FranAudioServer.hpp"

#include <WinSock2.h>
#include <ws2tcpip.h>

#include "FranAudioShared/Logger/Logger.hpp"

#pragma comment(lib, "ws2_32.lib")

WSADATA wsaData;
SOCKET listenSocket, clientSocket;
sockaddr_in serverAddress;

bool isSocketValid = false;
bool isWSAInitialised = false;

static const wchar_t* StringToWideString(const char* asciiStr)
{
	static wchar_t wideStr[256];
	MultiByteToWideChar(CP_UTF8, 0, asciiStr, -1, wideStr, 256);
	return wideStr;
}

int main()
{
	char buffer[FranAudioShared::Network::messageBufferSize];

	// Initialise Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		FranAudioShared::Logger::LogError("WSAStartup failed");
		FranAudioServer::Shutdown();
		return 1;
	}
	isWSAInitialised = true;

	// Create TCP socket
	listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
	{
		FranAudioShared::Logger::LogError("Socket creation failed");
		FranAudioServer::Shutdown();
		return 1;
	}
	isSocketValid = true;

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(FranAudioShared::Network::listenPort);
	if (InetPton(AF_INET, StringToWideString(FranAudioShared::Network::listenAddress), &serverAddress.sin_addr) != 1)
	{
		FranAudioShared::Logger::LogError("Invalid address!");
		FranAudioServer::Shutdown();
		return 1;
	}

	if (bind(listenSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
	{
		FranAudioShared::Logger::LogError("Bind failed!");
		FranAudioServer::Shutdown();
		return 1;
	}

	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		FranAudioShared::Logger::LogError("Listen failed!");
		FranAudioServer::Shutdown();
		return 1;
	}

	FranAudioShared::Logger::LogMessage(std::format("TCP server listening on port {}...", FranAudioShared::Network::listenPort));

	// Accept client connection
	clientSocket = accept(listenSocket, nullptr, nullptr);
	if (clientSocket == INVALID_SOCKET)
	{
		FranAudioShared::Logger::LogError("Accept failed!");
		FranAudioServer::Shutdown();
		return 1;
	}

	bool shutDown = false;
	while (!shutDown)
	{
		std::string request = FranAudioShared::Network::Win32Helpers::RecvFrame(clientSocket);

		std::string response = FranAudioServer::Receive(request.c_str());

		if (response.empty()) 
		{
			// Keep-alive behavior (optional): send an empty frame
			FranAudioShared::Network::Win32Helpers::SendFrame(clientSocket, std::string());
			continue;
		}

		if (response == "stop") 
		{ 
			shutDown = true; 
		}

		FranAudioShared::Network::Win32Helpers::SendFrame(clientSocket, response);
	}

	FranAudioServer::Shutdown();
}

void FranAudioServer::Shutdown()
{
	if (clientSocket)
		closesocket(clientSocket);
	if (isSocketValid)
		closesocket(listenSocket);
	if (isWSAInitialised)
		WSACleanup();
}

#endif // _WIN32
