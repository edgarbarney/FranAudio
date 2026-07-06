// FranticDreamer 2022-2025
#pragma once

#include <memory>
#include <array>
#include <tuple>
#include <span>

#include "FranAudioAPI.hpp"

/// <summary>
/// Contains shared code for the FranAudio library itself, Server and Client.
/// </summary>
/// <remarks>
/// This is a separate project to avoid circular dependencies.
/// Contains classes and functions for Networking, Logging, Containers and other shared utilities.
/// </remarks>
namespace FranAudioShared
{
	// ========================
	// General Data Start
	// 
	// TODO: Maybe move these to shared or somewhere more fitting.
	// ========================

	/// <summary>
	/// 3D Vector type used for sound positions, listener positions, etc.
	/// </summary>
	FRANAUDIO_API struct Vector3
	{
		float x;
		float y;
		float z;
	};

	/// <summary>
	/// Tuple-like type for listener transform, containing position, forward vector, and up vector.
	/// </summary>
	FRANAUDIO_API struct ListenerTransform
	{
		Vector3 position;
		Vector3 forward;
		Vector3 up;
	};

	/// <summary>
	/// Tuple-like type for listener orientation, containing forward vector and up vector.
	/// </summary>
	FRANAUDIO_API struct ListenerOrientation
	{
		Vector3 forward;
		Vector3 up;
	};

	/// <summary>
	/// Tuple-like type for sound attenuation parameters, containing rolloff factor, minimum distance, and maximum distance.
	/// </summary>
	FRANAUDIO_API struct SoundAttenuation
	{
		float rolloffFactor;
		float minDistance;
		float maxDistance;
	};

	// ========================
	// General Data End
	// ========================
}
