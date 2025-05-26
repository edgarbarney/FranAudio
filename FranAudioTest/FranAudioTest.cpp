// FranticDreamer 2022-2025

#include <iostream>

#ifndef FRANAUDIO_USE_SERVER
#include "FranAudio.hpp"
#else
#include "FranAudioClient/FranAudioClient.hpp"
#endif

#include "FranAudioTest.hpp"

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
	std::cin.get();
}