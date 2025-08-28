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
// If you want to see how you should use FranAudio in your own application,
// please refer to the examples provided with the library.
//
//
// ====================================================================================

#include <iostream>
#include <string>
#include <format>
#include <vector>

#ifndef FRANAUDIO_USE_SERVER
#include "FranAudio.hpp"
#else
#include "FranAudioClient/FranAudioClient.hpp"
#endif

#include "FranAudioShared/Logger/Logger.hpp"
#include "FranAudioShared/Containers/UnorderedMap.hpp"

#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_filebrowser.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"
#include "styles/imgui_style_candy.hpp"

#include "windows.h"

#include "FranAudioTest.hpp"

size_t PlayTestFile(const std::string& filename)
{
	FranAudioShared::Logger::LogMessage(std::format("Playing test file: {}", filename));

#ifndef FRANAUDIO_USE_SERVER
	auto backend = FranAudio::GetBackend();
	backend->LoadAudioFile(filename);
	size_t soundId = backend->PlayAudioFile(filename);

	if (soundId == SIZE_MAX)
	{
		return SIZE_MAX;
	}

	return soundId;
#else
	FranAudioClient::Wrapper::Backend::LoadAudioFile(filename);

	return FranAudioClient::Wrapper::Backend::PlayAudioFile(filename);
#endif
}

void StopTestSound(size_t soundId)
{
#ifndef FRANAUDIO_USE_SERVER
	FranAudio::GetBackend()->StopPlayingSound(soundId);
#else
	FranAudioClient::Wrapper::Sound::Stop(soundId);
#endif
	
	FranAudioShared::Logger::LogMessage(std::format("Stopped sound ID: {}", soundId));
}

void PauseTestSound(size_t soundId, bool pause)
{
#ifndef FRANAUDIO_USE_SERVER
	FranAudio::GetBackend()->SetSoundPaused(soundId, pause);
#else
	FranAudioClient::Wrapper::Sound::SetPaused(soundId, pause);
#endif
}

bool IsTestSoundPaused(size_t soundId)
{
#ifndef FRANAUDIO_USE_SERVER
	//return FranAudio::GetBackend()->GetSound(soundId).IsPaused();
	return FranAudio::GetBackend()->IsSoundPaused(soundId);
#else
	return FranAudioClient::Wrapper::Sound::IsPaused(soundId);
#endif
}

void SetListenerTransform(float position[3], float forward[3], float up[3])
{
#ifndef FRANAUDIO_USE_SERVER
	FranAudio::GetBackend()->SetListenerTransform(position, forward, up);
#else
	FranAudioClient::Wrapper::Backend::SetListenerTransform(position, forward, up);
#endif
}

void SetListenerVolume(float volume)
{
#ifndef FRANAUDIO_USE_SERVER
	FranAudio::GetBackend()->SetMasterVolume(volume);
#else
	FranAudioClient::Wrapper::Backend::SetMasterVolume(volume);
#endif
}

void SetSoundPosition(size_t soundId, float position[3])
{
#ifndef FRANAUDIO_USE_SERVER
	//FranAudio::GetBackend()->GetSound(soundId).SetPosition(position);
	FranAudio::GetBackend()->SetSoundPosition(soundId, position);
#else
	FranAudioClient::Wrapper::Sound::SetPosition(soundId, position);
#endif
}

void SetSoundVolume(size_t soundId, float volume)
{
#ifndef FRANAUDIO_USE_SERVER
	//FranAudio::GetBackend()->GetSound(soundId).SetVolume(volume);
	FranAudio::GetBackend()->SetSoundVolume(soundId, volume);
#else
	FranAudioClient::Wrapper::Sound::SetVolume(soundId, volume);
#endif
	FranAudioShared::Logger::LogMessage(std::format("Set volume of sound ID {} to {}", soundId, volume));
}

float GetSoundVolume(size_t soundId)
{
#ifndef FRANAUDIO_USE_SERVER
	//return FranAudio::GetBackend()->GetSound(soundId).GetVolume();
	return FranAudio::GetBackend()->GetSoundVolume(soundId);
#else
	return FranAudioClient::Wrapper::Sound::GetVolume(soundId);
#endif
}

std::vector<size_t> GetActiveSoundIDs()
{
#ifndef FRANAUDIO_USE_SERVER
	return FranAudio::GetBackend()->GetActiveSoundIDs();
#else
	return FranAudioClient::Wrapper::Backend::GetActiveSoundIDs();
#endif

	FranAudioShared::Logger::LogMessage("Retrieved active sound ids.");
}


