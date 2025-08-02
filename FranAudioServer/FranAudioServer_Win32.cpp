// FranticDreamer 2022-2025

#ifdef _WIN32

#include <iostream>
#include <string>
#include <WinSock2.h>
#include <ws2tcpip.h>

#include "FranAudio.hpp"

#include "FranAudioServer.hpp"

#pragma comment(lib, "ws2_32.lib")

static const wchar_t* StringToWideString(const char* asciiStr) 
{
	static wchar_t wideStr[256];
	MultiByteToWideChar(CP_UTF8, 0, asciiStr, -1, wideStr, 256);
	return wideStr;
}

WSADATA wsaData;
SOCKET udpSocket;
sockaddr_in serverAddress, clientAddress;

bool isSocketValid = false;
bool isWSAInitialised = false;

int main(int argc, char* argv[])
{
	int clientAddressSize = sizeof(clientAddress);
	char buffer[FranAudioServer::messageBufferSize];

	// Initialise Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cerr << "WSAStartup failed\n";
		FranAudioServer::Shutdown();
		return 1;
	}
	isWSAInitialised = true;

	// Create UDP socket
	udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (udpSocket == INVALID_SOCKET)
	{
		std::cerr << "Socket creation failed\n";
		FranAudioServer::Shutdown();
		return 1;
	}
	isSocketValid = true;

	// Bind socket to a port
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(FranAudioServer::listenPort);

	if (InetPton(AF_INET, StringToWideString(FranAudioServer::listenAddress), &serverAddress.sin_addr) != 1) {
		std::cerr << "Invalid address!\n";
		FranAudioServer::Shutdown();
		return 1;
	}

	if (bind(udpSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
	{
		std::cerr << "Bind failed!\n";
		FranAudioServer::Shutdown();
		return 1;
	}

	std::cout << "Waiting for UDP messages on port " << FranAudioServer::listenPort << "...\n";

	bool shutDown = false;

	// Receive messages
	while (!shutDown)
	{
		int bytesReceived = recvfrom(udpSocket, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&clientAddress, &clientAddressSize);
		if (bytesReceived == SOCKET_ERROR)
		{
			std::cerr << "recvfrom failed!\n";
			continue;
			//break;
		}

		buffer[bytesReceived] = '\0'; // Null-terminate message
		FranAudioServer::Receive(buffer);
	}

	// Cleanup
	FranAudioServer::Shutdown();

	//std::cin.get();
}

void FranAudioServer::Shutdown()
{
	if (isSocketValid)
		closesocket(udpSocket);

	if (isWSAInitialised)
		WSACleanup();

	//std::cin.get();
}

#endif // _WIN32