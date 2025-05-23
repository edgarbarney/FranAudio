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

# libopus

if (BUILD_LIBOPUS)
    project(libopus)

    file(GLOB third_opus_src
        "${LIBNYQUIST_ROOT}/third_party/opus/celt/*.c"
        "${LIBNYQUIST_ROOT}/third_party/opus/opusfile/src/*.c"
        "${LIBNYQUIST_ROOT}/third_party/opus/silk/*.c"
        "${LIBNYQUIST_ROOT}/third_party/opus/silk/float/*.c"
    )
    set(lib_opus_src
        "${LIBNYQUIST_ROOT}/third_party/opus/libopus/src/analysis.c"
        "${LIBNYQUIST_ROOT}/third_party/opus/libopus/src/mlp_data.c"
        "${LIBNYQUIST_ROOT}/third_party/opus/libopus/src/mlp.c"
        "${LIBNYQUIST_ROOT}/third_party/opus/libopus/src/opus_decoder.c"
        "${LIBNYQUIST_ROOT}/third_party/opus/libopus/src/opus_multistream_decoder.c"
        "${LIBNYQUIST_ROOT}/third_party/opus/libopus/src/opus_multistream_encoder.c"
        "${LIBNYQUIST_ROOT}/third_party/opus/libopus/src/opus.c"
        "${LIBNYQUIST_ROOT}/third_party/opus/libopus/src/repacketizer.c"
    )

    add_library(libopus STATIC ${third_opus_src} ${lib_opus_src})

    set_cxx_version(libopus)
    _set_compile_options(libopus)

    if (WIN32)
        _disable_warning(4244)
        _disable_warning(4018)
    endif()

    target_include_directories(libopus PRIVATE
        ${LIBNYQUIST_ROOT}/third_party/libogg/include
        ${LIBNYQUIST_ROOT}/third_party/opus/celt
        ${LIBNYQUIST_ROOT}/third_party/opus/libopus/include
        ${LIBNYQUIST_ROOT}/third_party/opus/opusfile/include
        ${LIBNYQUIST_ROOT}/third_party/opus/opusfile/src/include
        ${LIBNYQUIST_ROOT}/third_party/opus/silk
        ${LIBNYQUIST_ROOT}/third_party/opus/silk/float)

    if (MSVC_IDE)
        # hack to get around the "Debug" and "Release" directories cmake tries to add on Windows
        #set_target_properties(libnyquist PROPERTIES PREFIX "../")
        set_target_properties(libopus PROPERTIES IMPORT_PREFIX "../")
    endif()

    target_compile_definitions(libopus PRIVATE OPUS_BUILD)
    target_compile_definitions(libopus PRIVATE USE_ALLOCA)

    set_target_properties(libopus
        PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )

    set_target_properties(libopus PROPERTIES OUTPUT_NAME_DEBUG libopus_d)

    install (TARGETS libopus
             LIBRARY DESTINATION lib
             ARCHIVE DESTINATION lib
             RUNTIME DESTINATION bin)

    install (TARGETS libopus DESTINATION lib)

    # folders

    source_group(src FILES ${third_opus_src})
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
    ${LIBNYQUIST_ROOT}/third_party/libogg/include
    ${LIBNYQUIST_ROOT}/third_party/libvorbis/include
    ${LIBNYQUIST_ROOT}/third_party/libvorbis/src
    ${LIBNYQUIST_ROOT}/third_party/musepack/include
    ${LIBNYQUIST_ROOT}/third_party/opus/celt
    ${LIBNYQUIST_ROOT}/third_party/opus/libopus/include
    ${LIBNYQUIST_ROOT}/third_party/opus/opusfile/include
    ${LIBNYQUIST_ROOT}/third_party/opus/opusfile/src/include
    ${LIBNYQUIST_ROOT}/third_party/opus/silk
    ${LIBNYQUIST_ROOT}/third_party/opus/silk/float
    ${LIBNYQUIST_ROOT}/third_party/wavpack/include
    ${LIBNYQUIST_ROOT}/src
)

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

# install the libnyquist binaries
install(TARGETS libnyquist
        EXPORT libnyquist
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

# install the cmake configuration files
install(EXPORT libnyquist
    FILE libnyquistConfig.cmake
    NAMESPACE libnyquist::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/libnyquist"
)

message(STATUS "CMAKE_INSTALL_INCLUDEDIR: ${CMAKE_INSTALL_INCLUDEDIR}")

# todo, install the headers

# export the targets for use when libnyquist is included as a subproject
export(TARGETS libnyquist
       NAMESPACE libnyquist::
       FILE "${CMAKE_CURRENT_BINARY_DIR}/libnyquistConfig.cmake"
)

# folders
source_group(src FILES ${nyquist_src})

#-------------------------------------------------------------------------------