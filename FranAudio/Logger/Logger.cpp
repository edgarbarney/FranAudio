// FranticDreamer 2022-2024
#pragma once

#include <print>

#include "Logger.hpp"

void FranAudio::Logger::LogMessage(const std::string& message, bool newLine)
{
#ifndef FRANAUDIO_DISABLE_LOGGING
	if (newLine)
		std::print("\033[36m[INFO]\033[0m {}\n", message);
	else
		std::print("\033[36m[INFO]\033[0m {}", message);
#endif
}

void FranAudio::Logger::LogError(const std::string& message, bool newLine)
{
#ifndef FRANAUDIO_DISABLE_LOGGING
	if (newLine)
		std::print("\033[31m[ERROR]\033[0m {}\n", message);
	else
		std::print("\033[31m[ERROR]\033[0m {}", message);
#endif
}

void FranAudio::Logger::LogWarning(const std::string& message, bool newLine)
{
#ifndef FRANAUDIO_DISABLE_LOGGING
	if (newLine)
		std::print("\033[33m[WARNING]\033[0m {}\n", message);
	else
		std::print("\033[33m[WARNING]\033[0m {}", message);
#endif
}

void FranAudio::Logger::LogSuccess(const std::string& message, bool newLine)
{
#ifndef FRANAUDIO_DISABLE_LOGGING
	if (newLine)
		std::print("\033[32m[SUCCESS]\033[0m {}\n", message);
	else
		std::print("\033[32m[SUCCESS]\033[0m {}", message);
#endif
}

void FranAudio::Logger::LogGeneric(const std::string& message, bool newLine)
{
#ifndef FRANAUDIO_DISABLE_LOGGING
	if (newLine)
		std::print("{}", message);
	else
		std::print("{}", message);
#endif
}

void FranAudio::Logger::LogSeperator(bool newLine)
{
#ifndef FRANAUDIO_DISABLE_LOGGING
	std::print("========================================");

	if (newLine)
		LogNewline();
#endif
}

void FranAudio::Logger::LogNewline()
{
#ifndef FRANAUDIO_DISABLE_LOGGING
	std::print("\n");
#endif
}

