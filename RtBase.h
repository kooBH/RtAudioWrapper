#ifndef _H_RT_BASE_
#define _H_RT_BASE_

#define __INF 1

#include <iostream>
#include "RtAudio/RtAudio.h"
#include <cstdlib>
#include <cstring>
#include <stdio.h>
// Platform-dependent sleep routines.
#if defined( __WINDOWS_ASIO__ ) || defined( __WINDOWS_DS__ ) || defined( __WINDOWS_WASAPI__ )
  #include <windows.h>
  #define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds ) 
#else // Unix variants
  #include <unistd.h>
  #define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif


/*
typedef char MY_TYPE;
#define FORMAT RTAUDIO_SINT8
*/

/* In RtAudio.h
static const RtAudioFormat RTAUDIO_SINT8 = 0x1;    // 8-bit signed integer.
static const RtAudioFormat RTAUDIO_SINT16 = 0x2;   // 16-bit signed integer.
static const RtAudioFormat RTAUDIO_SINT24 = 0x4;   // 24-bit signed integer.
static const RtAudioFormat RTAUDIO_SINT32 = 0x8;   // 32-bit signed integer.
static const RtAudioFormat RTAUDIO_FLOAT32 = 0x10; // Normalized between plus/minus 1.0.
static const RtAudioFormat RTAUDIO_FLOAT64 = 0x20; // Normalized between plus/minus 1.0.

 * */

typedef signed short MY_TYPE;
#define FORMAT RTAUDIO_SINT16
#define SCALE 32767.0
/*
typedef S24 MY_TYPE;
#define FORMAT RTAUDIO_SINT24

typedef signed long MY_TYPE;
#define FORMAT RTAUDIO_SINT32

typedef float MY_TYPE;
#define FORMAT RTAUDIO_FLOAT32

typedef double MY_TYPE;
#define FORMAT RTAUDIO_FLOAT64
*/

inline void AudioProbe();

class RtBase {
  protected:
  /*
   * According to OS, size of input buffer might be
   * different from shift_size;
   * ex) surfacebook -> 288
   * */
  unsigned int input_size;
  unsigned int shift_size;
  unsigned int frame_size;
  unsigned int channels;
  unsigned int device;
  unsigned int sample_rate;
  unsigned int read_offset;
  //adc,dac -> rtaudio
  RtAudio* rtaudio;
  //iParams,oParams -> ioParams
  RtAudio::StreamParameters ioParams;

  public:
  inline RtBase(unsigned int _device, unsigned int _channels,
           unsigned int _sample_rate, unsigned int _shift_size,
           unsigned int _frame_size,unsigned int _input_size=0);
  virtual ~RtBase() { }

  virtual void CleanUp() {}
  inline void Start();
  inline void Stop();
  inline void Wait();
  inline bool IsRunning();

};


RtBase::RtBase(unsigned int _device, unsigned int _channels,
                   unsigned int _sample_rate, unsigned int _shift_size,
                   unsigned int _frame_size,unsigned int _input_size) {

  shift_size = _shift_size;
  if(_input_size == 0)
    input_size = shift_size;
  else 
    input_size = _input_size;
  sample_rate = _sample_rate;
  device = _device;
  channels = _channels;
  frame_size = _frame_size;
  read_offset = 0;

  ioParams.deviceId = device;
  ioParams.nChannels = channels;;
  ioParams.firstChannel = read_offset;
  

  try {
    rtaudio = new RtAudio();
 } catch (RtAudioError &e) {
    std::cout << "ERROR::" << e.getMessage() << '\n' << std::endl;
    CleanUp();
    return;
  }
}

void RtBase::Start() {
  try {
    rtaudio->startStream();
  } catch (RtAudioError &e) {
    std::cout << '\n' << e.getMessage() << '\n' << std::endl;
    CleanUp();
  }

  printf("input_size :%d\n",input_size);
  
}

void RtBase::Stop(){
 try {
    rtaudio->stopStream();
  } catch (RtAudioError &e) {
    std::cout << "\nRtAudioError::" << e.getMessage() << '\n' << std::endl;
    CleanUp();
  }
}

void RtBase::Wait() {

  while (rtaudio->isStreamRunning()) {
    SLEEP(100);
  }
}

bool RtBase::IsRunning() {
  if (rtaudio->isStreamRunning())
    return true;
  else
    return false;
}

void AudioProbe(){
  RtAudio audio;
  unsigned int devices = audio.getDeviceCount();
  RtAudio::DeviceInfo info;
  for (unsigned int i = 0; i < devices; i++) {
    info = audio.getDeviceInfo(i);
    if (info.probed == true) {
        std::cout << "device = " << i << "\n";
        std::cout << "name = " << info.name << "\n";
        std::cout << "maximum input channels = " << info.inputChannels << "\n";
        std::cout << "maximum output channels = " << info.outputChannels << "\n";
        std::cout << "Samplerates : ";
        for (auto sr : info.sampleRates)
          std::cout << sr << " ";
        std::cout << "\n";
        std::cout << "----------------------------------------------------------" << "\n";
    }
  }
}

#endif

