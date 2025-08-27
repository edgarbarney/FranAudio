# This file is modified from the original libnyquist CMakeLists.txt
# This will use FranAudio's Ogg, Vorbis and Opus libraries instead of embedded ones if found.

cmake_minimum_required(VERSION 3.13)

set(LIBNYQUIST_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_MODULE_PATH ${LIBNYQUIST_ROOT}/cmake)

include(CXXhelpers)
include(GNUInstallDirs)


if (CMAKE_OSX_ARCHITECTURES)
    if(CMAKE_OSX_SYSROOT MATCHES ".*iphoneos.*")
        # RtAudio is not portable to ios currently
        option(LIBNYQUIST_BUILD_EXAMPLE "Build example application" OFF)
    else()
        option(LIBNYQUIST_BUILD_EXAMPLE "Build example application" ON)
    endif()
else()
    option(LIBNYQUIST_BUILD_EXAMPLE "Build example application" ON)
endif()

#-------------------------------------------------------------------------------

# libwavpack

if (BUILD_LIBWAVPACK)
    project(libwavpack)

    if(MSVC)
        # Disable warning C4996 regarding fopen(), strcpy(), etc.
        _add_define("_CRT_SECURE_NO_WARNINGS")

        # Disable warning C4996 regarding unchecked iterators for std::transform,
        # std::copy, std::equal, et al.
        _add_define("_SCL_SECURE_NO_WARNINGS")

        # Make sure WinDef.h does not define min and max macros which
        # will conflict with std::min() and std::max().
        _add_define("NOMINMAX")
    endif()

    add_definitions(${_NQR_CXX_DEFINITIONS})
    set(CMAKE_CXX_FLAGS "${_NQR_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")

    file(GLOB third_wavpack_src "${LIBNYQUIST_ROOT}/third_party/wavpack/src/*.c")

    add_library(libwavpack STATIC ${third_wavpack_src})

    set_cxx_version(libwavpack)
    _set_compile_options(libwavpack)

    if (WIN32)
        _disable_warning(181)
        _disable_warning(111)
        _disable_warning(4267)
        _disable_warning(4996)
        _disable_warning(4244)
        _disable_warning(4701)
        _disable_warning(4702)
        _disable_warning(4133)
        _disable_warning(4100)
        _disable_warning(4127)
        _disable_warning(4206)
        _disable_warning(4312)
        _disable_warning(4505)
        _disable_warning(4365)
        _disable_warning(4005)
        _disable_warning(4013)
        _disable_warning(4334)
        _disable_warning(4703)
    endif()

    target_include_directories(libwavpack PRIVATE ${LIBNYQUIST_ROOT}/third_party/wavpack/include)

    if (MSVC_IDE)
        set_target_properties(libwavpack PROPERTIES IMPORT_PREFIX "../")
    endif()

    set_target_properties(libwavpack
        PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )

    set_target_properties(libwavpack PROPERTIES OUTPUT_NAME_DEBUG libwavpack_d)

    install(TARGETS libwavpack
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib
            RUNTIME DESTINATION bin)

    install (TARGETS libwavpack DESTINATION lib)
endif()

#-------------------------------------------------------------------------------

# libnyquist static library

project(libnyquist)

file(GLOB nyquist_include "${LIBNYQUIST_ROOT}/include/libnyquist/*")
file(GLOB nyquist_src     "${LIBNYQUIST_ROOT}/src/*")
file(GLOB wavpack_src     "${LIBNYQUIST_ROOT}/third_party/wavpack/src/*.c")

add_library(libnyquist STATIC
    ${nyquist_include}
    ${nyquist_src}
    ${wavpack_src}
)

set_cxx_version(libnyquist)
_set_compile_options(libnyquist)

if (WIN32)
    _disable_warning(4244)
    _disable_warning(4018)
endif()

target_include_directories(libnyquist
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${LIBNYQUIST_ROOT}/include>
  PRIVATE
    ${LIBNYQUIST_ROOT}/include/libnyquist
    ${LIBNYQUIST_ROOT}/third_party
    ${LIBNYQUIST_ROOT}/third_party/FLAC/src/include
	${LIBNYQUIST_ROOT}/third_party/musepack/include
	${LIBNYQUIST_ROOT}/third_party/wavpack/include
	${LIBNYQUIST_ROOT}/src
)
# Use FranAudio's Ogg, Vorbis and Opus libraries if found

if (FRANAUDIO_USE_VORBIS AND FRANAUDIO_USE_OPUS)
	target_include_directories(libnyquist
	PRIVATE
		${FRANAUDIO_LIBRARIES_PARENT_DIR}
		${FRANAUDIO_LIBRARIES_PARENT_DIR}/opus/include
	)

	target_compile_definitions(libnyquist PRIVATE
		FRANAUDIO_USE_VORBIS
		FRANAUDIO_USE_OPUS
	)

	target_link_libraries(libnyquist PRIVATE
		vorbis
		ogg
		opus
		opusfile
	)
else()
	target_include_directories(libnyquist
	PRIVATE
		${LIBNYQUIST_ROOT}/third_party
		${LIBNYQUIST_ROOT}/third_party/vorbis/include
		${LIBNYQUIST_ROOT}/third_party/ogg/include
		${LIBNYQUIST_ROOT}/third_party/opus/include
		${LIBNYQUIST_ROOT}/third_party/opus/celt
		${LIBNYQUIST_ROOT}/third_party/opus/silk
		${LIBNYQUIST_ROOT}/third_party/opus/silk/float
		${LIBNYQUIST_ROOT}/third_party/opusfile/include
	)
endif()

if (MSVC_IDE)
    # hack to get around the "Debug" and "Release" directories cmake tries to add on Windows
    #set_target_properties(libnyquist PROPERTIES PREFIX "../")
    set_target_properties(libnyquist PROPERTIES IMPORT_PREFIX "../")
endif()

set_target_properties(libnyquist PROPERTIES OUTPUT_NAME_DEBUG libnyquist_d)

set_target_properties(libnyquist
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_INSTALL_BINDIR}"
)

#target_link_libraries(libnyquist PRIVATE libwavpack)

add_library(libnyquist::libnyquist ALIAS libnyquist)

message(STATUS "CMAKE_INSTALL_INCLUDEDIR: ${CMAKE_INSTALL_INCLUDEDIR}")

# folders
source_group(src FILES ${nyquist_src})

#-------------------------------------------------------------------------------