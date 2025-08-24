// FranticDreamer 2022-2025
#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "Decoder/Decoder.hpp"
#include "Sound/WaveData/WaveData.hpp"
#include "Sound/Sound.hpp"

namespace FranAudio::Backend
{
	/// <summary>
	/// Possible backend types.
	/// </summary>
	enum class BackendType
	{
		None = 0,
		miniaudio,
		OpenALSoft,
	};

	/// <summary>
	/// Interface for backend implementations.
	/// </summary>
	class Backend
	{
	protected:
		FranAudio::Decoder::Decoder* currentDecoder = nullptr;
		FranAudio::Decoder::DecoderType currentDecoderType = FranAudio::Decoder::DecoderType::None;

		/// <summary>
		/// Next Sound ID to be used.
		/// This is used to generate unique IDs for sounds.
		/// </summary>
		size_t nextSoundID = 0;

		/// <summary>
		/// Cache for decoded audio data.
		/// This is used to cache the decoded audio data to avoid decoding every time the audio is played.
		/// </summary>
		std::vector<FranAudio::Sound::WaveData> waveDataCache;

		/// <summary>
		/// Map for finding decoded audio data in cache by filename.
		/// This is used to evade a lookup in the vector.
		/// </summary>
		std::unordered_map<std::string, size_t> filenameWaveMap;

		/// <summary>
		/// Currently Active Sounds
		/// Tied to nextSoundID
		/// </summary>
		std::unordered_map<size_t, FranAudio::Sound::Sound> activeSounds;

	public:
		Backend() = default;
		~Backend();

		/// <summary>
		/// Initialise the backend.
		/// This is used to initialise the backend and set it up for use.
		/// </summary>
		virtual bool Init(FranAudio::Decoder::DecoderType decoderType = FranAudio::Decoder::DecoderType::None) = 0;

		/// <summary>
		/// Reset the backend. 
		/// This is used to reset the backend to its initial state.
		/// </summary>
		virtual void Reset() = 0;

		/// <summary>
		/// Shutdown the backend.
		/// This is used to shutdown the backend and clean up any resources.
		/// </summary>
		virtual void Shutdown() = 0;

		/// <summary>
		/// Get the backend type.
		/// </summary>
		/// <returns>Type of this Backend instance</returns>
		virtual constexpr BackendType GetBackendType() const noexcept;

		// TODO: Should we make the supported decoders a bitmap or bool array?
		/// <summary>
		/// Get the supported decoders.
		/// </summary>
		/// <returns>List of supported decoders</returns>
		virtual const std::vector<FranAudio::Decoder::DecoderType>& GetSupportedDecoders() const = 0;

		/// <summary>
		/// Get the decoder type.
		/// </summary>
		/// <returns>Type of the current decoder used by this backend</returns>
		FranAudio::Decoder::DecoderType GetDecoderType() const;

		/// <summary>
		/// Get the current decoder.
		/// 
		/// <para>
		/// NOTE: Do not cache the return of this function.
		/// It may change if the decoder is changed or destroyed.
		/// </para>
		/// 
		/// </summary>
		/// <returns>Pointer to the current decoder used by this backend</returns>
		FranAudio::Decoder::Decoder* GetCurrentDecoder() const;

		/// <summary>
		/// Set the decoder type.
		/// </summary>
		/// <param name="decoderType">Type of the decoder to replace the current one</param>
		void SetDecoder(FranAudio::Decoder::DecoderType decoderType);

		/// <summary>
		/// Destroy the current decoder.
		/// </summary>
		void DestroyDecoder();

		/// <summary>
		/// Set the listener's position and orientation.
		/// </summary>
		/// <param name="position">New position of the listener</param>
		/// <param name="forward">New forward vector of the listener</param>
		virtual void SetListenerTransform(const float position[3], const float forward[3], const float up[3]) = 0;

		/// <summary>
		/// Get the listener's position and orientation.
		/// </summary>
		/// <param name="position">Output position of the listener</param>
		/// <param name="forward">Output forward vector of the listener</param>
		/// <param name="up">Output up vector of the listener</param>
		virtual void GetListenerTransform(float outPosition[3], float outForward[3], float outUp[3]) = 0;

		/// <summary>
		/// Set the listener's position.
		/// </summary>
		/// <param name="position">New position of the listener</param> 
		virtual void SetListenerPosition(const float position[3]) = 0;

		/// <summary>
		/// Get the listener's position.
	 	/// </summary>
		/// <param name="position">Output position of the listener</param>
		virtual void GetListenerPosition(float outPosition[3]) = 0;

