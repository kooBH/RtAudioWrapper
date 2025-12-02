#include "RTAw.h"
#include "WAV.h"
#include <stdio.h>

#if _WIN32
#include <Windows.h>
#endif

void main() {
#if _WIN32
  SetConsoleOutputCP(CP_UTF8);
#endif

  int device = 10;
  int channels = 16;
  int samplerate = 48000;
  int buf_size = 128;

  int cnt = 0;
  WAV wav_in(channels, samplerate);

  short* buffer = new short[buf_size * channels];


  wav_in.NewFile("output.wav");
  RTAw_audioprobe();



  void* RTAObj = RTAw_input_construct(device, channels, samplerate, buf_size);
  RTAw_input_start(RTAObj);

  printf("Input size : %d -> %d\n", buf_size, RTAw_input_buf_size(RTAObj));

  while(cnt < 1028){
    if (RTAw_input_buffer_length(RTAObj) > buf_size*channels) {
      RTAw_input_buffer_read(RTAObj, buffer);
      wav_in.Append(buffer, buf_size*channels);
      cnt++;
    }
  }

  wav_in.Finish();
  RTAw_input_stop(RTAObj);
  RTAw_input_release(RTAObj);

  delete[] buffer;


}