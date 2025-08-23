// FranticDreamer 2022-2025
#pragma once

//#ifdef FRANAUDIO_USE_VORBIS
#include "miniaudio/extras/decoders/libvorbis/miniaudio_libvorbis.h"
//#endif

#include "Backend/Backend.hpp"
#include "Sound/Sound.hpp"
#include "Sound/WaveData/WaveData.hpp"

namespace FranAudio::Backend
{
	inline ma_decoding_backend_vtable* miniaudio_backendVTables[] =
	{
	#ifdef FRANAUDIO_USE_VORBIS
		ma_decoding_backend_libvorbis,
	#endif
	#ifdef FRANAUDIO_USE_OPUS
		ma_decoding_backend_libopus,
	#endif
	// If neither vorbis or opus are defined, this will be nullptr.
	#if !defined(FRANAUDIO_USE_VORBIS) && !defined(FRANAUDIO_USE_OPUS)
		nullptr,
	#endif
	};

	class miniaudio : public Backend
	{
	private:
		ma_engine engine = {};
		ma_engine_config engineConfig = {};
		ma_device device = {};
		ma_device_config deviceConfig = {};
		ma_decoder_config defaultDecoderConfig = {};

		// ==========
		// VORBIS
		// ==========

		ma_libvorbis libvorbis = {};
		
		// ==========
		// OPUS
		// ==========

		//ma_libopus libopus = {};


		/// <summary>
		/// Sound data in a format that can be played by the miniaudio backend.
		/// </summary>
		struct MiniaudioSound
		{
			ma_audio_buffer_config audioBufferConfig = {};
			ma_audio_buffer audioBuffer = {};
			ma_sound sound = {};
		};

		/// <summary>
		/// A map of active sounds' corresponding data in a format that miniaudio can play.
		/// 
		/// This is used for making miniaudio interaction easier.
		/// </summary>
		std::unordered_map<size_t, std::unique_ptr<MiniaudioSound>> miniaudioSoundData;

	public:
		//miniaudio();
		//~miniaudio();

		/// <summary>
		/// Initialise the backend.
		/// This is used to initialise the backend and set it up for use.
		/// </summary>
		virtual bool Init(FranAudio::Decoder::DecoderType decoderType = FranAudio::Decoder::DecoderType::None) override;

		/// <summary>
		/// Reset the backend. 
		/// This is used to reset the backend to its initial state.
		/// </summary>
		virtual void Reset() override;

		/// <summary>
		/// Shutdown the backend.
		/// This is used to shutdown the backend and clean up any resources.
		/// </summary>
		virtual void Shutdown() override;

		/// <summary>
		/// Get the backend type.
		/// </summary>
		/// <returns>Type of this Backend instance</returns>
		virtual constexpr BackendType GetBackendType() const noexcept override { return BackendType::miniaudio; }

		/// <summary>
		/// Get the supported decoders.
		/// </summary>
		/// <returns>List of supported decoders</returns>
		virtual const std::vector<FranAudio::Decoder::DecoderType>& GetSupportedDecoders() const override;

		/// <summary>
		/// Set the listener's position and orientation.
		/// </summary>
		virtual void SetListenerTransform(const float position[3], const float forward[3], const float up[3]) override;

		/// <summary>
		/// Set the listener's position.
		/// </summary>
		virtual void SetListenerPosition(const float position[3]) override;

		/// <summary>
		/// Set the listener's orientation.
		/// </summary>
		virtual void SetListenerOrientation(const float forward[3], const float up[3]) override;

		/// <summary>
		/// Set the master volume.
		/// Can also be the listener's hearing volume.
		/// </summary>
		virtual void SetMasterVolume(float volume) override;

		/// <summary>
		/// Decode an audio file and load it into the memory.
		/// </summary>
		/// <param name="filename">Path to the audio file</param>
		/// <returns>Wave Data Cache Index</returns>
		virtual size_t LoadAudioFile(const std::string& filename) override;

		/// <summary>
		/// Play an audio file after checking if it's loaded.
		/// If the audio file is not loaded, it will be loaded and then played.
		/// </summary>
		/// <param name="filename">Path to the audio file</param>
		/// <returns>Active Sounds List Index</returns>
		virtual size_t PlayAudioFile(const std::string& filename) override;

		/// <summary>
		/// Play an audio file without checking if it's loaded.
		/// If the audio file is not loaded, it won't be played and will be ignored.
		/// </summary>
		/// <param name="filename">Path to the audio file</param>
		/// <returns>Active Sounds List Index</returns>
		virtual size_t PlayAudioFileNoChecks(const std::string& filename) override;

		/// <summary>
		/// Play an audio file without loading it, stream it from the disk.
		/// This is used to play an audio file without loading it into memory.
		/// </summary>
		/// <param name="filename">Path to the audio file</param>
		/// <returns>Active Sounds List Index</returns>
		virtual size_t PlayAudioFileStream(const std::string& filename) override;

		/// <summary>
		/// Stop an active sound by its index.
		/// </summary>
		/// <param name="soundIndex">Index of the sound in the active sounds list</param>
		virtual void StopPlayingSound(size_t soundIndex) override;

		/// <summary>
		/// Set the position of a playing sound by its index.
		/// </summary>
		/// <param name="soundID">ID of the sound to set the position of</param>
		virtual void SetSoundPosition(size_t soundID, const float position[3]) override;

		/// <summary>
		/// Get the default decoder configuration.
		/// </summary>
		/// <returns>Default decoder configuration</returns>
		ma_decoder_config* GetDefaultDecoderConfig();

		/// <summary>
		/// Convert a miniaudio format to a FranAudio format.
		/// </summary>
		/// <param name="format">miniaudio format</param>
		/// <returns>FranAudio format</returns>
		static Sound::WaveFormat ConvertFormat(ma_format format);

		/// <summary>
		/// Convert a FranAudio format to a miniaudio format.
		/// </summary>
		/// <param name="format">FranAudio format</param>
		/// <returns>miniaudio format</returns>
		static ma_format ConvertFormat(Sound::WaveFormat format);
	};
}
