// FranticDreamer 2022-2025
#pragma once

#include "Backend/Backend.hpp"

#include "FranAudioAPI.hpp"

namespace FranAudio
{
	/// <summary>
	/// Entity handle type.
	/// You may change this to your engine's entity handle type if needed.
	/// </summary>
	using EntityHandle = size_t;

	/// <summary>
	/// Unique ID type for entities, sounds, etc.
	/// </summary>
	using UniqueID = size_t;

	inline const Backend::BackendType defaultBackend = Backend::BackendType::miniaudio;

	class GlobalData
	{
	public:
		inline static Backend::Backend* currentBackend = nullptr;
	};

	extern GlobalData gGlobals;

	/// <summary>
	/// Initializes the FranAudio library.
	/// </summary>
	FRANAUDIO_API void Init();

	/// <summary>
	/// Resets the FranAudio library.
	/// This is used to reset the library to its initial state.
	/// </summary>
	FRANAUDIO_API void Reset();

	/// <summary>
	/// Shuts down the FranAudio library and cleans up resources.
	/// </summary>
	FRANAUDIO_API void Shutdown();

	/// <summary>
	/// Sets the audio backend to use.
	/// </summary>
	/// <param name="type">The backend type to set, specified as a value of Backend::BackendType.</param>
	/// <returns>This function does not return a value.</returns>
	FRANAUDIO_API void SetBackend(Backend::BackendType type);

	/// <summary>
	/// Get the current backend.
	/// 
	/// <para>
	/// NOTE: Do not cache the return of this function.
	/// It may change if the decoder is changed or destroyed.
	/// </para>
	/// 
	/// </summary>
	/// <returns>Pointer to the current backend</returns>
	FRANAUDIO_API Backend::Backend* GetBackend();
}
