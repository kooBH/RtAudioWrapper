#########################################
#                                       #
#         RtAudio      Setting          #
#                                       #
#########################################

# Check for Jack (any OS)

# local variables   


find_library(JACK_LIB jack)
find_package(PkgConfig)
pkg_check_modules(jack jack)
if(JACK_LIB OR jack_FOUND)
  set(HAVE_JACK TRUE)
endif()

# Check for Pulse (any OS)
pkg_check_modules(pulse libpulse-simple)

# Check for known non-Linux unix-likes
if (CMAKE_SYSTEM_NAME MATCHES "kNetBSD.*|NetBSD.*")
  message(STATUS "NetBSD detected, using OSS")
  set(xBSD ON)
elseif(UNIX AND NOT APPLE)
  set(LINUX ON)
endif()

# Necessary for Windows
if(MINGW)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()


# API Options
option(RTAUDIO_API_DS "Build DirectSound API" OFF)
option(RTAUDIO_API_ASIO "Build ASIO API" OFF)
option(RTAUDIO_API_WASAPI "Build WASAPI API" ${WIN32})
option(RTAUDIO_API_OSS "Build OSS4 API" ${xBSD})
option(RTAUDIO_API_ALSA "Build ALSA API" ${LINUX})
option(RTAUDIO_API_PULSE "Build PulseAudio API" ${pulse_FOUND})
option(RTAUDIO_API_JACK "Build JACK audio server API" ${HAVE_JACK})
option(RTAUDIO_API_CORE "Build CoreAudio API" ${APPLE})


# Check for functions
include(CheckFunctionExists)
check_function_exists(gettimeofday HAVE_GETTIMEOFDAY)
if (HAVE_GETTIMEOFDAY)
    add_definitions(-DHAVE_GETTIMEOFDAY)
endif ()


list(APPEND SRC 
lib/RtAudio/RtAudio.cpp
lib/RtAudio/RtAudio.h
	)
	
set(API_LIST)
set(PKGCONFIG_REQUIRES)

# Jack
if (RTAUDIO_API_JACK AND jack_FOUND)
  set(NEED_PTHREAD ON)
  list(APPEND PKGCONFIG_REQUIRES "jack")
  list(APPEND COMPILE_OPTION "-D__UNIX_JACK__")
  list(APPEND API_LIST "jack")
  if(jack_FOUND)
    list(APPEND LINKLIBS ${jack_LIBRARIES})
    list(APPEND INCL ${jack_INCLUDEDIR})
  else()
    list(APPEND LINKLIBS ${JACK_LIB})
  endif()
endif()

# ALSA
if (RTAUDIO_API_ALSA)
  set(NEED_PTHREAD ON)
  find_package(ALSA)
  if (NOT ALSA_FOUND)
    message(FATAL_ERROR "ALSA API requested but no ALSA dev libraries found")
  endif()
  list(APPEND INCL ${ALSA_INCLUDE_DIR})
  list(APPEND LINKLIBS ${ALSA_LIBRARY})
  list(APPEND PKGCONFIG_REQUIRES "alsa")
  list(APPEND COMPILE_OPTION "-D__LINUX_ALSA__")
  list(APPEND API_LIST "alsa")
endif()

# OSS
if (RTAUDIO_OSS)
  set(NEED_PTHREAD ON)
  find_library(OSSAUDIO_LIB ossaudio)
  if (OSSAUDIO_LIB)
    list(APPEND LINKLIBS ossaudio)
    # Note: not an error on some systems
  endif()
  list(APPEND COMPILE_OPTION "-D__LINUX_OSS__")
  list(APPEND API_LIST "oss")
endif()

# Pulse
if (RTAUDIO_API_PULSE)
  set(NEED_PTHREAD ON)
  find_library(PULSE_LIB pulse)
  find_library(PULSESIMPLE_LIB pulse-simple)
  list(APPEND LINKLIBS ${PULSE_LIB} ${PULSESIMPLE_LIB})
  list(APPEND PKGCONFIG_REQUIRES "libpulse-simple")
  list(APPEND COMPILE_OPTION "-D__LINUX_PULSE__")
  list(APPEND API_LIST "pulse")
endif()

# CoreAudio
if (RTAUDIO_API_CORE)
  find_library(COREAUDIO_LIB CoreAudio)
  find_library(COREFOUNDATION_LIB CoreFoundation)
  list(APPEND LINKLIBS ${COREAUDIO_LIB} ${COREFOUNDATION_LIB})
  list(APPEND COMPILE_OPTION "-D__MACOSX_CORE__")
  list(APPEND API_LIST "core")
endif()

# ASIO
if (RTAUDIO_API_ASIO)
  set(NEED_WIN32LIBS ON)
    list(APPEND SRC
  lib/RtAudio/asio.cpp
  lib/RtAudio/asiodrivers.cpp
  lib/RtAudio/asiolist.cpp
  lib/RtAudio/iasiothiscallresolver.cpp
  )
    list(APPEND COMPILE_OPTION "-D__WINDOWS_ASIO__")
  list(APPEND API_LIST "asio")
endif()

# DSound
if (RTAUDIO_API_DS)
  set(NEED_WIN32LIBS ON)
  list(APPEND LINKLIBS dsound)
  list(APPEND COMPILE_OPTION "-D__WINDOWS_DS__")
  list(APPEND API_LIST "ds")
endif()

# WASAPI
if (RTAUDIO_API_WASAPI)
  set(NEED_WIN32LIBS ON)
  list(APPEND LINKLIBS ksuser mfplat mfuuid wmcodecdspuuid)
  list(APPEND COMPILE_OPTION "-D__WINDOWS_WASAPI__")
  list(APPEND API_LIST "wasapi")
endif()

# Windows libs
if (NEED_WIN32LIBS)
  list(APPEND LINKLIBS winmm ole32)
endif()

# pthread
if (NEED_PTHREAD)
  find_package(Threads REQUIRED
    CMAKE_THREAD_PREFER_PTHREAD
    THREADS_PREFER_PTHREAD_FLAG)
  list(APPEND LINKLIBS Threads::Threads)
endif()



################################################

message(STATUS "Compiling with support for: ${API_LIST}")
