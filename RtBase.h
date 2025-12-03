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
           unsigned int _frame_size=-1,unsigned int _input_size=-1);
  virtual ~RtBase() { }

  virtual void CleanUp() {}
  inline int Start();
  inline void Stop();
  inline void Wait();
  inline bool IsRunning();

  inline std::string GetDeviceName(int idx);

  int GetInputSize() { return input_size; }

};


RtBase::RtBase(unsigned int _device, unsigned int _channels,
                   unsigned int _sample_rate, unsigned int _shift_size,
                   unsigned int _frame_size,unsigned int _input_size) {

  shift_size = _shift_size;
  if(_input_size == 0)
    input_size = shift_size;
  else 
    input_size = _input_size;

  if(_frame_size == 0)
    frame_size = int(shift_size/4);
  else
    frame_size = _frame_size;

  sample_rate = _sample_rate;
  device = _device;
  channels = _channels;
  frame_size = _frame_size;
  read_offset = 0;

  ioParams.deviceId = device;
  ioParams.nChannels = channels;;
  ioParams.firstChannel = read_offset;
  

  rtaudio = new RtAudio();
}

int RtBase::Start() {
  if(rtaudio->startStream()){
      std::cout << "ERROR::Cannot start stream\n" << std::endl;
      CleanUp();
      return -1;
    }
    return 0;
}

void RtBase::Stop(){
    if(rtaudio->stopStream()){
      std::cout << "ERROR::Cannot stop stream\n" << std::endl;
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
  RtAudio::DeviceInfo info;

  std::cout << "\nAPI: " << RtAudio::getApiDisplayName(audio.getCurrentApi()) << std::endl;

  std::vector<unsigned int> devices = audio.getDeviceIds();
  std::cout << "\nFound " << devices.size() << " device(s) ...\n";

  for (unsigned int i=0; i<devices.size(); i++) {
    info = audio.getDeviceInfo( devices[i] );

    std::cout << "\nDevice Name = " << info.name << '\n';
    std::cout << "Device Index = " << i << '\n';
    std::cout << "Output Channels = " << info.outputChannels << '\n';
    std::cout << "Input Channels = " << info.inputChannels << '\n';
    std::cout << "Duplex Channels = " << info.duplexChannels << '\n';
    if ( info.isDefaultOutput ) std::cout << "This is the default output device.\n";
    else std::cout << "This is NOT the default output device.\n";
    if ( info.isDefaultInput ) std::cout << "This is the default input device.\n";
    else std::cout << "This is NOT the default input device.\n";
    if ( info.nativeFormats == 0 )
      std::cout << "No natively supported data formats(?)!";
    else {
      std::cout << "Natively supported data formats:\n";
      if ( info.nativeFormats & RTAUDIO_SINT8 )
        std::cout << "  8-bit int\n";
      if ( info.nativeFormats & RTAUDIO_SINT16 )
        std::cout << "  16-bit int\n";
      if ( info.nativeFormats & RTAUDIO_SINT24 )
        std::cout << "  24-bit int\n";
      if ( info.nativeFormats & RTAUDIO_SINT32 )
        std::cout << "  32-bit int\n";
      if ( info.nativeFormats & RTAUDIO_FLOAT32 )
        std::cout << "  32-bit float\n";
      if ( info.nativeFormats & RTAUDIO_FLOAT64 )
        std::cout << "  64-bit float\n";
    }
    if ( info.sampleRates.size() < 1 )
      std::cout << "No supported sample rates found!";
    else {
      std::cout << "Supported sample rates = ";
      for (unsigned int j=0; j<info.sampleRates.size(); j++)
        std::cout << info.sampleRates[j] << " ";
    }
    std::cout << std::endl;
    if ( info.preferredSampleRate == 0 )
      std::cout << "No preferred sample rate found!" << std::endl;
    else
      std::cout << "Preferred sample rate = " << info.preferredSampleRate << std::endl;
  }
}

std::string RtBase::GetDeviceName(int idx){
  RtAudio audio;
  unsigned int devices = audio.getDeviceCount();
  RtAudio::DeviceInfo info;
  if(idx >= devices){
    std::cout << "ERROR::RtBase::GetDeviceName::Invalid device index\n";
    return "";
  }
  info = audio.getDeviceInfo(idx);
  return info.name;
}

#endif



