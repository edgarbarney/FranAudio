// FranticDreamer 2022-2025
#pragma once

#include "FranAudioClientAPI.hpp"
#include "FranAudioShared/Network/Network.hpp"
#include "FranAudioShared/Logger/Logger.hpp"

#include "Decoder/DecoderTypes.hpp"
#include "Backend/BackendTypes.hpp"

namespace FranAudioClient
{
	// ===========================
	// Functions with Platform Specific Implementations
	// ===========================

	/// <summary>
	/// Initialize the FranAudio Client.
	/// This must be called before any other functions are used.
	/// NOTE: This should be called after the server is started.
	/// </summary>
	FRANAUDIO_CLIENT_API void Init(bool isTestmode = false);

	/// <summary>
	/// Shutdown the FranAudio Client.
	/// This is for cleaning up resources used by the client.
	/// </summary>
	FRANAUDIO_CLIENT_API void Shutdown();

	/// <summary>
	/// Send a message to the server, and wait for a response.
	/// Response will be returned as a string, and will be empty if no response is received.
	///
	/// - Message format should be like this:
	/// "$[functionName]|[param1]|[param2]|...|[paramN]"
	/// </summary>
	/// 
	/// <param name="message">Message to Send</param>
	/// <returns>Response from the server, empty if no response is received</returns>
	FRANAUDIO_CLIENT_API std::string Send(const char* message);

	/// <summary>
	/// Send a message to the server, and wait for a response.
	/// Response will be returned as a string, and will be empty if no response is received.
	/// 
	/// <param name="message">Message to Send</param>
	/// <returns>Response from the server, empty if no response is received</returns>
	FRANAUDIO_CLIENT_API std::string Send(const FranAudioShared::Network::NetworkFunction& message);

	// ===========================
	// End of Functions with Platform Specific Implementations
	// ===========================

	// ========================
	// Shared Logger Usage
	// ========================

	/// <summary>
	/// Routes the client output to the specified console stream buffer.
	/// </summary>
	/// <param name="consoleBuffer">A pointer to the ConsoleStreamBuffer where client output will be directed.</param>
	FRANAUDIO_CLIENT_API void RouteClientLoggingToConsole(FranAudioShared::Logger::ConsoleStreamBuffer* consoleBuffer);

	namespace Wrapper
	{
		/// <summary>
		/// Sets the audio backend to use.
		/// </summary>
		/// <param name="type">The backend type to set, specified as a value of Backend::BackendType.</param>
		FRANAUDIO_CLIENT_API void SetBackend(FranAudio::Backend::BackendType backendType);

		namespace Backend
		{
			/// <summary>
			/// Get the backend type.
			/// </summary>
			/// <returns>Type of this Backend instance</returns>
			FRANAUDIO_CLIENT_API FranAudio::Backend::BackendType GetBackendType();

			// ========================
			// Decoder Management
			// ========================
			
			/// <summary>
			/// Set the decoder type.
			/// </summary>
			/// <param name="decoderType">Type of the decoder to replace the current one</param>
			FRANAUDIO_CLIENT_API void SetDecoder(FranAudio::Decoder::DecoderType decoderType);

			/// <summary>
			/// Get the decoder type.
			/// </summary>
			/// <returns>Type of the current decoder used by this backend</returns>
			FRANAUDIO_CLIENT_API FranAudio::Decoder::DecoderType GetDecoderType();

			// ========================
			// Listener (3D Audio)
			// ========================
	
			/// <summary>
			/// Set the listener's position and orientation.
			/// </summary>
			/// <param name="position">New position of the listener</param>
			/// <param name="forward">New forward vector of the listener</param>
			FRANAUDIO_CLIENT_API void SetListenerTransform(float position[3], float forward[3], float up[3]);

			/// <summary>
			/// Get the listener's position and orientation.
			/// </summary>
			/// <param name="position">Output position of the listener</param>
			/// <param name="forward">Output forward vector of the listener</param>
			/// <param name="up">Output up vector of the listener</param>
			FRANAUDIO_CLIENT_API void GetListenerTransform(float position[3], float forward[3], float up[3]);

			/// <summary>
			/// Set the listener's position.
			/// </summary>
			/// <param name="position">New position of the listener</param> 
			FRANAUDIO_CLIENT_API void SetListenerPosition(const float position[3]);

			/// <summary>
			/// Get the listener's position.
			/// </summary>
			/// <param name="position">Output position of the listener</param>
 			FRANAUDIO_CLIENT_API void GetListenerPosition(float position[3]);

			/// <summary>
			/// Set the listener's orientation.
			/// </summary>
			/// <param name="forward">New forward vector of the listener</param>
			/// <param name="up">New up vector of the listener</param>
			FRANAUDIO_CLIENT_API void SetListenerOrientation(const float forward[3], const float up[3]);