int main()
{
	// Setup Logger to route to console
	FranAudioShared::Logger::FranAudioConsole franConsole;
	FranAudioShared::Logger::ConsoleStreamBuffer consoleBuffer(franConsole);
	FranAudioShared::Logger::RouteToConsole(&consoleBuffer);
#ifndef FRANAUDIO_USE_SERVER
	FranAudio::RouteLoggingToConsole(&consoleBuffer);
#else
	FranAudioClient::RouteClientLoggingToConsole(&consoleBuffer);
#endif

#ifndef FRANAUDIO_USE_SERVER
	FranAudioShared::Logger::LogMessage("Starting FranAudio Library Test Application...");
	FranAudio::Init();
#else
	FranAudioShared::Logger::LogMessage("Starting FranAudio Server-Client Test Application...");
	FranAudioClient::Init(true);
#endif

	if (!glfwInit())
	{
		FranAudioShared::Logger::LogError("Failed to initialize GLFW!");
		return -1;
	}

	GLFWwindow* window = glfwCreateWindow(1280, 720, "FranAudio Test App", nullptr, nullptr);
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

	ImGui::FileBrowser fileBrowser;

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

		// ID to prevent conflicts
		size_t orderedID = 0;

		ImGui::Begin("FranAudio Test Controls", nullptr, ImGuiWindowFlags_NoResize);
		ImGui::Text("This is a simple test window for FranAudio.");
		ImGui::Text("You can test audio files by clicking the buttons below.");
#ifndef FRANAUDIO_USE_SERVER
		ImGui::Text("Backend:");
		auto eben = FranAudio::Backend::BackendTypeNames[(size_t)FranAudio::GetBackend()->GetBackendType()];
		if (ImGui::BeginCombo("##backend", eben))
		{
			for (size_t backendId = 0; backendId < std::size(FranAudio::Backend::BackendTypeNames); backendId++)
			{
				const bool isSelected = ((FranAudio::Backend::BackendType)backendId == FranAudio::GetBackend()->GetBackendType());
				if (ImGui::Selectable(FranAudio::Backend::BackendTypeNames[backendId], isSelected))
				{
					FranAudio::SetBackend((FranAudio::Backend::BackendType)backendId);
				}
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		ImGui::Text("Decoder:");
		if (ImGui::BeginCombo("##decoder", FranAudio::Decoder::DecoderTypeNames[(size_t)FranAudio::GetBackend()->GetDecoderType()]))
		{
			for (size_t decoderId = 0; decoderId < std::size(FranAudio::Decoder::DecoderTypeNames); decoderId++)
			{
				bool isSelected = ((FranAudio::Decoder::DecoderType)decoderId == FranAudio::GetBackend()->GetDecoderType());
				if (ImGui::Selectable(FranAudio::Decoder::DecoderTypeNames[decoderId], isSelected))
				{
					FranAudio::GetBackend()->SetDecoder((FranAudio::Decoder::DecoderType)decoderId);
				}
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
#else
#endif

		if (ImGui::Button("Browse file to play"))
		{
			fileBrowser.Open();
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
		static float listenerVolume = 1.0f;

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
			ImGui::Separator();
			if (ImGui::SliderFloat3("Listener Position Vector", listenerPosition, 0.0f, static_cast<float>(glfwGetVideoMode(glfwGetPrimaryMonitor())->width), "%.1f"))
			{
				ImGui::SetWindowPos("Listener", ImVec2(listenerPosition[0], listenerPosition[1]));
			}
			ImGui::SliderFloat3("Listener Forward Vector", listenerForward, -1.0f, 1.0f, "%.1f");
			ImGui::SliderFloat3("Listener Up Vector", listenerUp, -1.0f, 1.0f, "%.1f");
			ImGui::Separator();
			if (ImGui::SliderFloat("Master (Listener) Volume", &listenerVolume, 0.0f, 2.0f, "%.2f"))
			{
				SetListenerVolume(listenerVolume);
			}
		ImGui::End();

		SetListenerTransform(listenerPosition, listenerForward, listenerUp);

		// Console
		ImGui::Begin("Console", nullptr);
			ImGui::Separator();
			if (ImGui::Button("Clear Console"))
			{
				// Clear console entries
				franConsole.Clear();
			}
			if (ImGui::BeginListBox("##consoleentries", ImVec2(-FLT_MIN, -FLT_MIN)))
			{
					for (size_t i = 0; i < franConsole.GetEntries().size(); i++)
					{
						ImGui::PushID(orderedID++);
						if (ImGui::Selectable(franConsole.GetEntries()[i].text.c_str()))
						{
							franConsole.Remove(i);
							ImGui::PopID();
							break;
						}
						ImGui::PopID();
					}
				ImGui::EndListBox();
			}
			ImGui::Separator();
		ImGui::End();

		auto soundIDs = GetActiveSoundIDs();
		auto activeSounds = FranAudio::GetBackend()->GetActiveSounds();

		// Display currently playing sounds
		for (size_t soundId : soundIDs)
		{
			ImVec2 soundPos;
			float soundPosition[3] = { 0.0f, 0.0f, 0.0f };
			float soundVolume = GetSoundVolume(soundId);

			ImGui::Begin(std::to_string(soundId).c_str(), nullptr, ImGuiWindowFlags_NoResize);
				// Set sound position to window position
				soundPos = ImGui::GetWindowPos();
				soundPosition[0] = soundPos.x;
				soundPosition[1] = soundPos.y;
				SetSoundPosition(soundId, soundPosition);

				ImGui::Text("Sound ID: %zu", soundId);

				ImGui::Text("Sound Volume: ");

				if (ImGui::SliderFloat("##soundvolume", &soundVolume, 0.0f, 2.0f, "%.2f"))
				{
					SetSoundVolume(soundId, soundVolume);
				}

				if (IsTestSoundPaused(soundId))
				{
					if (ImGui::Button(std::format("Resume Sound ID: {}", soundId).c_str()))
					{
						PauseTestSound(soundId, false);
					}
				}
				else
				{
					if (ImGui::Button(std::format("Pause Sound ID: {}", soundId).c_str()))
					{
						PauseTestSound(soundId, true);
					}
				}

				if (ImGui::Button(std::format("Stop Sound ID: {}", soundId).c_str()))
				{
					StopTestSound(soundId);
				}
			ImGui::End();
		}

		fileBrowser.Display();

		if (fileBrowser.HasSelected())
		{
			lastPlayedSoundId = PlayTestFile(fileBrowser.GetSelected().string());
			fileBrowser.ClearSelected();
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

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	return main();
}
