// FranticDreamer 2022-2025

#include <iostream>
#include <filesystem>
#include <chrono>

#include "FranAudio.hpp"

#include "FranAudioServer.hpp"

// TODO: Move this to the test executable.
//
// For now, we do test stuff in the server.
// But when we complete the client-server communication,
// We should move this to the test executable.
void ServerTest_PlaySoundAndWait(const std::string& filename)
{
	FranAudio::GetBackend()->LoadAudioFile(filename);
	auto tempId = FranAudio::GetBackend()->PlayAudioFile(filename);
	std::cout << "Press Enter to continue..." << std::endl;
	std::cin.get(); // Wait for user input to continue
	FranAudio::GetBackend()->StopPlayingSound(tempId);
	std::cout << "Continuing..." << std::endl;
}

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
			std::cerr << "Failed to initialise backend!" << std::endl;
			return;
		}
	}
	else if (strcmp(buffer, "_testmode") == 0)
	{
		// TODO: Move this to the test executable.
		// 
		// For now, we do test stuff in the server.
		// But when we complete the client-server communication,
		// We should move this to the test executable.
		
		Init();

		if (FranAudio::GetBackend() == nullptr)
		{
			std::cerr << "Failed to initialise backend!" << std::endl;
			return;
		}

		//auto t_start = std::chrono::high_resolution_clock::now();
		//auto t_end = t_start;

		ServerTest_PlaySoundAndWait("test.wav");
		ServerTest_PlaySoundAndWait("test_unusualdata.wav");

		ServerTest_PlaySoundAndWait("test.mp3");
		ServerTest_PlaySoundAndWait("test.ogg");
		ServerTest_PlaySoundAndWait("test.opus");
		ServerTest_PlaySoundAndWait("test.flac");

		auto t_start = std::chrono::high_resolution_clock::now();
		auto t_end = t_start;

		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start).count();
		//std::cout << "Time taken to load audio files: " << duration << " ms" << std::endl;

		std::cout << "Now the test will benchmark formats:" << std::endl;

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
		// opus read speed test
		for (int i = 0; i < 5; ++i)
		{
			FranAudio::GetBackend()->LoadAudioFile("test.opus");
		}

		t_end = std::chrono::high_resolution_clock::now();
		duration = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start).count();
		std::cout << "Opus read time: " << duration << " ms" << std::endl;

		t_start = std::chrono::high_resolution_clock::now();
		// wav read speed test
		for (int i = 0; i < 5; ++i)
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

		t_start = std::chrono::high_resolution_clock::now();
		// flac read speed test
		for (int i = 0; i < 5; ++i)
		{
			FranAudio::GetBackend()->LoadAudioFile("test.flac");
		}
		t_end = std::chrono::high_resolution_clock::now();
		duration = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start).count();
		std::cout << "FLAC read time: " << duration << " ms" << std::endl;

		std::cout << std::endl << "All tests completed!" << std::endl;
	}
	else
	{
		std::cout << "Received: " << buffer << std::endl;
	}
}