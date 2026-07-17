// FranticDreamer 2022-2025

#ifdef _WIN32

#include <iostream>
#include <string>

#include "FranAudio.hpp"

#include "FranAudioClient.hpp"

#include "FranAudioShared/Network/Network.hpp"
#include "FranAudioShared/Logger/Logger.hpp"

#pragma comment(lib, "ws2_32.lib")

// Ugh
WSADATA wsaData;
SOCKET tcpSocket;
sockaddr_in serverAddress;

bool isSocketValid = false;
bool isWSAInitialised = false;

static std::wstring StringToWideString(const char* asciiStr)
{
	wchar_t wideStr[256] = {};
	MultiByteToWideChar(CP_UTF8, 0, asciiStr, -1, wideStr, 256);
	return wideStr;
}

static void ApplySocketOptions(SOCKET socket)
{
	setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&FranAudioShared::Network::messageTimeout, sizeof(FranAudioShared::Network::messageTimeout));
	setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&FranAudioShared::Network::messageTimeout, sizeof(FranAudioShared::Network::messageTimeout));

	// Request/Response Ping-pong protocol. Nagle's Algorithm only adds lag here.
	constexpr DWORD noDelay = 1;
	setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&noDelay, sizeof(noDelay));
}

/// <summary>
/// Creates a fresh socket and tries to connect to the server once.
/// Applies socket options and sets isSocketValid on success.
/// </summary>
static bool TryConnect()
{
	tcpSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (tcpSocket == INVALID_SOCKET)
	{
		return false;
	}

	if (connect(tcpSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
	{
		closesocket(tcpSocket);
		tcpSocket = INVALID_SOCKET;
		return false;
	}

	ApplySocketOptions(tcpSocket);
	isSocketValid = true;
	return true;
}

/// <summary>
/// Launches FranAudioServer.exe without opening a console window.
/// The server is started with -autoexit.
/// So it shuts itself down when the game process dies and its connection goes away.
/// </summary>
static bool LaunchServerProcess()
{
	// Find the directory of the module that contains this function (Client DLL).
	HMODULE thisModule = nullptr;
	GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		reinterpret_cast<LPCWSTR>(&LaunchServerProcess), &thisModule);

	wchar_t modulePath[MAX_PATH] = {};
	GetModuleFileNameW(thisModule, modulePath, MAX_PATH);

	std::wstring directory(modulePath);
	const size_t lastSlash = directory.find_last_of(L"\\/");
	directory = lastSlash == std::wstring::npos ? L"" : directory.substr(0, lastSlash + 1);

	const std::wstring serverExe = directory + L"FranAudioServer.exe";
	std::wstring commandLine = L"\"" + serverExe + L"\" -autoexit";

	STARTUPINFOW startupInfo = { sizeof(startupInfo) };
	PROCESS_INFORMATION processInfo = {};

	if (!CreateProcessW(serverExe.c_str(), commandLine.data(), nullptr, nullptr, FALSE,
		CREATE_NO_WINDOW, nullptr, directory.empty() ? nullptr : directory.c_str(), &startupInfo, &processInfo))
	{
		FranAudioShared::Logger::LogError("Failed to launch FranAudioServer.exe!");
		return false;
	}

	CloseHandle(processInfo.hThread);
	CloseHandle(processInfo.hProcess);
	return true;
}

namespace FranAudioClient
{
	FRANAUDIO_CLIENT_API void Init(bool isTestmode)
	{
		// Setup Logger to route to console.
		static FranAudioShared::Logger::FranAudioConsole franConsole;
		static FranAudioShared::Logger::ConsoleStreamBuffer consoleBuffer(franConsole);
		FranAudioShared::Logger::RouteToConsole(&consoleBuffer);

		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		{
			FranAudioShared::Logger::LogError("WSAStartup failed!");
			Shutdown();
			return;
		}
		isWSAInitialised = true;

		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(FranAudioShared::Network::listenPort);
		if (InetPton(AF_INET, StringToWideString(FranAudioShared::Network::listenAddress).c_str(), &serverAddress.sin_addr) != 1)
		{
			FranAudioShared::Logger::LogError("Invalid address!");
			Shutdown();
			return;
		}

		if (!TryConnect())
		{
			// No server running; try to launch one next to the client module, and wait for it to start listening.
			FranAudioShared::Logger::LogMessage("No server found, launching FranAudioServer...");

			if (!LaunchServerProcess())
			{
				FranAudioShared::Logger::LogError("TCP connection failed and the server could not be launched!");
				Shutdown();
				return;
			}

			constexpr int maxAttempts = 40;
			bool connected = false;
			for (int attempt = 0; attempt < maxAttempts && !connected; ++attempt)
			{
				Sleep(250);
				connected = TryConnect();
			}

			if (!connected)
			{
				FranAudioShared::Logger::LogError("TCP connection failed! Launched the server but could not connect to it.");
				Shutdown();
				return;
			}
		}

		Wrapper::ClearCache();

		Send("$server-init");
	}

	FRANAUDIO_CLIENT_API void Shutdown()
	{
		if (isSocketValid)
		{
			closesocket(tcpSocket);
			isSocketValid = false;
		}

		if (isWSAInitialised)
		{
			WSACleanup();
			isWSAInitialised = false;
		}
	}

	FRANAUDIO_CLIENT_API bool Reconnect()
	{
		if (isSocketValid)
		{
			closesocket(tcpSocket);
			isSocketValid = false;
		}

		if (!TryConnect())
			return false;

		// The server may have been restarted, so cached values may be stale.
		Wrapper::ClearCache();
		return true;
	}

	FRANAUDIO_CLIENT_API std::string Send(std::string message)
	{
		if (!isSocketValid)
		{
			return {};
		}

		if (!FranAudioShared::Network::Win32Helpers::SendFrame(tcpSocket, message))
		{
			if (Reconnect())
			{
				FranAudioShared::Logger::LogMessage("Reconnected to server, retrying...");
				FranAudioShared::Network::Win32Helpers::SendFrame(tcpSocket, message);
			}
			else
			{
				FranAudioShared::Logger::LogError("Failed to send message and reconnect to server.");
				Shutdown();
				return {};
			}
		}

		auto reply = FranAudioShared::Network::Win32Helpers::RecvFrame(tcpSocket);
		if (!reply)
		{
			if (Reconnect())
			{
				FranAudioShared::Logger::LogMessage("Reconnected to server, but no reply received.");
				return {};
			}

			FranAudioShared::Logger::LogError("Failed to receive reply from server.");
			return {};
		}

		return *reply;
	}

	FRANAUDIO_CLIENT_API std::string Send(const FranAudioShared::Network::NetworkFunction& message)
	{
#ifdef FRANAUDIO_SERVER_DEBUG
		FranAudioShared::Logger::LogMessage(std::format("Sending network function: {}", message.ToString()));
#endif

		return Send(message.ToString());
	}

	FRANAUDIO_CLIENT_API void SendNoReply(const FranAudioShared::Network::NetworkFunction& message)
	{
		if (!isSocketValid)
		{
			return;
		}

		// "$name|..." -> "$!name|..." so the server knows not to reply.
		std::string wireMessage = message.ToString();
		wireMessage.insert(1, 1, FranAudioShared::Network::noReplyMarker);

#ifdef FRANAUDIO_SERVER_DEBUG
		FranAudioShared::Logger::LogMessage(std::format("Sending network function (no reply): {}", wireMessage));
#endif

		if (!FranAudioShared::Network::Win32Helpers::SendFrame(tcpSocket, wireMessage))
		{
			if (Reconnect())
			{
				FranAudioShared::Logger::LogMessage("Reconnected to server, retrying...");
				FranAudioShared::Network::Win32Helpers::SendFrame(tcpSocket, wireMessage);
			}
			else
			{
				FranAudioShared::Logger::LogError("Failed to send message and reconnect to server.");
				Shutdown();
			}
		}
	}
}

#endif // _WIN32
