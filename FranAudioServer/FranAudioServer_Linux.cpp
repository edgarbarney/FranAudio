// FranticDreamer 2022-2025

#ifdef __linux__

#include <iostream>

#include "FranAudio.hpp"

#include "FranAudioServer.hpp"

int main(int argc, char* argv[])
{
	bool init = false;

	while (!init)
	{
		std::println("Waiting for Initialisation Message...");
	}

	FranAudio::Init();

	std::cin.get();
}

#endif // __linux__
