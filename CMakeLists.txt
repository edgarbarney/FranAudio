# FranticDreamer 2022-2025
# CMakeLists file for CMake Configuration

# At least 3.20 Needed for C++23
cmake_minimum_required (VERSION 3.20)

set (SUPPORTED_ARCHITECTURES "x86" "x64")
set (CURDIR ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project ("FranAudio"
	VERSION 0.1.0
	DESCRIPTION "FranAudio - Audio Library, Mainly for Games and Game Engines"
	LANGUAGES CXX C
)

if (CMAKE_SIZEOF_VOID_P EQUAL 8) # 64 Bit
	set (PROJECT_ARCH "x64")
elseif (CMAKE_SIZEOF_VOID_P EQUAL 4) # 32 Bit
	set (PROJECT_ARCH "x86")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# =================
# Options
# =================

option (FRANAUDIO_USE_TEST "Compile an app to test the library" ON)
option (FRANAUDIO_USE_SERVER "Compile as an external app (Server)" OFF)

if (FRANAUDIO_USE_SERVER)
	add_compile_definitions (FRANAUDIO_USE_SERVER)
	add_compile_definitions (FRANAUDIO_SERVER_DEBUG)
endif()

# =================
# Output Settings
# =================

set (CMAKE_INCLUDE_CURRENT_DIR ON)

# Debug
# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Windows only pre-processor defines
#if (MSVC) 
if (WIN32)
	add_compile_definitions (_CRT_SECURE_NO_DEPRECATE)
	
	add_compile_definitions (_USE_MATH_DEFINES)

	# Unicode Program
	add_compile_definitions (_UNICODE UNICODE)

	if (MSVC) 
		set (CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
		set (BUILD_SHARED_LIBS TRUE)
	endif()
elseif (UNIX)
	# TODO: Linux Support
endif()

#if (PROJECT_ARCH STREQUAL "x64")
#	link_directories("Public/Win64")
#else()
#	link_directories("Public/Win32")
#endif()

# ==================
# Macros
# ==================

include ("CMakeMacros.cmake")
include ("CMakeFindLibs.cmake")

# ==================
# FranAudio Files
# ==================

include ("FranAudio/Files.cmake")
include ("FranAudioShared/Files.cmake")
if (FRANAUDIO_USE_TEST)
	include ("FranAudioTest/Files.cmake")
endif()
if (FRANAUDIO_USE_SERVER)
	include ("FranAudioServer/Files.cmake")
	include ("FranAudioClient/Files.cmake")
endif()

# ==================
# External Libraries
# ==================
include_directories ("Libs")

# Let's check if we can use Vorbis and Opus
find_vorbis_and_opus()

if (FRANAUDIO_CAN_USE_VORBIS)
	# Ogg
	add_subdirectory ("Libs/ogg")
	set (OGG_INCLUDE_DIR "Libs/ogg/include")
	include_directories ("Libs/ogg/include")
	set (OGG_LIBRARY "Libs/ogg")

	# Vorbis
	add_subdirectory ("Libs/vorbis")
	include_directories ("Libs/vorbis/include")
endif()

if (FRANAUDIO_CAN_USE_OPUS)
	# Opus
	add_subdirectory ("Libs/opus")
	include_directories ("Libs/opus/include")
endif()

# Miniaudio
add_subdirectory ("Libs/miniaudio")

# Libnyquist
add_subdirectory ("Libs/libnyquist")
include_directories ("Libs/libnyquist/include")

# Graphics stuff for FranAudio Test Application
if (FRANAUDIO_USE_TEST)
	find_package(OpenGL REQUIRED)

	if (WIN32)
		if (MSVC) 
			set (GLFW_LIB_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Libs/glfw/lib-vc2019/glfw3.lib") # Targets VC++ 2019 Runtime. Use "glfw3_mt.lib" for Multi-thread support.
		else()
			# TODO: MinGW Support
		endif()
	elseif (UNIX)
		# TODO: Linux Libraries for GLFW/GLAD
	endif()
endif()

# ==================
# Binaries
# ==================

# FranAudio DLL/SO
add_library (FranAudio SHARED ${FRANAUDIO_SOURCEFILES} ${FRANAUDIOSHARED_SOURCEFILES})
target_compile_features(FranAudio PRIVATE cxx_std_23)
target_link_libraries (FranAudio
	miniaudio
	libnyquist
)
target_compile_definitions (FranAudio PRIVATE
	FRANAUDIO_DYNAMIC
	FRANAUDIO_EXPORTS
)
# Export headers
COPY_FILES (FranAudio FRANAUDIO_HEADERFILES ${CMAKE_BINARY_DIR}/include)
COPY_FILES (FranAudio FRANAUDIOSHARED_HEADERFILES ${CMAKE_BINARY_DIR}/include)

if (FRANAUDIO_CAN_USE_VORBIS)
	target_compile_definitions (FranAudio PRIVATE
		FRANAUDIO_USE_VORBIS
	)
	target_link_libraries (FranAudio
		miniaudio_libvorbis
)
endif()

if (FRANAUDIO_CAN_USE_OPUS)
	target_compile_definitions (FranAudio PRIVATE
		FRANAUDIO_USE_OPUS
	)
	target_link_libraries (FranAudio
		miniaudio_libopus
	)
endif()

# FranAudio Server and Client
if (FRANAUDIO_USE_SERVER)
	add_executable (FranAudioServer ${FRANAUDIOSERVER_SOURCEFILES} ${FRANAUDIOSHARED_SOURCEFILES})	
	target_compile_features(FranAudioServer PRIVATE cxx_std_23)
	target_link_libraries (FranAudioServer
		FranAudio
	)

	add_library (FranAudioClient SHARED ${FRANAUDIOCLIENT_SOURCEFILES} ${FRANAUDIOSHARED_SOURCEFILES})
	target_compile_features(FranAudioClient PRIVATE cxx_std_23)
	target_compile_definitions (FranAudioClient PRIVATE
		FRANAUDIO_CLIENT_DYNAMIC
	)

	# Export headers
	COPY_FILES (FranAudioClient FRANAUDIOCLIENT_HEADERFILES ${CMAKE_BINARY_DIR}/include)
endif()

# FranAudio Test Application
if (FRANAUDIO_USE_TEST)
	add_executable (FranAudioTest ${FRANAUDIOTEST_SOURCEFILES} ${FRANAUDIOSHARED_SOURCEFILES})
	target_compile_features(FranAudioTest PRIVATE cxx_std_23)

	target_include_directories (FranAudioTest PRIVATE
		Libs/glad/include
		Libs/glfw/include
		Libs/imgui
		${OPENGL_INCLUDE_DIR}
	)

	target_sources (FranAudioTest PRIVATE
		Libs/imgui/imgui.cpp
		Libs/imgui/imgui_draw.cpp
		Libs/imgui/imgui_tables.cpp
		Libs/imgui/imgui_widgets.cpp
		Libs/imgui/backends/imgui_impl_glfw.cpp
		Libs/imgui/backends/imgui_impl_opengl3.cpp
	)

	target_link_libraries (FranAudioTest
		FranAudio
		${GLFW_LIB_FILE}
		${OPENGL_LIBRARIES}
	)

	if (FRANAUDIO_USE_SERVER)
		target_link_libraries (FranAudioTest
			FranAudioClient
		)
	endif()
endif()
