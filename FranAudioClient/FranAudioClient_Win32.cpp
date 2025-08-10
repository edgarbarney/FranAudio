// FranticDreamer 2022-2025

#ifdef _WIN32

#include <iostream>
#include <string>
#include <WinSock2.h>
#include <ws2tcpip.h>

#include "FranAudio.hpp"

#include "FranAudioClient.hpp"

#include "FranAudioShared/Logger/Logger.hpp"

#pragma comment(lib, "ws2_32.lib")

// Ugh
WSADATA wsaData;
SOCKET tcpSocket;
sockaddr_in serverAddress;

bool isSocketValid = false;
bool isWSAInitialised = false;

static const wchar_t* StringToWideString(const char* asciiStr)
{
	static wchar_t wideStr[256];
	MultiByteToWideChar(CP_UTF8, 0, asciiStr, -1, wideStr, 256);
	return wideStr;
}

FRANAUDIO_CLIENT_API void FranAudioClient::Init(bool isTestmode)
{
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		FranAudioShared::Logger::LogError("WSAStartup failed!");
		Shutdown();
		return;
	}
	isWSAInitialised = true;

	tcpSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (tcpSocket == INVALID_SOCKET)
	{
		FranAudioShared::Logger::LogError("Socket creation failed!");
		Shutdown();
		return;
	}
	isSocketValid = true;

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(FranAudioShared::Network::listenPort);
	if (InetPton(AF_INET, StringToWideString(FranAudioShared::Network::listenAddress), &serverAddress.sin_addr) != 1)
	{
		FranAudioShared::Logger::LogError("Invalid address!");
		Shutdown();
		return;
	}

	if (connect(tcpSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
	{
		FranAudioShared::Logger::LogError("TCP connection failed!");
		Shutdown();
		return;
	}

	setsockopt(tcpSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&FranAudioShared::Network::messageTimeout, sizeof(FranAudioShared::Network::messageTimeout));

	Send("$server-init");
}

FRANAUDIO_CLIENT_API void FranAudioClient::Shutdown()
{
	if (isSocketValid)
		closesocket(tcpSocket);

	if (isWSAInitialised)
		WSACleanup();
}

FRANAUDIO_CLIENT_API std::string FranAudioClient::Send(const char* message)
{
	int len = (int)strlen(message);
	if (send(tcpSocket, message, len, 0) == SOCKET_ERROR)
	{
		FranAudioShared::Logger::LogError("Send failed!\n Message: {}", message);
		return {};
	}
	
	char buffer[FranAudioShared::Network::messageBufferSize]{};
	int received = recv(tcpSocket, buffer, sizeof(buffer) - 1, 0);
	
	if (received <= 0)
	{
		FranAudioShared::Logger::LogError("Receive failed or connection closed!");
		return {};
	}

	// Check if the message is "err"
	if (received == 3 && strncmp(buffer, "err", 3) == 0)
	{
		FranAudioShared::Logger::LogError("Received 'err' message!");
	}

	// Explicitly null-terminate
	// Or bad things will happen
	buffer[received] = '\0';
	return buffer;
}

FRANAUDIO_CLIENT_API std::string FranAudioClient::Send(const FranAudioShared::Network::NetworkFunction& message)
{
	return Send(message.ToString().c_str());
}

#endif // _WIN32
