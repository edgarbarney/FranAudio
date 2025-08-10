// FranticDreamer 2022-2025

#include <iostream>
#include <string>
#include <format>

#ifndef FRANAUDIO_USE_SERVER
#include "FranAudio.hpp"
#else
#include "FranAudioClient/FranAudioClient.hpp"
#endif

#include "FranAudioShared/Logger/Logger.hpp"

#include "FranAudioTest.hpp"

void TestFile(std::string filename)
{
#ifndef FRANAUDIO_USE_SERVER
	FranAudio::GetBackend()->LoadAudioFile(filename);
	auto tempId = FranAudio::GetBackend()->PlayAudioFile(filename);
	FranAudioShared::Logger::LogGeneric("Press Enter to continue...");
	std::cin.get(); // Wait for user input to continue
	FranAudio::GetBackend()->StopPlayingSound(tempId);
	FranAudioShared::Logger::LogMessage("Continuing...");
#else
	FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-load_audio_file", {filename}));
	auto tempId = FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-play_audio_file", { filename }));
	FranAudioShared::Logger::LogGeneric("Press Enter to continue...");
	std::cin.get(); // Wait for user input to continue
	FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-stop_playing_sound", { tempId }));
	FranAudioShared::Logger::LogMessage("Continuing...");
#endif
}

int main()
{
#ifndef FRANAUDIO_USE_SERVER
	//FranAudio::Globals::Init();
	//
	//FranAudio::Globals::LogMessage("Hello World!");
	//
	//FranAudio::Globals::Shutdown();
#else
	FranAudioClient::Init(true);
#endif

	TestFile("test.wav");
	TestFile("test_unusualdata.wav");

	TestFile("test.mp3");
	TestFile("test.ogg");
	TestFile("test.opus");
	TestFile("test.flac");

	std::cin.get();
}