			/// <summary>
			/// Get the listener's orientation.
			/// </summary>
			/// <param name="forward">Output forward vector of the listener</param>
			/// <param name="up">Output up vector of the listener</param>
			FRANAUDIO_CLIENT_API void GetListenerOrientation(float forward[3], float up[3]);

			/// <summary>
			/// Set the master volume.
			/// Can also be the listener's hearing volume.
			/// </summary>
			/// <param name="volume">Volume to set the master volume to (0.0 - 1.0)</param>
			FRANAUDIO_CLIENT_API void SetMasterVolume(float volume);

			/// <summary>
			/// Get the master volume.
			/// Can also be the listener's hearing volume.	
			/// </summary>
			FRANAUDIO_CLIENT_API float GetMasterVolume();

			// ========================
			// Audio File Management
			// ========================

			/// <summary>
			/// Decode an audio file and load it into the memory.
			/// </summary>
			/// <param name="filename">Path to the audio file</param>
			/// <returns>Wave Data Cache Index</returns>
			FRANAUDIO_CLIENT_API size_t LoadAudioFile(const std::string& filename);

			/// <summary>
			/// Play an audio file after checking if it's loaded.
			/// If the audio file is not loaded, it will be loaded and then played.
			/// </summary>
			/// <param name="filename">Path to the audio file</param>
			/// <returns>Active Sounds List Index</returns>
			FRANAUDIO_CLIENT_API size_t PlayAudioFile(const std::string& filename);

			/// <summary>
			/// Play an audio file without checking if it's loaded.
			/// If the audio file is not loaded, it won't be played and will be ignored.
			/// </summary>
			/// <param name="filename">Path to the audio file</param>
			/// <returns>Active Sounds List Index</returns>
			FRANAUDIO_CLIENT_API size_t PlayAudioFileNoChecks(const std::string& filename);

			/// <summary>
			/// Play an audio file without loading it, stream it from the disk.
			/// This is used to play an audio file without loading it into memory.
			/// </summary>
			/// <param name="filename">Path to the audio file</param>
			/// <returns>Active Sounds List Index</returns>
			FRANAUDIO_CLIENT_API size_t PlayAudioFileStream(const std::string& filename);

			// ========================
			// Macro Sound Management
			// ========================

			/// <summary>
			/// Retrieves a list of active sound IDs.
			/// </summary>
			/// <returns>A vector containing the IDs of currently active sounds.</returns>
			FRANAUDIO_CLIENT_API const std::vector<size_t> GetActiveSoundIDs();
		}

		namespace Sound
		{
			// ========================
			// Sound Management
			// ========================

			/// <summary>
			/// Check if a sound is valid by its index.
			/// </summary>
			/// <param name="soundIndex">Index of the sound in the active sounds list</param>
			FRANAUDIO_CLIENT_API bool IsValid(size_t soundIndex);

			/// <summary>
			/// Stop and clear an active sound by its index.
			/// </summary>
			/// <param name="soundIndex">Index of the sound in the active sounds list</param>
			FRANAUDIO_CLIENT_API void Stop(size_t soundIndex);

			/// <summary>
			/// Set whether the sound is paused or not by its ID.
			/// </summary>
			/// <param name="soundID">ID of the sound to modify</param>
			/// <param name="isPaused">True to pause the sound, false to resume playback</param>
			FRANAUDIO_CLIENT_API void SetPaused(size_t soundID, bool isPaused);

			/// <summary>
			/// Check if a sound is paused or not by its index.
			/// </summary>
			/// <param name="soundID">ID of the sound to check</param>
			/// <returns>True if the sound is paused, false if not</returns>
			FRANAUDIO_CLIENT_API bool IsPaused(size_t soundID);

			/// <summary>
			/// Set the volume of a playing sound by its ID.
			/// </summary>
			/// <param name="soundID">ID of the sound to set the volume of</param>
			/// <param name="volume">Volume to set the sound to (0.0 - 1.0)</param>
			FRANAUDIO_CLIENT_API void SetVolume(size_t soundID, float volume);

			/// <summary>
			/// Get the volume of a playing sound by its ID.
			/// </summary>
			/// <param name="soundID">ID of the sound to get the volume of</param>
			/// <returns>Volume of the sound (0.0 - 1.0)</returns>
			FRANAUDIO_CLIENT_API float GetVolume(size_t soundID);

			/// <summary>
			/// Set the position of a playing sound by its ID.
			/// </summary>
			/// <param name="soundID">ID of the sound to set the position of</param>
			/// <param name="position">Position to set the sound to</param>
			FRANAUDIO_CLIENT_API void SetPosition(size_t soundID, float position[3]);

			/// <summary>
			/// Get the position of a playing sound by its ID.
			/// </summary>
			/// <param name="soundID">ID of the sound to get the position of</param>
			/// <param name="position">Output position of the sound</param>
			FRANAUDIO_CLIENT_API void GetPosition(size_t soundID, float position[3]);
		}
	}
}
