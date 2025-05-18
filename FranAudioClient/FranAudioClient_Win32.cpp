// FranticDreamer 2022-2024

#ifdef _WIN32

#include <iostream>
#include <string>
#include <WinSock2.h>
#include <ws2tcpip.h>

#include "FranAudio.hpp"

#include "FranAudioClient.hpp"

#pragma comment(lib, "ws2_32.lib")

// Ugh
WSADATA wsaData;
SOCKET udpSocket;
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
		std::cerr << "WSAStartup failed!\n";
		Shutdown();
		return;
	}
	isWSAInitialised = true;

	udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (udpSocket == INVALID_SOCKET)
	{
		std::cerr << "Socket creation failed!\n";
		Shutdown();
		return;
	}
	isSocketValid = true;

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(FranAudioClient::listenPort);
	if (InetPton(AF_INET, StringToWideString(FranAudioClient::listenAddress), &serverAddress.sin_addr) != 1)
	{
		std::cerr << "Invalid address!\n";
		Shutdown();
		return;
	}

	if (isTestmode)
	{
		Send("_testmode");
	}
	else
	{
		Send("initServer");
	}

	Shutdown();
}

FRANAUDIO_CLIENT_API void FranAudioClient::Shutdown()
{
	if (isSocketValid)
		closesocket(udpSocket);

	if (isWSAInitialised)
		WSACleanup();

	//std::cin.get();
}

FRANAUDIO_CLIENT_API void FranAudioClient::Send(const char* message)
{
	if (sendto(udpSocket, message, (int)strlen(message), 0, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
	{
		std::cerr << "Sendto: Send Message Failed!\n Message: " << message;
	}
	else
	{
		std::cout << "Message sent!\n";
	}
}

#endif // _WIN32