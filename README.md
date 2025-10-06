# Frantic's Audio (aka. FranAudio or FranticAudio)

A basic audio library made mainly for games and game engines that utilises C++23.  
**Not fully complete yet**, but will be very soon.

(This is NOT a continuation of Sprinity's FranAudio extension, but will be a replacement eventually.)

# Features  
- Modular Backend Support with `FranAudio::Backend::Backend` Interface  
- **Miniaudio** and **OpenAL-Soft** Backends  
- Optional High-Level Local Server-Client Communication for Inter-Process Usage (Mainly for Game Modding)  
- **WAV**, **MP3**, **FLAC** support, along with *optional* **Vorbis** and **Opus** extensions.
- Optional extensive logging for both developers and end users.
- Dynamic Positional Audio

# To-Do  
- Post-Processing Effects  
- Linux Support  
- C API  

# Modules  
### Shared Structure  
<b>FranAudioShared</b> - Contains the shared modules for both client-server and the library itself.  
- FranAudioShared::<b>Logger</b> - The module that contains the logging functionality for the library.  
- FranAudioShared::<b>Network</b> - The module that contains the network helpers and constants for the library.  
- FranAudioShared::<b>Containers</b> - The module that contains the container classes for the library.  

### FranAudio Library Structure  
<b>FranAudio</b> - The main module that contains the core functionality of the library.  
- FranAudio::<b>Backend</b> - The module that contains the backend abstract and various backends.  
    - FranAudio::Backend::<b>Miniaudio</b> - The module that contains the miniaudio backend implementation.  
    - FranAudio::Backend::<b>OpenAL</b> - The module that contains the OpenAL backend implementation.  
- FranAudio::<b>Decoder</b> - The module that contains the decoder abstract and various decoders.  
    - FranAudio::Decoder::<b>Miniaudio</b> - The module that contains the miniaudio decoder implementation.  
    - FranAudio::Decoder::<b>Libnyquist</b> - The module that contains the libnyquist decoder implementation.  

### FranAudio High-Level Server-Client Structure  
<b>FranAudioClient</b> - Library module that contains the client-side functionality for standalone server-client system. To be linked with target applications.  
<b>FranAudioServer</b> - Executable module that contains the server-side functionality for standalone server-client system.  

### FranAudio C API Structure (Unfinished)  
<b>FranAudioLegacy</b> - Module that contains the C API for the library. To be used in C projects or other languages that support C bindings.  

### FranAudio Test Application  
<b>FranAudioTest</b> - Module that contains the test application for the library. Can test both library and server-client system.  

# Basic Usage  
Basic usage of the library is as follows:
```cpp
#include "FranAudio/FranAudio.h"

// TODO
```

You can also see the `FranAudioTest/FranAudioTest.cpp`

# Requirements and Building  
## Requirements  
- CMake 3.23 or newer
- A Compiler that supports C++23

## Building on Linux  
For now, only Windows is supported. In the near future, Linux support will be added.  

## Building on Windows  
#### Build and Install Steps for Visual Studio (MSVC)  
- Get the code
    - (Recommended) Use git to clone the repository (for example, using `git clone https://github.com/edgarbarney/FranAudio`)
    - Download the code using the `<> Code` button and `Download ZIP` option.
- Open the directory using Visual Studio
    - You can use `File > Open > Folder`
    - You can use the `Open a local folder` button on the Welcome Screen
    - You can right-click on the folder, click on the `Open with Visual Studio` option, if you have context menus activated for Visual Studio
- Choose your desired profile and build the project.
    - You can use the `F7` build shortcut.
    - You can use `Build > Build All`
- After choosing your desired profile, you can also install the project.
    - You can use `Build > Install FranAudio`

Upon completing the build process:  
 your binaries will be in `FranAudio/out/build/{build_profile}/bin`  

Upon completing the installation:  
your binaries will be in `FranAudio/out/install/{build_profile}/bin`  
your headers will be in `FranAudio/out/install/{build_profile}/include`  
your libraries will be in `FranAudio/out/install/{build_profile}/lib`  


# Testing  
After you've [built](#requirements-and-building) the project, you can test if it works with the test application. Instead of unit tests, you should use the application manually, since interactivity and 3D audio is the main feature.

- Get some test files.
    - They should be similar to [these](https://github.com/edgarbarney/FranAudio_TestFiles "miniaudio").
    - Or you can just [download](https://github.com/edgarbarney/FranAudio_TestFiles/releases/download/v1.0.0/FranAudio_TestFiles.7z "Download Test Files") them.

- Run `FranAudio_Test` and use the UI to test out the library.  

# Libraries  

## Vendored  
[miniaudio](https://miniaud.io/ "miniaudio") - As Backend and Decoder  
[libnyquist](https://github.com/ddiakopoulos/libnyquist "libnyquist") - As Decoder  
[ImGui](https://github.com/ocornut/imgui "Dear ImGui") - As Test Application GUI  
[imgui-filebrowser](https://github.com/AirGuanZ/imgui-filebrowser "imgui-filebrowser") - Extension for ImGui for file browsing  
[GLAD](https://github.com/Dav1dde/glad "GLAD") - For Test Application GUI  
#### Optional  
[ankerl::unordered_dense](https://github.com/martinus/unordered_dense "ankerl::unordered_dense") - As STL Unordered Map Replacement   
[vector.hpp from chobo-shl](https://github.com/Chobolabs/chobo-shl "Chobolabs chobo-shl") - As STL Vector Replacement  

## Fetched (All Optional - Will be Fetched by CMake Unless Disabled)    
[OpenALSoft](https://github.com/kcat/openal-soft "OpenALSoft") - As Backend  
[libogg](https://xiph.org/downloads/ "Xiph libogg") - Vorbis library depends on this.  
[libvorbis](https://xiph.org/downloads/ "Xiph libvorbis") - Used for OGG/Vorbis support.  
[libopus](https://opus-codec.org/ "Opus Codec") - Opusfile library depends on this.  
[libopusfile](https://opus-codec.org/ "Opus Codec") - Used for Opus support.  
[GLFW](https://www.glfw.org "GLFW") - For Test Application GUI  

### Notes  
Libraries can be modified in one way or another.  
So it's not advisable to just replace them drop-in when updating.  
Try to do a folder-by-folder diff with a tool like WinMerge or Meld.  

