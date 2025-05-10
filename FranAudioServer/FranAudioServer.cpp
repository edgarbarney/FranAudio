// FranticDreamer 2022-2024

#include <iostream>
#include <filesystem>
#include <chrono>

#include "FranAudio.hpp"

#include "FranAudioServer.hpp"

void FranAudioServer::Init()
{
	std::cout << "FranAudioServer::Init() Start" << std::endl;

	FranAudio::Init();

	std::cout << "FranAudioServer::Init() Done" << std::endl;
}

void FranAudioServer::Receive(char* buffer)
{
	if (buffer == nullptr)
	{
		std::cerr << "Invalid buffer!" << std::endl;
		return;
	}

	if (strlen(buffer) == 0)
	{
		std::cerr << "Empty buffer!" << std::endl;
		return;
	}

	if (strcmp(buffer, "initServer") == 0)
	{
		Init();

		if (FranAudio::GetBackend() == nullptr)
		{
			std::cerr << "Failed to initialize backend!" << std::endl;
			return;
		}
	}
	else if (strcmp(buffer, "_testmode") == 0)
	{
		Init();

		if (FranAudio::GetBackend() == nullptr)
		{
			std::cerr << "Failed to initialize backend!" << std::endl;
			return;
		}

		auto t_start = std::chrono::high_resolution_clock::now();
		auto t_end = t_start;

		FranAudio::GetBackend()->LoadAudioFile("test.wav");
		FranAudio::GetBackend()->LoadAudioFile("test_unusualdata.wav");
		FranAudio::GetBackend()->LoadAudioFile("test.ogg");
		FranAudio::GetBackend()->LoadAudioFile("test.opus");
		FranAudio::GetBackend()->LoadAudioFile("test.flac");
		FranAudio::GetBackend()->LoadAudioFile("test.mp3");

		t_end = std::chrono::high_resolution_clock::now();

		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start).count();
		std::cout << "Time taken to load audio files: " << duration << " ms" << std::endl;

		std::cout << "Testing specific audio formats:" << std::endl;

		t_start = std::chrono::high_resolution_clock::now();
		// Vorbis read speed test
		for (int i = 0; i < 5; ++i)
		{
			FranAudio::GetBackend()->LoadAudioFile("test.ogg");
		}
		t_end = std::chrono::high_resolution_clock::now();
		duration = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start).count();
		std::cout << "Vorbis read time: " << duration << " ms" << std::endl;

		t_start = std::chrono::high_resolution_clock::now();
		// wav read speed test
		for (int i = 0; i < 500; ++i)
		{
			FranAudio::GetBackend()->LoadAudioFile("test.wav");
		}
		t_end = std::chrono::high_resolution_clock::now();
		duration = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start).count();
		std::cout << "Wav read time: " << duration << " ms" << std::endl;

		t_start = std::chrono::high_resolution_clock::now();
		// mp3 read speed test
		for (int i = 0; i < 5; ++i)
		{
			FranAudio::GetBackend()->LoadAudioFile("test.mp3");
		}
		t_end = std::chrono::high_resolution_clock::now();
		duration = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start).count();
		std::cout << "MP3 read time: " << duration << " ms" << std::endl;
	}
	else
	{
		std::cout << "Received: " << buffer << std::endl;
	}
}