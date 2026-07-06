// FranticDreamer 2022-2025

#ifdef __linux__

#include <chrono>
#include <iostream>
#include <thread>

#include "FranAudio.hpp"

#include "FranAudioServer.hpp"

int main(int argc, char* argv[])
{
	bool init = false;

	while (!init)
	{
		std::println("Waiting for Initialisation Message...");
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	FranAudio::Init();

	std::cin.get();
}

#endif // __linux__
