// FranticDreamer 2022-2025
#pragma once

// TODO: Change file extension to .h because this can also be used in C

// DLL Exporting API Defines

#ifdef FRANAUDIO_CLIENT_DYNAMIC
#define FRANAUDIO_CLIENT_API __declspec(dllexport)
#else
#define FRANAUDIO_CLIENT_API __declspec(dllimport)
#endif

