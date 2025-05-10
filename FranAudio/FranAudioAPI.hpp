// FranticDreamer 2022-2024
#pragma once

// TODO: Change file extension to .h because this can also be used in C

// DLL Exporting API Defines

#ifdef FRANAUDIO_DYNAMIC
#define FRANAUDIO_API __declspec(dllexport)
#elif defined FRANAUDIO_IGNORE
#define FRANAUDIO_API
#else
#define FRANAUDIO_API __declspec(dllimport)
#endif

// If FranAudio will run on it's seperate proccess
// We can use IPC actions.
#ifdef FRANAUDIO_USE_SERVER
// TODO: Different Defines?
#endif
