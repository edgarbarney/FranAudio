// FranticDreamer 2022-2025

// FranAudio Test Application =========================================================
//
//
// This is a very primitive test application for FranAudio.
// It uses ImGui for the UI and GLFW for the windowing system.
// It allows you to test various audio files and see if they play correctly.
// It also allows you to stop currently playing sounds.
// You can test positioning, volume, and other features of the FranAudio library.
//
// Since this is a test application, it does not do much optimization or error handling.
// It is meant to show a simple way to test the FranAudio library and its features.
//
//
// ====================================================================================

#include <iostream>
#include <string>
#include <format>
#include <vector>

#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"
#include "styles/imgui_style_candy.hpp"

#ifndef FRANAUDIO_USE_SERVER
#include "FranAudio.hpp"
#else
#include "FranAudioClient/FranAudioClient.hpp"
#endif

#include "FranAudioShared/Logger/Logger.hpp"

#include "FranAudioTest.hpp"

std::vector<size_t> gPlayingSounds;

size_t PlayTestFile(const std::string& filename)
{
#ifndef FRANAUDIO_USE_SERVER
	auto backend = FranAudio::GetBackend();
	backend->LoadAudioFile(filename);
	size_t soundId = backend->PlayAudioFile(filename);

	if (soundId == SIZE_MAX)
	{
		return SIZE_MAX;
	}

	gPlayingSounds.push_back(soundId);
	return soundId;
#else
	FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-load_audio_file", { filename }));

	try
	{
		size_t soundId = std::stoull(FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-play_audio_file", { filename })));

		if (soundId == SIZE_MAX)
		{
			throw std::runtime_error("Invalid sound ID returned");
		}

		gPlayingSounds.push_back(soundId);
		return soundId;
	}
	catch (const std::exception& e)
	{
		return SIZE_MAX;
	}
#endif
}

void StopTestSound(size_t soundId)
{
#ifndef FRANAUDIO_USE_SERVER
	FranAudio::GetBackend()->StopPlayingSound(soundId);
#else
	FranAudioClient::Send(FranAudioShared::Network::NetworkFunction("backend-stop_playing_sound", { std::to_string(soundId) }));
#endif
	
	gPlayingSounds.erase(std::remove(gPlayingSounds.begin(), gPlayingSounds.end(), soundId), gPlayingSounds.end());
	FranAudioShared::Logger::LogMessage(std::format("Stopped sound ID: {}", soundId));
}

int main()
{
#ifndef FRANAUDIO_USE_SERVER
	FranAudioShared::Logger::LogMessage("Starting FranAudio Library Test Application...");
	FranAudio::Init();
#else
	FranAudioShared::Logger::LogMessage("Starting FranAudio Server-Client Test Application...");
	FranAudioClient::Init(true);
#endif

	gPlayingSounds.reserve(20);

	if (!glfwInit())
	{
		FranAudioShared::Logger::LogError("Failed to initialize GLFW!");
		return -1;
	}

	GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui Example", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		FranAudioShared::Logger::LogMessage("Failed to create GLFW window!");
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0) {
		FranAudioShared::Logger::LogError("Failed to initialize GLAD OpenGL Context!");
		return -1;
	}

	// Setup ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	//io.ConfigWindowsMoveFromTitleBarOnly = true; // Allow moving windows only from the title bar. To prevent teleportation of listener.

	// Fallback style
	ImGui::StyleColorsDark();

	//Apply The real ImGui style
	ImGui::Styles::Candy::ApplyStyle(io);

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	size_t lastPlayedSoundId = 0;

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Controls
		
		// Let's create a simple ImGui window with some controls

		ImGui::Begin("FranAudio Test Controls", nullptr, ImGuiWindowFlags_NoResize);
		ImGui::Text("This is a simple test window for FranAudio.");
		ImGui::Text("You can test audio files by clicking the buttons below.");
		if (ImGui::Button("Test WAV File"))
		{
			lastPlayedSoundId = PlayTestFile("test.wav");
		}
		if (ImGui::Button("Test WAV File with unusual data"))
		{
			lastPlayedSoundId = PlayTestFile("test_unusualdata.wav");
		}
		if (ImGui::Button("Test MP3 File"))
		{
			lastPlayedSoundId = PlayTestFile("test.mp3");
		}
		if (ImGui::Button("Test OGG File"))
		{
			lastPlayedSoundId = PlayTestFile("test.ogg");
		}
		if (ImGui::Button("Test Opus File"))
		{
			lastPlayedSoundId = PlayTestFile("test.opus");
		}
		if (ImGui::Button("Test FLAC File"))
		{
			lastPlayedSoundId = PlayTestFile("test.flac");
		}
		ImGui::End();

		if (lastPlayedSoundId == SIZE_MAX)
		{
			// Make this red
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
			ImGui::Begin("Debug");
				ImGui::Text("Last sound couldn't be played!");
			ImGui::End();
			ImGui::PopStyleColor();
		}

		static ImVec2 listenerPos = { 0.0f, 0.0f };
		static float listenerPosition[3] = { 0.0f, 0.0f, 0.0f };
		static float listenerForward[3] = { 0.0f, 0.0f, -1.0f };
		static float listenerUp[3] = { 0.0f, 1.0f, 0.0f };

		ImGui::Begin("Listener", nullptr, ImGuiWindowFlags_NoResize);
			// Is the window being dragged?
			//bool isDragging = ImGui::IsWindowHovered() && ImGui::IsMouseDragging(ImGuiMouseButton_Left);

			// If the window is not being dragged, update the listener position based on sliders
			//if (isDragging)
			//{
				listenerPos = ImGui::GetWindowPos();
				listenerPosition[0] = listenerPos.x;
				listenerPosition[1] = listenerPos.y;
			//}

			ImGui::Text("Listener Position Z");
			//ImGui::VSliderFloat("##", ImVec2(75, 50), &listenerPosition[2], 0.0f, 2000, "%.1f");
			ImGui::SliderFloat("##", &listenerPosition[2], 0.0f, 2000, "%.1f");
		ImGui::End();

		ImGui::Begin("Listener Controls", nullptr, ImGuiWindowFlags_NoResize);
			ImGui::Text("This is a simple listener control window.");
			ImGui::Text("You can position the \"Listener\" window anywhere on the screen.");
			ImGui::Text("Listener's position's X and Y is determined by the window's top left corner's position on screen.");
			ImGui::Text("You can also change the listener's Z and orientation.");
			ImGui::SeparatorText("");
			if (ImGui::SliderFloat3("Listener Position Vector", listenerPosition, 0.0f, static_cast<float>(glfwGetVideoMode(glfwGetPrimaryMonitor())->width), "%.1f"))
			{
				ImGui::SetWindowPos("Listener", ImVec2(listenerPosition[0], listenerPosition[1]));
			}
			ImGui::SliderFloat3("Listener Forward Vector", listenerForward, -1.0f, 1.0f, "%.1f");
			ImGui::SliderFloat3("Listener Up Vector", listenerUp, -1.0f, 1.0f, "%.1f");
		ImGui::End();

		// Display currently playing sounds
		for (size_t soundId : gPlayingSounds)
		{
			ImGui::Begin(std::to_string(soundId).c_str(), nullptr, ImGuiWindowFlags_NoResize);
				ImGui::Text("Sound ID: %zu", soundId);
				if (ImGui::Button(std::format("Stop Sound ID: {}", soundId).c_str()))
				{
					StopTestSound(soundId);
				}
			ImGui::End();
		}

		// Controls End

		glClearColor(0.75f, 0.65f, 0.25f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap buffers
		glfwSwapBuffers(window);
	}

#ifndef FRANAUDIO_USE_SERVER
	FranAudio::Shutdown();
#endif

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}
