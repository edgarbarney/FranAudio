// FranticDreamer 2022-2025

#ifdef _WIN32

#include <iostream>
#include <string>
#include <csignal>

#include "FranAudio.hpp"
#include "FranAudioServer.hpp"

#include "FranAudioShared/Network/Network.hpp"
#include "FranAudioShared/Logger/Logger.hpp"

#pragma comment(lib, "ws2_32.lib")

WSADATA wsaData;
SOCKET listenSocket = INVALID_SOCKET;
SOCKET clientSocket = INVALID_SOCKET;
sockaddr_in serverAddress;

bool isSocketValid = false;
bool isWSAInitialised = false;

static const wchar_t* StringToWideString(const char* asciiStr)
{
	static wchar_t wideStr[256];
	MultiByteToWideChar(CP_UTF8, 0, asciiStr, -1, wideStr, 256);
	return wideStr;
}

BOOL WINAPI ConsoleEventsHandler(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		FranAudioServer::Shutdown();
		return TRUE;
	default:
		return FALSE;
	}
}

void ClosedBySignal(int signal)
{
	FranAudioServer::Shutdown();
}

int main()
{
	if (!SetConsoleCtrlHandler(ConsoleEventsHandler, TRUE))
	{
		FranAudioShared::Logger::LogError("Could not set control handler");
		FranAudioServer::Shutdown();
		return 1;
	}

	std::signal(SIGINT, ClosedBySignal);
	std::signal(SIGILL, ClosedBySignal);
	std::signal(SIGFPE, ClosedBySignal);
	std::signal(SIGSEGV, ClosedBySignal);
	std::signal(SIGTERM, ClosedBySignal);
	std::signal(SIGBREAK, ClosedBySignal);
	std::signal(SIGABRT, ClosedBySignal);

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

	bool shutDown = false;
	while (!shutDown)
	{
		clientSocket = accept(listenSocket, nullptr, nullptr);
		if (clientSocket == INVALID_SOCKET)
		{
			const int acceptError = WSAGetLastError();
			if (acceptError == WSAEINTR || acceptError == WSAENOTSOCK || acceptError == WSAEINVAL)
			{
				// Listen socket was closed, maybe by the shutdown handler.
				break;
			}

			FranAudioShared::Logger::LogError("Accept failed! Retrying...");
			continue;
		}

		// Request/Response Ping-pong protocol. Nagle's Algorithm only adds lag here.
		constexpr DWORD noDelay = 1;
		setsockopt(clientSocket, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const char*>(&noDelay), sizeof(noDelay));

		FranAudioShared::Logger::LogMessage("Client connected.");

		// Per-client loop
		while (!shutDown)
		{
			auto requestOpt = FranAudioShared::Network::Win32Helpers::RecvFrame(clientSocket);
			if (!requestOpt)
			{
				FranAudioShared::Logger::LogMessage("Client disconnected. Waiting for a new connection...");
				closesocket(clientSocket);
				clientSocket = INVALID_SOCKET;
				break; // Let's go back to accept loop.
			}

			std::string request = *requestOpt;
			std::string response = FranAudioServer::Receive(request);

			if (response.empty())
			{
				FranAudioShared::Network::Win32Helpers::SendFrame(clientSocket, std::string());
				continue;
			}

			if (response == "$server-stop")
			{
				shutDown = true;
				closesocket(clientSocket);
				clientSocket = INVALID_SOCKET;
				break;
			}

			FranAudioShared::Network::Win32Helpers::SendFrame(clientSocket, response);
		}
	}

	FranAudioServer::Shutdown();
}

void FranAudioServer::Shutdown()
{
	if (clientSocket != INVALID_SOCKET)
		closesocket(clientSocket);
	if (isSocketValid)
		closesocket(listenSocket);
	if (isWSAInitialised)
		WSACleanup();
}

#endif // _WIN32
