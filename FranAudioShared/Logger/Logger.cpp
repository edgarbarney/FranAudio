// FranticDreamer 2022-2025
#pragma once

#include <print>
#include <format>

#include "Logger.hpp"

void FranAudioShared::Logger::LogMessage(const std::string& message, bool newLine)
{
#ifndef FRANAUDIO_DISABLE_LOGGING
	if (newLine)
		std::println("\033[36m[INFO]\033[0m {}", message);
	else
		std::print("\033[36m[INFO]\033[0m {}", message);
#endif
}

void FranAudioShared::Logger::LogError(const std::string& message, bool newLine)
{
#ifndef FRANAUDIO_DISABLE_LOGGING
	if (newLine)
		std::println("\033[31m[ERROR]\033[0m {}", message);
	else
		std::print("\033[31m[ERROR]\033[0m {}", message);
#endif
}

void FranAudioShared::Logger::LogWarning(const std::string& message, bool newLine)
{
#ifndef FRANAUDIO_DISABLE_LOGGING
	if (newLine)
		std::println("\033[33m[WARNING]\033[0m {}", message);
	else
		std::print("\033[33m[WARNING]\033[0m {}", message);
#endif
}

void FranAudioShared::Logger::LogSuccess(const std::string& message, bool newLine)
{
#ifndef FRANAUDIO_DISABLE_LOGGING
	if (newLine)
		std::println("\033[32m[SUCCESS]\033[0m {}", message);
	else
		std::print("\033[32m[SUCCESS]\033[0m {}", message);
#endif
}

void FranAudioShared::Logger::LogGeneric(const std::string& message, bool newLine)
{
#ifndef FRANAUDIO_DISABLE_LOGGING
	if (newLine)
		std::println("{}", message);
	else
		std::print("{}", message);
#endif
}

void FranAudioShared::Logger::LogSeperator(bool newLine)
{
#ifndef FRANAUDIO_DISABLE_LOGGING
	std::print("========================================");

	if (newLine)
		LogNewline();
#endif
}

void FranAudioShared::Logger::LogNewline()
{
#ifndef FRANAUDIO_DISABLE_LOGGING
	std::println("");
#endif
}