		/// <summary>
		/// Set the listener's orientation.
		/// </summary>
		/// <param name="forward">New forward vector of the listener</param>
		/// <param name="up">New up vector of the listener</param>
		virtual void SetListenerOrientation(const float forward[3], const float up[3]) = 0;

		/// <summary>
		/// Get the listener's orientation.
		/// </summary>
		/// <param name="forward">Output forward vector of the listener</param>
		/// <param name="up">Output up vector of the listener</param>
		virtual void GetListenerOrientation(float outForward[3], float outUp[3]) = 0;

		/// <summary>
		/// Set the master volume.
		/// Can also be the listener's hearing volume.
		/// </summary>
	 	/// <param name="volume">Volume to set the master volume to (0.0 - 1.0)</param>
		virtual void SetMasterVolume(float volume) = 0;

		/// <summary>
		/// Get the master volume.
 		/// Can also be the listener's hearing volume.	
		/// </summary>
		virtual float GetMasterVolume() = 0; // Not const because some audio backends might require non-const pointer.

		/// <summary>
		/// Decode an audio file and load it into the memory.
		/// </summary>
		/// <param name="filename">Path to the audio file</param>
		/// <returns>Wave Data Cache Index</returns>
		virtual size_t LoadAudioFile(const std::string& filename) = 0;

		/// <summary>
		/// Play an audio file after checking if it's loaded.
		/// If the audio file is not loaded, it will be loaded and then played.
		/// </summary>
		/// <param name="filename">Path to the audio file</param>
		/// <returns>Active Sounds List Index</returns>
		virtual size_t PlayAudioFile(const std::string& filename) = 0;

		/// <summary>
		/// Play an audio file without checking if it's loaded.
		/// If the audio file is not loaded, it won't be played and will be ignored.
		/// </summary>
		/// <param name="filename">Path to the audio file</param>
		/// <returns>Active Sounds List Index</returns>
		virtual size_t PlayAudioFileNoChecks(const std::string& filename) = 0;

		/// <summary>
		/// Play an audio file without loading it, stream it from the disk.
		/// This is used to play an audio file without loading it into memory.
		/// </summary>
		/// <param name="filename">Path to the audio file</param>
		/// <returns>Active Sounds List Index</returns>
		virtual size_t PlayAudioFileStream(const std::string& filename) = 0;

		/// <summary>
		/// Check if a sound is valid by its index.
		/// </summary>
		/// <param name="soundIndex">Index of the sound in the active sounds list</param>
		virtual bool IsSoundValid(size_t soundIndex);

		/// <summary>
		/// Stop and clear an active sound by its index.
		/// </summary>
		/// <param name="soundIndex">Index of the sound in the active sounds list</param>
		virtual void StopPlayingSound(size_t soundIndex) = 0;

		/// <summary>
		/// Set the volume of a playing sound by its index.
		/// </summary>
		/// <param name="soundID">ID of the sound to set the volume of</param>
		/// <param name="volume">Volume to set the sound to (0.0 - 1.0)</param>
		virtual void SetSoundVolume(size_t soundID, float volume) = 0;

		/// <summary>
		/// Get the volume of a playing sound by its index.
	 	/// </summary>
		/// <param name="soundID">ID of the sound to get the volume of</param>
		/// <returns>Volume of the sound (0.0 - 1.0)</returns>
		virtual float GetSoundVolume(size_t soundID) = 0;

		/// <summary>
		/// Set the position of a playing sound by its index.
		/// </summary>
 		/// <param name="soundID">ID of the sound to set the position of</param>
 		/// <param name="position">Position to set the sound to</param>
		virtual void SetSoundPosition(size_t soundID, const float position[3]) = 0;

		/// <summary>
		/// Get the position of a playing sound by its index.
		/// </summary>
 		/// <param name="soundID">ID of the sound to get the position of</param>
	 	/// <param name="position">Output position of the sound</param>
		virtual void GetSoundPosition(size_t soundID, float position[3]) = 0;

		/// <summary>
		/// Get a reference to a playing sound by its index.
		/// </summary>
		/// <param name="soundID">ID of the sound to get</param>
		virtual Sound::Sound& GetSound(size_t soundID);
		
		/// <summary>
		/// Get the map of currently active sounds.
 		/// </summary>
 		/// <returns>Map of currently active sounds</returns>
		virtual const std::unordered_map<size_t, Sound::Sound>& GetActiveSounds() const;

		/// <summary>
		/// Create a backend instance.
		/// </summary>
		/// <param name="backendType">Type of the backend to create</param>
		/// <returns>Pointer to the created backend instance</returns>
		static Backend* CreateBackend(BackendType backendType);
	};
}
