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
// (Though as of now, there is none. But there will be, eventually.)
//
//
// ====================================================================================

#include <iostream>
#include <string>
#include <format>
#include <filesystem>
#include <span>
#include <utility>

#if defined(_WIN32) || defined(_WIN64)
#include "windows.h"
#endif

#ifndef FRANAUDIO_USE_SERVER
#include "FranAudio.hpp"
#include "FranAudio/Backend/Backend.hpp"
#else
#include "FranAudioClient/FranAudioClient.hpp"
#endif

#include "FranAudioShared/FranAudioShared.hpp"
#include "FranAudioShared/Logger/Logger.hpp"
#include "FranAudioShared/Containers/Vector.hpp"
#include "FranAudioShared/Containers/UnorderedMap.hpp"

#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_filebrowser.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"
#include "styles/imgui_style_candy.hpp"

#include "FranAudioTest.hpp"

// I don't like macros, but this is useful now.
#ifndef FRANAUDIO_USE_SERVER

#define BACKEND_CHECK(shouldEndPrematurely)	\
if (!FranAudio::IsBackendValid())			\
{											\
	ImGui::Begin("Debug");					\
	ImGui::Text("Backend is invalid!");		\
	ImGui::End();							\
											\
	if (shouldEndPrematurely)				\
	{										\
		ImGui::End();						\
	}										\
											\
	CompleteFrameDraw(window);				\
	continue;								\
}

#define DECODER_CHECK(shouldEndPrematurely)			\
if (!FranAudio::GetBackend()->GetCurrentDecoder())	\
{													\
	ImGui::Begin("Debug");							\
	ImGui::Text("Decoder is invalid!");				\
	ImGui::End();									\
													\
	if (shouldEndPrematurely)						\
	{												\
		ImGui::End();								\
	}												\
													\
	CompleteFrameDraw(window);						\
	continue;										\
}

#else

// Don't do anything in the client-server mode
#define BACKEND_CHECK(shouldEndPrematurely)
#define DECODER_CHECK(shouldEndPrematurely)

#endif

