# FranticDreamer 2022-2025
# CMakeLists file for CMake Configuration

# At least 3.20 Needed for C++23
cmake_minimum_required(VERSION 3.23)

project("FranAudio"
    VERSION 0.1.0
    DESCRIPTION "FranAudio - Audio Library, Mainly for Games and Game Engines"
    LANGUAGES CXX C
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =================
# Options
# =================
option(FRANAUDIO_USE_TEST "Build a test application for the library" ON)
option(FRANAUDIO_USE_SERVER "Build FranAudio as a server + client" OFF)
option(FRANAUDIO_SERVERCLIENT_DEBUG "Enable extended debug messages for server and client" OFF)

# =================
# Architecture
# =================
if (CMAKE_SIZEOF_VOID_P EQUAL 8) # 64 Bit
	set (PROJECT_ARCH "x64")
elseif (CMAKE_SIZEOF_VOID_P EQUAL 4) # 32 Bit
	set (PROJECT_ARCH "x86")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# =================
# Windows Specifics
# =================
if (WIN32)
    add_compile_definitions(
        _CRT_SECURE_NO_DEPRECATE
        _USE_MATH_DEFINES
        NOMINMAX
        _UNICODE
        UNICODE
    )
    if (MSVC)
        set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
        set(BUILD_SHARED_LIBS TRUE)
    endif()
elseif (UNIX)
	# TODO: Linux Support
endif()

# =================
# Macros
# =================
include("CMakeMacros.cmake")
include("CMakeFindLibs.cmake")

# =================
# FranAudio Sources
# =================
include("FranAudio/Files.cmake")
include("FranAudioShared/Files.cmake")
if (FRANAUDIO_USE_TEST)
    include("FranAudioTest/Files.cmake")
endif()
if (FRANAUDIO_USE_SERVER)
    include("FranAudioServer/Files.cmake")
    include("FranAudioClient/Files.cmake")
endif()

# =================
# External Libraries
# =================
include_directories ("Libs")
set(FRANAUDIO_LIBRARIES_PARENT_DIR "Libs")

# Ogg / Vorbis / Opus
find_vorbis_and_opus()

if (FRANAUDIO_CAN_USE_VORBIS)
	# Ogg
    add_subdirectory("Libs/ogg")
	include_directories ("Libs/ogg/include")
	set (OGG_INCLUDE_DIR "Libs/ogg/include")
	set (OGG_LIBRARY "Libs/ogg")
	set (Ogg_DIR "Libs/ogg")

    # Vorbis
	add_subdirectory ("Libs/vorbis")
	include_directories ("Libs/vorbis/include")
	set (VORBIS_LIBRARY "Libs/ogg")
	set (Vorbis_DIR "Libs/ogg")
endif()

if (FRANAUDIO_CAN_USE_OPUS)
    set(OP_DISABLE_HTTP ON CACHE BOOL "Disable HTTP/HTTPS support" FORCE)
    
	# Opus
	add_subdirectory ("Libs/opus")
	include_directories ("Libs/opus/include")
	set (OPUS_LIBRARY "Libs/opus")
	set (Opus_DIR "Libs/opus")

	# Opusfile
	add_subdirectory ("Libs/opusfile")
	include_directories ("Libs/opusfile/include")
	set (OPUSFILE_LIBRARY "Libs/opusfile")
	set (OpusFile_DIR "Libs/opusfile")
endif()

# Miniaudio
add_subdirectory("Libs/miniaudio")

# Libnyquist
add_subdirectory("Libs/libnyquist")

# =================
# FranAudio Library
# =================
add_library(FranAudio SHARED
    ${FRANAUDIO_SOURCEFILES}
    ${FRANAUDIOSHARED_SOURCEFILES}
)

target_compile_definitions(FranAudio PRIVATE
    FRANAUDIO_DYNAMIC
    FRANAUDIO_EXPORTS
    $<$<BOOL:${FRANAUDIO_CAN_USE_VORBIS}>:FRANAUDIO_USE_VORBIS>
    $<$<BOOL:${FRANAUDIO_CAN_USE_OPUS}>:FRANAUDIO_USE_OPUS>
)

target_link_libraries(FranAudio
    PRIVATE miniaudio libnyquist
    PRIVATE $<$<BOOL:${FRANAUDIO_CAN_USE_VORBIS}>:miniaudio_libvorbis>
    PRIVATE $<$<BOOL:${FRANAUDIO_CAN_USE_OPUS}>:miniaudio_libopus>
)

# Export headers (CMake ≥ 3.23 style)
target_sources(FranAudio
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        FILES
            ${FRANAUDIO_HEADERFILES}
            ${FRANAUDIOSHARED_HEADERFILES}
)

# =================
# FranAudio Server & Client
# =================
if (FRANAUDIO_USE_SERVER)
    add_executable(FranAudioServer
        ${FRANAUDIOSERVER_SOURCEFILES}
        ${FRANAUDIOSHARED_SOURCEFILES}
    )
    target_link_libraries(FranAudioServer PRIVATE FranAudio)

    add_library(FranAudioClient SHARED
        ${FRANAUDIOCLIENT_SOURCEFILES}
        ${FRANAUDIOSHARED_SOURCEFILES}
    )
    target_compile_definitions(FranAudioClient PRIVATE FRANAUDIO_CLIENT_DYNAMIC)

    target_sources(FranAudioClient
        PUBLIC
            FILE_SET HEADERS
            BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
            FILES ${FRANAUDIOCLIENT_HEADERFILES}
    )
endif()

# =================
# FranAudio Test Application
# =================
if (FRANAUDIO_USE_TEST)
    find_package(OpenGL REQUIRED)

    add_executable(FranAudioTest
        ${FRANAUDIOTEST_SOURCEFILES}
        ${FRANAUDIOSHARED_SOURCEFILES}

        # ImGui sources
        Libs/imgui/imgui.cpp
        Libs/imgui/imgui_draw.cpp
        Libs/imgui/imgui_tables.cpp
        Libs/imgui/imgui_widgets.cpp
        Libs/imgui/backends/imgui_impl_glfw.cpp
        Libs/imgui/backends/imgui_impl_opengl3.cpp
    )

    set_target_properties(FranAudioTest PROPERTIES
        WIN32_EXECUTABLE TRUE # replaces manual /SUBSYSTEM:WINDOWS or -mwindows
    )

    target_include_directories(FranAudioTest PRIVATE
        Libs/glad/include
        Libs/glfw/include
        Libs/imgui
        ${OPENGL_INCLUDE_DIR}
    )

    if (WIN32)
		if (MSVC)
			target_link_libraries(FranAudioTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Libs/glfw/lib-vc2019/glfw3.lib)
		endif()
	else()
		# Other OS
    endif()

    target_link_libraries(FranAudioTest
        PRIVATE FranAudio
        PRIVATE ${OPENGL_LIBRARIES}
    )

    if (FRANAUDIO_USE_SERVER)
        target_link_libraries(FranAudioTest PRIVATE FranAudioClient)
    endif()
endif()

# =================
# Install & Export (for consumers)
# =================
include(GNUInstallDirs)

# Avoid installing empty directories
set(CMAKE_INSTALL_DATAROOTDIR "")
set(CMAKE_INSTALL_DATADIR "")

install(TARGETS FranAudio
    EXPORT FranAudioTargets
    FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(EXPORT FranAudioTargets
    FILE FranAudioConfig.cmake
    NAMESPACE FranAudio::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/FranAudio
)