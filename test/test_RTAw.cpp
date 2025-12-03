#include "RTAw.h"
#include "WAV.h"
#include <stdio.h>

#if _WIN32
#include <Windows.h>
#endif

int main() {
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
  printf("-----AudioProbe ----\n");
  RTAw_audioprobe();

  printf("---------\n");

  int size = RTAw_count_devices();
  printf("Device count : %d\n", size);
  char name[256];
  for (int i = 0; i < size; i++) {
    RTAw_get_device_name(i, name);
    printf("Device %d : %s\n", i, name);
  }



  void* RTAObj = RTAw_input_construct(device, channels, samplerate, buf_size);
  RTAw_input_start(RTAObj);

  printf("Input size : %d -> %d\n", buf_size, RTAw_input_buf_size(RTAObj));

  while(cnt < 1){
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

  return 0;
}