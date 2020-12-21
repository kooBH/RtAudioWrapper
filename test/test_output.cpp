#include "RtOutput.h"
#include "WAV.h"


// Read output of AudioProbe() and set manually
#define DEVICE_OUTPUT 2

int main() {
  short* buf_1;

  AudioProbe();

  WAV input;
  input.OpenFile("hello_world_ch1.wav");
  int samplerate_in = input.GetSampleRate();
  int samplerate_out = 48000;

  RtOutput speaker(DEVICE_OUTPUT,1,samplerate_in,samplerate_out,128,512);

  input.Rewind();
  buf_1 = new short[input.GetSize()];
  fread(buf_1, sizeof(short), input.GetSize(), input.GetFilePointer());

  speaker.FullBufLoad(buf_1, input.GetSize());
  speaker.Start();
  speaker.Wait();

  return 0;
}

// Modification of AudioProbe()
int GetDefaultOutputDevice() {
  RtAudio audio;
  unsigned int devices = audio.getDeviceCount();
  RtAudio::DeviceInfo info;
  for (unsigned int i = 0; i < devices; i++) {
    info = audio.getDeviceInfo(i);
    if (info.probed == true) {
      //if (info.inputChannels != 0) {
      if (true) {
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
  return 0;
}
