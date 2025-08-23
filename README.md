# Frantic's Audio (aka. FranAudio or FranticAudio)

A basic audio library made mainly for games and game engines that utilises C++23.  
**Not fully complete yet**, but will be very soon.

(This is NOT a continuation of Sprinity's FranAudio extension, but will be a replacement eventually.)

# Features:
- Modular Backend Support with `FranAudio::Backend::Backend` Interface  
- Optional High-Level Server-Client Communication (localhost) for Inter-Process Usage (Mainly for Game Modding)  
- **WAV**, **MP3**, **FLAC** and *(optional)* **Vorbis** Support
- Optional extensive logging for both developers and end users.

# To-do:
- Extend Server-Client Communication  
- Dynamic Positional Audio  
- Post-Processing Effects  
- OpenAL Backend  
- Linux Support  
- Opus Support  
- C API  

# Modules:
<b>FranAudioShared</b> - Contains the shared modules for both client-server and the library itself.  
- FranAudioShared::<b>Logger</b> - The module that contains the logging functionality for the library.  
- FranAudioShared::<b>Network</b> - The module that contains the network helpers and constants for the library.  

### FranAudio Library Structure
<b>FranAudio</b> - The main module that contains the core functionality of the library.  
- FranAudio::<b>Backend</b> - The module that contains the backend interface and the default backend implementation.  
    - FranAudio::Backend::<b>Miniaudio</b> - The module that contains the miniaudio backend implementation.  
    - FranAudio::Backend::<b>OpenAL</b> - The module that contains the OpenAL backend implementation (not implemented yet).  
- FranAudio::<b>Decoder</b> - The module that contains the audio decoders for various formats.  
    - FranAudio::Decoder::<b>Miniaudio</b> - The module that contains the miniaudio decoder implementation.  
    - FranAudio::Decoder::<b>Libnyquist</b> - The module that contains the libnyquist decoder implementation.  

### FranAudio High-Level Server-Client Structure
<b>FranAudioClient</b> - Library module that contains the client-side functionality for standalone server-client system. To be linked with target applications.
<b>FranAudioServer</b> - Executable module that contains the server-side functionality for standalone server-client system.  

### FranAudio Test Application
<b>FranAudioTest</b> - Module that contains the test application for the library. Can test both library and server-client system.  

# Basic Usage
Since the project is in the initial stages, you may try to use it as a static or dynamic library as you may use any other one. But I suggest waiting for the initial release of the library. Please simply add this library to your watchlist and be patient. Thank you for your time.

But as a proof of concept, you can [build](#requirements-and-building) and [test](#testing) the project using the instructions below.

# Requirements and Building:
For now, only Windows is supported. In near future, Linux support will be added.

### Windows
#### Requirements:
- CMake 3.20 or newer
- A Compiler that supports C++23

#### Build Steps for Visual Studio 2022 (MSVC)
- Get the code
    - (Recommended) Use git to clone the repo (For example, using `git clone https://github.com/edgarbarney/FranAudio`)
    - Download the code using the `<> Code` button and `Download ZIP` option.
- Open the directory using  Visual Studio 2022
    - You can use `File > Open > Folder`
    - You can use the `Open a local folder` button on the Welcome Screen
    - You can right-click on the folder, click on the `Open with Visual Studio 2022` option, if you have context-menus activated for VS022
- Choose your desired profile and build the project.
    - You can use the `F7` build shortcut.
    - You can use `Build > Build All`

Upon completing the build process, your binaries will be in `FranAudio/out/build/{build_profile}/bin`


# Testing
After you've done [building](#requirements-and-building) the project, you can test if it works or not with the test app. 

- Get some test files.
    - They should be similar to [these](https://github.com/edgarbarney/FranAudio_TestFiles "miniaudio").
    - Or you can just directly [download](https://github.com/edgarbarney/FranAudio_TestFiles/releases/download/v1.0.0/FranAudio_TestFiles.7z "Download Test Files") them.
- Put them into the same folder as the binaries.
    - e.g. ``FranAudio/out/build/{build_profile}/bin`

- Simply run the `FranAudio_Server` and wait for it to tell you it's waiting for messages. 

- Run `FranAudio_Test` and switch back to the `FranAudio_Server` window. 

After these steps are done, follow the instructions on the `FranAudio_Server` window. If you hear a sound, congratulations! You've successfully built the library!

# List of used external libraries:   
[miniaudio](https://miniaud.io/ "miniaudio") - As Backend and Decoder  
[libnyquist](https://github.com/ddiakopoulos/libnyquist "libnyquist") - As Decoder  
[GLFW](https://www.glfw.org "GLFW") - For Test Application
[GLAD](https://github.com/Dav1dde/glad "GLAD") - For Test Application
[ImGui](https://github.com/ocornut/imgui "Dear ImGui") - As Test Application GUI

Libraries can be modified in one way or another. 
So it's not advised to just replace them when updating.
Try to do a folder-by-folder diff with a tool like WinMerge or Meld.
