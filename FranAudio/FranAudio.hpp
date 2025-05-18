// FranticDreamer 2022-2024
#pragma once

#include "Backend/Backend.hpp"

#include "FranAudioAPI.hpp"

/*
namespace GameEngine
{
	class Vector;
	class Entity;
}
*/

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

	FRANAUDIO_API void Init();
	FRANAUDIO_API void Reset();
	FRANAUDIO_API void Shutdown();

	FRANAUDIO_API void SetBackend(Backend::BackendType type);

	FRANAUDIO_API Backend::Backend* GetBackend();
}

/*
/// <summary>
/// Engine specific code.
/// 
/// This is a placeholder for the engine specific code.
/// You should either replace this with your engine's code
/// or use this as a wrapper around your engine's code.
/// 
/// This code is used to demonstrate how to integrate 
/// the FranAudio library with your engine.
/// </summary>
namespace GameEngine
{
	/// <summary>
	/// A simple 3D vector class.
	/// 
	/// This is a placeholder for the engine specific vector class.
	/// 
	/// You should either replace this with your engine's vector class
	/// or use this as a wrapper around your engine's vector class.
	/// </summary>
	class Vector
	{
	public:
		union
		{
			struct
			{
				float x, y, z;
			};
			float components[3];
		};

		Vector() : x(0), y(0), z(0) {}
		Vector(float x, float y, float z) : x(x), y(y), z(z) {}

		//Operator to return float array of components
		operator float* () { return components; }
	};

	/// <summary>
	/// A simple entity class.
	/// 
	/// Your engine's entity class should have similar functions.
	/// 
	/// You should either replace this with your engine's entity class
	/// or use this as a wrapper around your engine's entity class.
	/// </summary>
	class Entity
	{
	private:
		FranAudio::EntityHandle id;
		Vector position;
	public:
		Entity() { static FranAudio::EntityHandle globalid = 0; id = globalid++; position = Vector(); }
		virtual FranAudio::EntityHandle GetID() { return 0; }
		virtual Vector GetPosition() { return position; }
		virtual bool IsPlayer() { return false; }
		virtual void SetPosition(Vector pos) { position = pos; }
	};
}
*/