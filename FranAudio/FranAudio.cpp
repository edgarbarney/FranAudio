// FranticDreamer 2022-2025

#include "Backend/Backend.hpp"
#include "Backend/miniaudio/Backend_miniaudio.hpp"

#include "FranAudio.hpp"

namespace FranAudio
{
	GlobalData gGlobals;

	FRANAUDIO_API void Init()
	{
		gGlobals.currentBackend = nullptr; // Initialise to null
		SetBackend(defaultBackend);
	}

	FRANAUDIO_API void Reset()
	{
		gGlobals.currentBackend->Reset();
	}

	FRANAUDIO_API void Shutdown()
	{
		gGlobals.currentBackend->Shutdown();
		gGlobals.currentBackend.reset();
	}

	FRANAUDIO_API void RouteLoggingToConsole(FranAudioShared::Logger::ConsoleStreamBuffer* consoleBuffer)
	{
		FranAudioShared::Logger::RouteToConsole(consoleBuffer);
	}

	FRANAUDIO_API bool IsBackendValid()
	{
		return gGlobals.currentBackend != nullptr;
	}

	FRANAUDIO_API void SetBackend(Backend::BackendType type)
	{
		if (gGlobals.currentBackend)
		{
			// Might be identical to Shutdown() 
			// but reserved for future changes
			gGlobals.currentBackend->Shutdown();
			gGlobals.currentBackend.reset();
		}

		gGlobals.currentBackend = Backend::Backend::CreateBackend(type);
		gGlobals.currentBackend->SetDecoder(gGlobals.currentBackend->GetDecoderType(), true); // Initialize with default decoder
		return;
	}

	FRANAUDIO_API Backend::Backend* GetBackend()
	{
		return gGlobals.currentBackend.get();
	}
}
