// FranticDreamer 2022-2025

#include "Backend/Backend.hpp"
#include "Backend/miniaudio/Backend_miniaudio.hpp"

#include "FranAudio.hpp"

FranAudio::GlobalData gGlobals;

FRANAUDIO_API void FranAudio::Init()
{
	SetBackend(defaultBackend);
}

FRANAUDIO_API void FranAudio::Reset()
{
	gGlobals.currentBackend->Reset();
}

FRANAUDIO_API void FranAudio::Shutdown()
{
	gGlobals.currentBackend->Shutdown();
	delete gGlobals.currentBackend;
	gGlobals.currentBackend = nullptr;
}

FRANAUDIO_API void FranAudio::SetBackend(Backend::BackendType type)
{
	if (gGlobals.currentBackend)
	{
		gGlobals.currentBackend->Shutdown();
		delete gGlobals.currentBackend;
		gGlobals.currentBackend = nullptr;
	}

	gGlobals.currentBackend = Backend::Backend::CreateBackend(type);
}

FRANAUDIO_API FranAudio::Backend::Backend* FranAudio::GetBackend()
{
	return gGlobals.currentBackend;
}
