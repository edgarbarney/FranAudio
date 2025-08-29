// FranticDreamer 2022-2025
#pragma once

#include <string>
#include <vector>

#include "Backend/BackendTypes.hpp"

#include "FranAudioShared/Containers/UnorderedMap.hpp"
#include "Decoder/Decoder.hpp"
#include "Sound/WaveData/WaveData.hpp"
#include "Sound/Sound.hpp"

namespace FranAudio::Backend
{
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
		FranAudioShared::Containers::UnorderedMap<std::string, size_t> filenameWaveMap;

		/// <summary>
		/// Currently Active Sounds
		/// Tied to nextSoundID
		/// </summary>
		FranAudioShared::Containers::UnorderedMap<size_t, FranAudio::Sound::Sound> activeSounds;

		// ========================
		// Forced Decode Settings
		// ========================

		/// <summary>
		/// Forced format for decoding audio files.
		/// WaveFormat::Unknown means no forced format.
		/// </summary>
		FranAudio::Sound::WaveFormat forcedFormat = FranAudio::Sound::WaveFormat::Unknown;

		/// <summary>
		/// Forced number of channels for decoding audio files.
		/// 0 means no forced channels.
		/// </summary>
		char forcedChannels = 0;

		/// <summary>
		/// Forced sample rate for decoding audio files.
		/// 0 means no forced sample rate.
		/// </summary>
		int forcedSampleRate = 0;

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

		// ========================
		// Decoder Management
		// ========================

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
		/// <param name="force">Force the decoder to be replaced even if it's the same type</param>
		void SetDecoder(FranAudio::Decoder::DecoderType decoderType, bool force = false);

		/// <summary>
		/// Destroy the current decoder.
		/// </summary>
		void DestroyDecoder();

		/// <summary>
		/// Sets forced decode format for decoding.
		/// </summary>
		/// <param name="format">Forced format to be used in decoding</param>
		void SetForcedDecodeFormat(FranAudio::Sound::WaveFormat format);

		/// <summary>
		/// Gets forced decode format for decoding.
		/// </summary>
		/// <returns>The forced decode format</returns>
		FranAudio::Sound::WaveFormat GetForcedDecodeFormat() const;

		/// <summary>
		/// Sets the number of channels to be used in decoding.
		/// </summary>
		/// <param name="channels">Forced number of channels in decoding</param>
		void SetForcedDecodeChannels(char channels);

		/// <summary>
		/// Gets forced number of channels for decoding.
		/// </summary>
		/// <returns>The forced number of channels</returns>
		char GetForcedDecodeChannels() const;

		/// <summary>
		/// Sets forced sample rate for decoding.
		/// </summary>
		/// <param name="sampleRate">Forced sample rate to be used for decoding</param>
		void SetForcedDecodeSampleRate(int sampleRate);

		/// <summary>
		/// Gets forced sample rate for decoding.
		/// </summary>
		/// <returns>The forced sample rate</returns>
		int GetForcedDecodeSampleRate() const;

		// ========================
		// Listener (3D Audio)
		// ========================

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

		// ========================
		// Audio File Management
		// ========================

		/// <summary>
		/// Play an audio file that is already loaded into memory.
		/// </summary>
		/// <param name="waveData">Wave data to play</param>
		/// <returns>Active Sounds List Index</returns>
		virtual size_t PlayAudioWave(const FranAudio::Sound::WaveData& waveData) = 0;

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
		/// Play an audio file without loading it, stream it from the disk.
		/// This is used to play an audio file without loading it into memory.
		/// </summary>
		/// <param name="filename">Path to the audio file</param>
		/// <returns>Active Sounds List Index</returns>
		virtual size_t PlayAudioFileStream(const std::string& filename) = 0;

		// ========================
		// Sound Management
		// ========================

		/// <summary>
		/// Check if a sound is valid by its index.
		/// </summary>
		/// <param name="soundID">ID of the sound in to check</param>
		virtual bool IsSoundValid(size_t soundID);

		/// <summary>
		/// Stop and clear an active sound by its index.
		/// </summary>
		/// <param name="soundID">ID of the sound in to stop</param>
		virtual void StopPlayingSound(size_t soundID) = 0;

		/// <summary>
		/// Set whether the sound is paused or not by its ID.
		/// </summary>
		/// <param name="soundID">ID of the sound to modify</param>
		/// <param name="isPaused">True to pause the sound, false to resume playback</param>
		virtual void SetSoundPaused(size_t soundID, bool isPaused) = 0;

		/// <summary>
		/// Check if a sound is paused or not by its index.
		/// </summary>
		/// <param name="soundID">ID of the sound to check</param>
		/// <returns>True if the sound is paused, false if not</returns>
		virtual bool IsSoundPaused(size_t soundID) = 0;

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
		/// Index MUST be valid. Check with IsSoundValid() first.
		/// </summary>
		/// <param name="soundID">ID of the sound to get</param>
		virtual Sound::Sound& GetSound(size_t soundID);
		
		/// <summary>
		/// Get the map of currently active sounds.
 		/// </summary>
 		/// <returns>Map of currently active sounds</returns>
		virtual const FranAudioShared::Containers::UnorderedMap<size_t, Sound::Sound>& GetActiveSounds() const;

		/// <summary>
		/// Retrieves a list of active sound IDs.
		/// </summary>
		/// <returns>A vector containing the IDs of currently active sounds.</returns>
		virtual const std::vector<size_t> GetActiveSoundIDs() const;

		// ========================
		// Backend
		// ========================

		/// <summary>
		/// Create a backend instance.
		/// </summary>
		/// <param name="backendType">Type of the backend to create</param>
		/// <returns>Pointer to the created backend instance</returns>
		static Backend* CreateBackend(BackendType backendType);
	};
}
