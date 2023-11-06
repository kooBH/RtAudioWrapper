#include "RtOutput.h"
#include "WAV.h"


// Read output of AudioProbe() and set manually
#define DEVICE_OUTPUT 4

int main() {
  short* buf_1;

  AudioProbe();

  WAV input;
  input.OpenFile("input.wav");
  int samplerate_in = input.GetSampleRate();
  //int samplerate_out = 48000;
  int samplerate_out = 16000;
  short * buf = new short[512];

  RtOutput speaker(DEVICE_OUTPUT,1,samplerate_in,samplerate_out,512,512);

  /*
  input.Rewind();
  buf_1 = new short[input.GetSize()];
  fread(buf_1, sizeof(short), input.GetSize(), input.GetFilePointer());
  speaker.FullBufLoad(buf_1, input.GetSize());
  */

  speaker.PrepStream();
  speaker.Start();
  while (!input.IsEOF()) {
    input.ReadUnit(buf, 512);
    speaker.AppendQueue(buf);

    int size_queue = speaker.data.queue.size();
    if (size_queue > 10) {
      printf("tic %d\n",size_queue);
      SLEEP(30);
    }

  }
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
