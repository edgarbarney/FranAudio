// FranticDreamer 2022-2025
#pragma once

#include <print>
#include <format>

#include "Logger.hpp"

// ========================
// Logging Functions
// ========================

void FranAudioShared::Logger::LogMessage(const std::string& message, bool newLine)
{
#ifndef FRANAUDIO_DISABLE_LOGGING
	if (customStreamBuffer != nullptr)
	{
		*customOstream << "[INFO] " << message;

		if (newLine)
			*customOstream << "\n";

		return;
	}

	if (newLine)
		std::println("\033[36m[INFO]\033[0m {}", message);
	else
		std::print("\033[36m[INFO]\033[0m {}", message);
#endif
}

void FranAudioShared::Logger::LogError(const std::string& message, bool newLine)
{
#ifndef FRANAUDIO_DISABLE_LOGGING
	if (customStreamBuffer != nullptr)
	{
		*customOstream << "[ERROR] " << message;

		if (newLine)
			*customOstream << "\n";

		return;
	}

	if (newLine)
		std::println("\033[31m[ERROR]\033[0m {}", message);
	else
		std::print("\033[31m[ERROR]\033[0m {}", message);
#endif
}

void FranAudioShared::Logger::LogWarning(const std::string& message, bool newLine)
{
#ifndef FRANAUDIO_DISABLE_LOGGING
	if (customStreamBuffer != nullptr)
	{
		*customOstream << "[WARNING] " << message;

		if (newLine)
			*customOstream << "\n";

		return;
	}

	if (newLine)
		std::println("\033[33m[WARNING]\033[0m {}", message);
	else
		std::print("\033[33m[WARNING]\033[0m {}", message);
#endif
}

void FranAudioShared::Logger::LogSuccess(const std::string& message, bool newLine)
{
#ifndef FRANAUDIO_DISABLE_LOGGING
	if (customStreamBuffer != nullptr)
	{
		*customOstream << "[SUCCESS] " << message;

		if (newLine)
			*customOstream << "\n";

		return;
	}

	if (newLine)
		std::println("\033[32m[SUCCESS]\033[0m {}", message);
	else
		std::print("\033[32m[SUCCESS]\033[0m {}", message);
#endif
}

void FranAudioShared::Logger::LogGeneric(const std::string& message, bool newLine)
{
#ifndef FRANAUDIO_DISABLE_LOGGING
	if (customStreamBuffer != nullptr)
	{
		*customOstream << message;

		if (newLine)
			*customOstream << "\n";

		return;
	}

	if (newLine)
		std::println("{}", message);
	else
		std::print("{}", message);
#endif
}

void FranAudioShared::Logger::LogSeperator(bool newLine)
{
#ifndef FRANAUDIO_DISABLE_LOGGING
	// Don't log seperators to custom console for now.
	// TODO: Implement console messages ordered by time.
	if (customStreamBuffer != nullptr)
	{
		// customStreamBuffer->GetConsole().Write("========================================");
	}
	else
	{
		std::print("========================================");
	}

	if (newLine)
		LogNewline();
#endif
}

void FranAudioShared::Logger::LogNewline()
{
#ifndef FRANAUDIO_DISABLE_LOGGING
	if (customStreamBuffer != nullptr)
	{
		*customOstream << "\n";
	}
	else 
	{
		std::println("");
	}
#endif
}

// ========================
// Console
// ========================

FranAudioShared::Logger::FranAudioConsole::FranAudioConsole()
{
	entries.reserve(512); // Pre-allocate space for 512 entries
}

void FranAudioShared::Logger::FranAudioConsole::Write(const std::string& entry)
{
	/*
	if (!entries.empty())
	{
		for (size_t i = 0; i < entries.size(); i++)
		{
			if (entries[i].text == entry)
			{
				entries[i].count++;
				// Move to end
				if (i != entries.size() - 1)
				{
					auto temp = entries[i];
					entries.erase(entries.begin() + i);
					entries.push_back(temp);
				}
				return;
			}
		}
	}

	entries.push_back({ entry, 1 });
	*/

	entries.push_back({ entry });
}

const std::vector<FranAudioShared::Logger::ConsoleEntry>& FranAudioShared::Logger::FranAudioConsole::GetEntries() const
{
	return entries;
}

void FranAudioShared::Logger::FranAudioConsole::Clear()
{
	entries.clear();
}

void FranAudioShared::Logger::FranAudioConsole::Remove(size_t index)
{
	if (index < entries.size())
	{
		entries.erase(entries.begin() + index);
	}
}

int FranAudioShared::Logger::ConsoleStreamBuffer::overflow(int c)
{
	if (c != EOF)
	{
		buffer.push_back(static_cast<char>(c));
	}

	if (c == '\n' || buffer.size() > 254)
	{
		console.Write(buffer);
		buffer.clear();
	}
	return c;
}

FranAudioShared::Logger::ConsoleStreamBuffer::ConsoleStreamBuffer(FranAudioConsole& console)
	: console(console)
{

}

FranAudioShared::Logger::FranAudioConsole& FranAudioShared::Logger::ConsoleStreamBuffer::GetConsole()
{
	return console;
}

// =========================
// Console Routing
// =========================

void FranAudioShared::Logger::RouteToConsole(ConsoleStreamBuffer* consoleBuffer)
{
	if (customStreamBuffer != nullptr)
	{
		// Already routed
		LogError("Logger::RouteToConsole: Already routed! Restore first!");
		return;
	}

	customStreamBuffer = consoleBuffer;

	defaultCoutBuffer = std::cout.rdbuf();
	defaultCerrBuffer = std::cerr.rdbuf();

	std::cout.rdbuf(consoleBuffer);
	std::cerr.rdbuf(consoleBuffer);

	customOstream = std::make_unique<std::ostream>(consoleBuffer);
}

void FranAudioShared::Logger::RestoreDefault()
{
	if (customStreamBuffer == nullptr)
	{
		// Not routed
		LogError("Logger::RestoreDefault: Not routed! Route first!");
		return;
	}
	std::cout.rdbuf(defaultCoutBuffer);
	std::cerr.rdbuf(defaultCerrBuffer);
	customStreamBuffer = nullptr;
	defaultCoutBuffer = nullptr;
	defaultCerrBuffer = nullptr;

	customOstream.reset();
}