static void CompleteFrameDraw(GLFWwindow* window)
{
	glClearColor(0.75f, 0.65f, 0.25f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Swap buffers
	glfwSwapBuffers(window);
}

static void DrawConsole(FranAudioShared::Logger::FranAudioConsole& franConsole, size_t& orderedID)
{
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
}

static size_t PlayTestFile(const std::string& filename, bool looping, bool streamed)
{
	FranAudioShared::Logger::LogMessage(std::format("Playing test file: {}", filename));

#ifndef FRANAUDIO_USE_SERVER
	auto backend = FranAudio::GetBackend();

	if (streamed)
	{
		return backend->PlayAudioFileStream(filename, looping);
	}

	backend->LoadAudioFile(filename);
	size_t soundId = backend->PlayAudioFile(filename);

	if (soundId != SIZE_MAX && looping)
	{
		backend->SetSoundLooping(soundId, true);
	}

	return soundId;
#else
	if (streamed)
	{
		return FranAudioClient::Wrapper::Backend::PlayAudioFileStream(filename, looping);
	}

	FranAudioClient::Wrapper::Backend::LoadAudioFile(filename);

	return FranAudioClient::Wrapper::Backend::PlayAudioFile(filename, looping);
#endif
}

static void StopTestSound(size_t soundId)
{
#ifndef FRANAUDIO_USE_SERVER
	FranAudio::GetBackend()->StopPlayingSound(soundId);
#else
	FranAudioClient::Wrapper::Sound::Stop(soundId);
#endif
	
	FranAudioShared::Logger::LogMessage(std::format("Stopped sound ID: {}", soundId));
}

static void PauseTestSound(size_t soundId, bool pause)
{
#ifndef FRANAUDIO_USE_SERVER
	FranAudio::GetBackend()->SetSoundPaused(soundId, pause);
#else
	FranAudioClient::Wrapper::Sound::SetPaused(soundId, pause);
#endif
}

static bool IsTestSoundPaused(size_t soundId)
{
#ifndef FRANAUDIO_USE_SERVER
	//return FranAudio::GetBackend()->GetSound(soundId).IsPaused();
	return FranAudio::GetBackend()->IsSoundPaused(soundId);
#else
	return FranAudioClient::Wrapper::Sound::IsPaused(soundId);
#endif
}

static void SetListenerTransform(float position[3], float forward[3], float up[3])
{
#ifndef FRANAUDIO_USE_SERVER
	FranAudio::GetBackend()->SetListenerTransform
	(
		{ position[0], position[1], position[2] },
		{ forward[0], forward[1], forward[2] },
		{ up[0], up[1], up[2] }
	);
#else
	FranAudioClient::Wrapper::Backend::SetListenerTransform(position, forward, up);
#endif
}

static void SetListenerVolume(float volume)
{
#ifndef FRANAUDIO_USE_SERVER
	FranAudio::GetBackend()->SetMasterVolume(volume);
#else
	FranAudioClient::Wrapper::Backend::SetMasterVolume(volume);
#endif
}

static void SetSoundPosition(size_t soundId, float position[3])
{
#ifndef FRANAUDIO_USE_SERVER
	//FranAudio::GetBackend()->GetSound(soundId).SetPosition(position);
	FranAudio::GetBackend()->SetSoundPosition(soundId, { position[0], position[1], position[2] });
#else
	FranAudioClient::Wrapper::Sound::SetPosition(soundId, position);
#endif
}

static void SetSoundVolume(size_t soundId, float volume)
{
#ifndef FRANAUDIO_USE_SERVER
	//FranAudio::GetBackend()->GetSound(soundId).SetVolume(volume);
	FranAudio::GetBackend()->SetSoundVolume(soundId, volume);
#else
	FranAudioClient::Wrapper::Sound::SetVolume(soundId, volume);
#endif
	FranAudioShared::Logger::LogMessage(std::format("Set volume of sound ID {} to {}", soundId, volume));
}

static float GetSoundVolume(size_t soundId)
{
#ifndef FRANAUDIO_USE_SERVER
	//return FranAudio::GetBackend()->GetSound(soundId).GetVolume();
	return FranAudio::GetBackend()->GetSoundVolume(soundId);
#else
	return FranAudioClient::Wrapper::Sound::GetVolume(soundId);
#endif
}

static void SetTestSoundPitch(size_t soundId, float pitch)
{
#ifndef FRANAUDIO_USE_SERVER
	FranAudio::GetBackend()->SetSoundPitch(soundId, pitch);
#else
	FranAudioClient::Wrapper::Sound::SetPitch(soundId, pitch);
#endif
}

static float GetTestSoundPitch(size_t soundId)
{
#ifndef FRANAUDIO_USE_SERVER
	return FranAudio::GetBackend()->GetSoundPitch(soundId);
#else
	return FranAudioClient::Wrapper::Sound::GetPitch(soundId);
#endif
}

static void SetTestSoundLooping(size_t soundId, bool looping)
{
#ifndef FRANAUDIO_USE_SERVER
	FranAudio::GetBackend()->SetSoundLooping(soundId, looping);
#else
	FranAudioClient::Wrapper::Sound::SetLooping(soundId, looping);
#endif
}

static bool IsTestSoundLooping(size_t soundId)
{
#ifndef FRANAUDIO_USE_SERVER
	return FranAudio::GetBackend()->IsSoundLooping(soundId);
#else
	return FranAudioClient::Wrapper::Sound::IsLooping(soundId);
#endif
}

static void SetTestSoundAttenuation(size_t soundId, const FranAudioShared::SoundAttenuation& attenuation)
{
#ifndef FRANAUDIO_USE_SERVER
	FranAudio::GetBackend()->SetSoundAttenuation(soundId, attenuation);
#else
	FranAudioClient::Wrapper::Sound::SetAttenuation(soundId, attenuation.rolloffFactor, attenuation.minDistance, attenuation.maxDistance);
#endif
}

static FranAudioShared::SoundAttenuation GetTestSoundAttenuation(size_t soundId)
{
#ifndef FRANAUDIO_USE_SERVER
	return FranAudio::GetBackend()->GetSoundAttenuation(soundId);
#else
	FranAudioShared::SoundAttenuation attenuation = {};
	FranAudioClient::Wrapper::Sound::GetAttenuation(soundId, attenuation.rolloffFactor, attenuation.minDistance, attenuation.maxDistance);
	return attenuation;
#endif
}

static bool UnloadTestFile(const std::string& filename)
{
#ifndef FRANAUDIO_USE_SERVER
	return FranAudio::GetBackend()->UnloadAudioFile(filename);
#else
	return FranAudioClient::Wrapper::Backend::UnloadAudioFile(filename);
#endif
}

static FranAudioShared::Containers::Vector<std::pair<size_t, std::string>> GetLoadedTestFiles()
{
#ifndef FRANAUDIO_USE_SERVER
	FranAudioShared::Containers::Vector<std::pair<size_t, std::string>> loadedFiles;
	for (const auto& [waveDataID, waveData] : FranAudio::GetBackend()->GetWaveDataCache())
	{
		loadedFiles.emplace_back(waveDataID, waveData.GetFilename());
	}
	return loadedFiles;
#else
	return FranAudioClient::Wrapper::Backend::GetLoadedAudioFiles();
#endif
}

static FranAudioShared::Containers::Vector<size_t> GetActiveSoundIDs()
{
	//FranAudioShared::Logger::LogMessage("Retrieved active sound ids.");
#ifndef FRANAUDIO_USE_SERVER
	return FranAudio::GetBackend()->GetActiveSoundIDs();
#else
	static FranAudioShared::Containers::Vector<size_t> soundIDBuffer(32);

	size_t activeSoundCount = FranAudioClient::Wrapper::Backend::GetActiveSoundIDs(std::span<size_t>(soundIDBuffer.data(), soundIDBuffer.size()));

	while (activeSoundCount > soundIDBuffer.size())
	{
		soundIDBuffer.resize(activeSoundCount);
		activeSoundCount = FranAudioClient::Wrapper::Backend::GetActiveSoundIDs(std::span<size_t>(soundIDBuffer.data(), soundIDBuffer.size()));
	}

	return FranAudioShared::Containers::Vector<size_t>(soundIDBuffer.begin(), soundIDBuffer.begin() + activeSoundCount);
#endif
}

static void SetDecodeSettings(const FranAudio::Decoder::DecodeSettings& settings)
{
#ifndef FRANAUDIO_USE_SERVER
	FranAudio::GetBackend()->SetDecodeSettings(settings);
#else
	FranAudioClient::Wrapper::Backend::SetDecodeSettings(settings);
#endif
	FranAudioShared::Logger::LogMessage("Set new decode settings.");
}

static FranAudio::Decoder::DecodeSettings GetDecodeSettings()
{
	//FranAudioShared::Logger::LogMessage("Retrieved decode settings.");
#ifndef FRANAUDIO_USE_SERVER
	return FranAudio::GetBackend()->GetDecodeSettings();
#else
	return FranAudioClient::Wrapper::Backend::GetDecodeSettings();
#endif
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

	io.LogFilename = nullptr;
	io.IniFilename = nullptr;

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

		// Console
		DrawConsole(franConsole, orderedID);

		BACKEND_CHECK(false);

		ImGui::Begin("FranAudio Test Controls", nullptr, ImGuiWindowFlags_NoResize);
		ImGui::Text("This is a simple test window for FranAudio.");
		ImGui::Text("You can test audio files by clicking the buttons below.");
#ifndef FRANAUDIO_USE_SERVER
		ImGui::Text("Backend:");
		if (ImGui::BeginCombo("##backend", FranAudio::GetBackend()->GetBackendName()))
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

		// Check again
		// User might've invalidated it
		BACKEND_CHECK(true);

		DECODER_CHECK(true);

		ImGui::Text("Decoder:");
		if (ImGui::BeginCombo("##decoder", FranAudio::GetBackend()->GetDecoderName()))
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

		DECODER_CHECK(true);
#else
		ImGui::Text("Client-Server Mode Enabled");
		ImGui::Text("Backend:");
		if (ImGui::BeginCombo("##backend", FranAudioClient::Wrapper::Backend::GetBackendName().c_str()))
		{
			for (size_t backendId = 1; backendId < std::size(FranAudio::Backend::BackendTypeNames); backendId++)
			{
				const bool isSelected = ((FranAudio::Backend::BackendType)backendId == FranAudioClient::Wrapper::Backend::GetBackendType());
				if (ImGui::Selectable(FranAudio::Backend::BackendTypeNames[backendId], isSelected))
				{
					FranAudioClient::Wrapper::SetBackend((FranAudio::Backend::BackendType)backendId);
				}
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		ImGui::Text("Decoder:");
		if (ImGui::BeginCombo("##decoder", FranAudioClient::Wrapper::Backend::GetDecoderName().c_str()))
		{
			for (size_t decoderId = 1; decoderId < std::size(FranAudio::Decoder::DecoderTypeNames); decoderId++)
			{
				bool isSelected = ((FranAudio::Decoder::DecoderType)decoderId == FranAudioClient::Wrapper::Backend::GetDecoderType());
				if (ImGui::Selectable(FranAudio::Decoder::DecoderTypeNames[decoderId], isSelected))
				{
					FranAudioClient::Wrapper::Backend::SetDecoder((FranAudio::Decoder::DecoderType)decoderId);
				}
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
#endif

		FranAudio::Decoder::DecodeSettings decodeSettings = GetDecodeSettings();
		const char* formatDisplay = (size_t)decodeSettings.GetForcedFormat() == 0 ? "Auto" : FranAudio::Sound::WaveFormatNames[(size_t)decodeSettings.GetForcedFormat()];
		ImGui::Separator();
		ImGui::Text("Forced Decoder Settings: (0 means disabled)");
		ImGui::Text("Format:");
		if (ImGui::BeginCombo("##forceformat", formatDisplay))
		{
			for (size_t formatId = 0; formatId < std::size(FranAudio::Sound::WaveFormatNames); formatId++)
			{
				bool isSelected = (formatId == (size_t)decodeSettings.GetForcedFormat());
				if (ImGui::Selectable(FranAudio::Sound::WaveFormatNames[formatId], isSelected))
				{
					decodeSettings.SetForcedFormat((FranAudio::Sound::WaveFormat)formatId);
					SetDecodeSettings(decodeSettings);
				}
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		static const char* channelDisplay[] = { "Auto", "Mono", "Stereo" };

		ImGui::Text("Channels:");
		if (ImGui::BeginCombo("##forcechannels", channelDisplay[decodeSettings.GetForcedChannels()]))
		{
			for (size_t channels = 0; channels <= 2; channels++)
			{
				bool isSelected = (channels == (size_t)decodeSettings.GetForcedChannels());
				if (ImGui::Selectable(channelDisplay[channels], isSelected))
				{
					decodeSettings.SetForcedChannels((uint8_t)channels);
					SetDecodeSettings(decodeSettings);
				}
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		std::string sampleRateDisplay = (size_t)decodeSettings.GetForcedSampleRate() == 0 ? "Auto" : std::to_string(decodeSettings.GetForcedSampleRate());
		ImGui::Text("Sample Rate:");
		if (ImGui::BeginCombo("##forcesamplerate", sampleRateDisplay.c_str()))
		{
			bool isSelected_auto = (decodeSettings.GetForcedSampleRate() == 0);
			if (ImGui::Selectable("Auto", isSelected_auto))
			{
				decodeSettings.SetForcedSampleRate(0);
				SetDecodeSettings(decodeSettings);
			}
			if (isSelected_auto)
			{
				ImGui::SetItemDefaultFocus();
			}

			for (int samplerate : FranAudio::Sound::StandardSampleRates)
			{
				bool isSelected = (samplerate == decodeSettings.GetForcedSampleRate());
				if (ImGui::Selectable(std::to_string(samplerate).c_str(), isSelected))
				{
					decodeSettings.SetForcedSampleRate(samplerate);
					SetDecodeSettings(decodeSettings);
				}
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		// For the next played sound.
		static bool playLooping = false;
		static bool playStreamed = false;

		// The test app uses screen pixels as world units
		// So we have to scale up the defaults compared to the library's normal use.
		static FranAudioShared::SoundAttenuation newSoundAttenuation = { 1.0f, 100.0f, 10000.0f };

		ImGui::Separator();
		ImGui::Text("Play Options:");
		ImGui::Checkbox("Loop", &playLooping);
		ImGui::SameLine();
		ImGui::Checkbox("Stream from disk", &playStreamed);

		ImGui::Text("New Sound Attenuation:");
		ImGui::SliderFloat("Rolloff Factor", &newSoundAttenuation.rolloffFactor, 0.0f, 10.0f, "%.2f");
		ImGui::SliderFloat("Min Distance", &newSoundAttenuation.minDistance, 0.0f, 2000.0f, "%.1f");
		ImGui::SliderFloat("Max Distance", &newSoundAttenuation.maxDistance, 0.0f, 20000.0f, "%.1f");

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
			if (ImGui::SliderFloat("Master (Listener) Volume", &listenerVolume, 0.0f, 10.0f, "%.2f"))
			{
				SetListenerVolume(listenerVolume);
			}
		ImGui::End();

		SetListenerTransform(listenerPosition, listenerForward, listenerUp);

		auto soundIDs = GetActiveSoundIDs();

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

				if (ImGui::SliderFloat("##soundvolume", &soundVolume, 0.0f, 10.0f, "%.2f"))
				{
					SetSoundVolume(soundId, soundVolume);
				}

				float soundPitch = GetTestSoundPitch(soundId);
				ImGui::Text("Sound Pitch: ");
				if (ImGui::SliderFloat("##soundpitch", &soundPitch, 0.25f, 4.0f, "%.2f"))
				{
					SetTestSoundPitch(soundId, soundPitch);
				}

				bool soundLooping = IsTestSoundLooping(soundId);
				if (ImGui::Checkbox("Looping", &soundLooping))
				{
					SetTestSoundLooping(soundId, soundLooping);
				}

				FranAudioShared::SoundAttenuation soundAttenuation = GetTestSoundAttenuation(soundId);
				bool attenuationChanged = false;
				ImGui::Text("Attenuation: ");
				attenuationChanged |= ImGui::SliderFloat("Rolloff##soundatt", &soundAttenuation.rolloffFactor, 0.0f, 10.0f, "%.2f");
				attenuationChanged |= ImGui::SliderFloat("Min Dist##soundatt", &soundAttenuation.minDistance, 0.0f, 2000.0f, "%.1f");
				attenuationChanged |= ImGui::SliderFloat("Max Dist##soundatt", &soundAttenuation.maxDistance, 0.0f, 20000.0f, "%.1f");
				if (attenuationChanged)
				{
					SetTestSoundAttenuation(soundId, soundAttenuation);
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

		// Live WaveData cache view to watch allocations/unallocations in real time.
		ImGui::Begin("Loaded Audio Files");
			auto loadedFiles = GetLoadedTestFiles();
			ImGui::Text("Loaded files: %zu", loadedFiles.size());
			ImGui::Separator();
			for (const auto& [waveDataID, loadedFilename] : loadedFiles)
			{
				ImGui::PushID(orderedID++);
				if (ImGui::Button("Unload"))
				{
					UnloadTestFile(loadedFilename);
				}
				ImGui::SameLine();
				ImGui::Text("[%zu] %s", waveDataID, std::filesystem::path(loadedFilename).filename().string().c_str());
				if (ImGui::IsItemHovered())
				{
					ImGui::SetTooltip("%s", loadedFilename.c_str());
				}
				ImGui::PopID();
			}
		ImGui::End();

		// Live active sounds view to watch allocations/unallocations in real time.
		// Re-fetch the list in case a sound was stopped from its own window this frame.
		soundIDs = GetActiveSoundIDs();
		ImGui::Begin("Playing Sounds");
			ImGui::Text("Active sounds: %zu", soundIDs.size());
			bool stoppedAll = false;
			if (ImGui::Button("Stop All"))
			{
				for (size_t soundId : soundIDs)
				{
					StopTestSound(soundId);
				}
				stoppedAll = true;
			}
			ImGui::Separator();
			if (!stoppedAll)
			{
				for (size_t soundId : soundIDs)
				{
					// Query state before the stop button.
					// So a click can't trigger "invalid sound" errors on the same frame.
					const bool isPaused = IsTestSoundPaused(soundId);
					const bool isLooping = IsTestSoundLooping(soundId);

					ImGui::PushID(orderedID++);
					if (ImGui::Button("Stop"))
					{
						StopTestSound(soundId);
					}
					ImGui::SameLine();
					ImGui::Text("Sound ID: %zu%s%s", soundId,
						isPaused ? " (paused)" : "",
						isLooping ? " (looping)" : "");
					ImGui::PopID();
				}
			}
		ImGui::End();

		fileBrowser.Display();

		if (fileBrowser.HasSelected())
		{
			lastPlayedSoundId = PlayTestFile(fileBrowser.GetSelected().string(), playLooping, playStreamed);

			if (lastPlayedSoundId != SIZE_MAX)
			{
				SetTestSoundAttenuation(lastPlayedSoundId, newSoundAttenuation);
			}

			fileBrowser.ClearSelected();
		}

		// Controls End

		CompleteFrameDraw(window);
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

#if defined(_WIN32) || defined(_WIN64)
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	return main();
}
#endif
