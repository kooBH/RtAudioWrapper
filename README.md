# RtAudioWrapper
Wrapper classes of RtAudio for myself.  

# Include
clone this repository to user project diretory  

include   
```RtBase.h```  
```RtInput.h```  
```RtOutput.h```  
into user project.  
  
include   
```RtAudio.cmake```  
```Libsamplerate.cmake```  
in your ```CMakeLists.txt```.  

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

NOTICE :  
+ [RtAudio](https://github.com/thestk/rtaudio)   
A set of C++ classes that provide a common API for realtime audio input/output across Linux (native ALSA, JACK, PulseAudio and OSS), Macintosh OS X (CoreAudio and JACK), and Windows (DirectSound, ASIO, and WASAPI) operating systems.  

+ [libsamplerate](https://github.com/libsndfile/libsamplerate)    
An audio Sample Rate Conversion library  
