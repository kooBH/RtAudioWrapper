# RtAudioWrapper
Wrapper classes of RtAudio for myself.  

## prerequisite

```
sudo apt-get -y install libasound2-dev
```

### Example for inclusion
+ CMakeLists.txt
```cmake
set(LINKLIBS)
set(COMPILE_OPTION)
set(SRC)
set(INCL)

list(APPEND INCL
lib/RtAudioWrapper
)

include(lib/RtAudioWrapper/RtAudio.cmake)
include(lib/RtAudioWrapper/Libsamplerate.cmake)

add_executable(${PROJECT_NAME} ${SRC})
target_include_directories(${PROJECT_NAME}	PRIVATE	${INCL}  )
target_link_libraries(${PROJECT_NAME} ${LINKLIBS})
target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_OPTION} )
```

### Example for Usage

```cpp
// TODO
```

# TIPS
When the number of channel is large(ex 16), it is good to use large input_size.   
Using large input_size is easily done in linux system.   
But in Windows you need to change driver's buffer size manually, and set input_size param as well.   

# Why do I need to use resampler ? 
Usually input files I use are 16kHz.    
And most of output device don't support 16kHz, mostly 48kHz.  

# NOTICE
+ [RtAudio](https://github.com/thestk/rtaudio)   
A set of C++ classes that provide a common API for realtime audio input/output across Linux (native ALSA, JACK, PulseAudio and OSS), Macintosh OS X (CoreAudio and JACK), and Windows (DirectSound, ASIO, and WASAPI) operating systems.  

+ [libsamplerate](https://github.com/libsndfile/libsamplerate)    
An audio Sample Rate Conversion library  
