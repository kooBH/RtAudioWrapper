#include "RtInput.h"
#include "WAV.h"
#include <iostream>


int main() {
  // Allow Korean on terminal
#if _WIN32
  SetConsoleOutputCP(CP_UTF8);
#endif

  const int n_channels = 1;
  const int sample_rate = 16000;
  const int input_unit = 256;

  // Audio Probe
  RtInput::AudioProbe();

  int device = 0;
  std::cout << "Select device index : ";
  std::cin >> device;


  // Init RtInput & WAV
  WAV input_dump(n_channels,sample_rate);
  RtInput mic_in(device,n_channels, sample_rate, input_unit);

  if (!mic_in.IsOpen()) {
    std::cout << "ERROR::Cannot open input stream with device : "<< device << std::endl;
    return -1;
  }

  input_dump.NewFile("test.wav");


  // Record for 5 seconds
  int n_recorded = 0;
  int n_to_record = sample_rate * 5;
  short* buffer = new short[input_unit * n_channels];
  mic_in.Start();
  while (mic_in.IsRunning()) {
    if (mic_in.BufferAvailable() > input_unit) {
      mic_in.GetBuffer(buffer);
      n_recorded += input_unit;
      input_dump.Append(buffer, input_unit);
      if (n_recorded >= n_to_record) {
        break;
      }
    }
    else {
      SLEEP(10);
    }
  }

  // Finish 
  mic_in.Stop();
  input_dump.Finish();
  delete[] buffer;


  return 0;
}