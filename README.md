# Frantic's Audio (aka. FranAudio or FranticAudio)

A basic audio library made mainly for games and game engines that utilises C++23.  
**Not fully complete yet**, but will be very soon.

(This is NOT a continuation of Sprinity's FranAudio extension, but will be a replacement eventually.)

# Features:
- Modular Backend Support with `FranAudio::Backend::Backend` Interface  
- Local Server-Client Communication for Inter-Process Usage (Mainly for Game Modding)  
- **WAV**, **MP3**, **FLAC** and *(optional)* **Vorbis** Support
- Optional extensive logging for both developers and end users.

# To-do:
- Extend Server-Client Communication to Full Control  
- Dynamic Positional Audio  
- Post-Processing Effects  
- OpenAL Backend  
- Linux Support  
- Opus Support  
- C API  

# List of used external libraries:   
[miniaudio](https://miniaud.io/ "miniaudio") - As Backend and Decoder  
[libnyquist](https://github.com/ddiakopoulos/libnyquist "libnyquist") - As Decoder  

Libraries can be modified in one way or another. 
So it's not advised to just replace them when updating.
Try to do a folder-by-folder diff with a tool like WinMerge or Meld.
