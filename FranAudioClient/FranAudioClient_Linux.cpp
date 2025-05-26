// FranticDreamer 2022-2025

#ifdef __linux__

#include <iostream>

#include "FranAudio.hpp"

#include "FranAudioClient.hpp"

int main(int argc, char* argv[])
{
	bool init = false;

	while (!init)
	{
		std::cout << "Waiting for Initialisation Message..." << std::endl;
	}

	FranAudio::Init();

	std::cin.get();
}

#endif // __linux__