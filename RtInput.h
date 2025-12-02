#ifndef _H_RT_INPUT_
#define _H_RT_INPUT_

#include "RtBase.h"
#include <atomic>


struct InputData {
  short *buffer;
  unsigned int bufferBytes;
  unsigned int totalFrames;
  /* local counter : Reset when rewinds */
  unsigned int frameCounter;
  /* global counter */
  unsigned int totalFrameCounter;
  unsigned int channels;
  unsigned int max_stock;
  std::atomic<int> stock;
  bool record_inf;
};

class RtInput:public RtBase {
protected:
  const unsigned int max_stock_second = 20;
 
  unsigned long buffer_bytes;
  unsigned long frame_counter;
  unsigned int max_stock;
  
  double record_time;
  

public:

inline RtInput(unsigned int _device, unsigned int _channels,
                   unsigned int _sample_rate, unsigned int _shift_size,
                   unsigned int _frame_size=0,unsigned int _input_size=0);
inline ~RtInput() { CleanUp(); }

  inline void CleanUp();

  /* For just recording */
  inline void GetBuffer(short *);

  /* For Preprocessing */
  inline virtual int Convert2ShiftedArray(double **arr);
  inline int Convert2Array(double **arr);
  
  inline void SetRecordingTime(double time);
  inline void SetRecordingInf();
  
  // Enable RtInput to record agian.
  inline void Clear();

//https://github.com/kooBH/IIP_Demo/issues/212
  inline int Convert2ShiftedArrayInverted(double **arr);

  InputData data;
};

inline int rt_call_back(void * /*outputBuffer*/, void *inputBuffer,
                 unsigned int nBufferFrames, double /*streamTime*/,
                 RtAudioStreamStatus /*status*/, void *data);

RtInput::RtInput(unsigned int _device, unsigned int _channels,
                   unsigned int _sample_rate, unsigned int _shift_size,
                   unsigned int _frame_size,unsigned int _input_size):
          RtBase(_device, _channels,_sample_rate,_shift_size,_frame_size,_input_size),
		  record_time(5.0)
{
  max_stock = sample_rate * max_stock_second;
  data.max_stock = max_stock;
#ifndef NDEBUG
  std::cout << "INFO::Init RtInput\n";
  std::cout << " device " << device << "\n";
  std::cout << " sample rate " << sample_rate << "\n";
  std::cout << " channels " << channels << "\n";
  std::cout << " frame_size " << frame_size << "\n";
  std::cout << " shift_size " << shift_size << "\n";
  std::cout << " input_size " << input_size << "\n";
#endif

  /* Unused
    if ( device == 0 )
      ioParams.deviceId = rtaudio->getDefaultInputDevice();
    else
      ioParams.deviceId = device;
  */

  /*** BLOCK 3 ***/
  data.buffer = nullptr;
  data.record_inf=true;
  Clear();
  //input_size = 512;
  RtAudio::StreamOptions options;
  /*
	RTAUDIO_NONINTERLEAVED: Use non-interleaved buffers (default = interleaved).
	RTAUDIO_MINIMIZE_LATENCY: Attempt to set stream parameters for lowest possible latency.
	RTAUDIO_HOG_DEVICE: Attempt grab device for exclusive use.
	RTAUDIO_SCHEDULE_REALTIME: Attempt to select realtime scheduling for callback thread.
	RTAUDIO_ALSA_USE_DEFAULT: Use the "default" PCM device (ALSA only).
  */
  // options.flags = RTAUDIO_NONINTERLEAVED;

  //options.flags |= RTAUDIO_SCHEDULE_REALTIME;
  //options.flags |= RTAUDIO_MINIMIZE_LATENCY;
  try {
	rtaudio->openStream(NULL, &ioParams, FORMAT, sample_rate, &input_size,
		&rt_call_back, (void *)&data);
  } catch (RtAudioError &e) {
    std::cout << "ERROR::" << e.getMessage() << '\n' << std::endl;
    CleanUp();
    throw std::runtime_error(e.getMessage());
    return;
  }

#ifdef DEBUG
  printf("input_size : %d\n", input_size);
#endif
  // input_size might be changed;
  data.bufferBytes = input_size * channels * sizeof(short);

  data.totalFrames = max_stock;

  data.channels = channels;
  unsigned long totalBytes;
  unsigned long paddingBytes 
    = static_cast<unsigned long>((0.1) * max_stock * channels * sizeof(short));

  totalBytes = max_stock * channels * sizeof(short);

  // Allocate the entire data buffer before starting stream.
  data.buffer = (short *)malloc(totalBytes + paddingBytes);
  if (data.buffer == 0) {
    std::cout << "Memory allocation error ... quitting!\n";
    CleanUp();
  }
} 

int rt_call_back(void* /*outputBuffer*/, void* inputBuffer,
	unsigned int nBufferFrames, double /*streamTime*/,
	RtAudioStreamStatus status, void* data) {
	InputData* iData = (InputData*)data;

	if (status)std::cout << "Stream overflow detected!" << std::endl;

	unsigned int frames = nBufferFrames;
	iData->bufferBytes = frames * iData->channels * sizeof(signed short);
   
	unsigned long offset = iData->frameCounter * iData->channels;

	memcpy(iData->buffer + offset, inputBuffer, iData->bufferBytes);

	iData->frameCounter += frames;
	iData->totalFrameCounter += frames;
	iData->stock.fetch_add(frames);

	/* Rewind local frame counter*/
	if (iData->frameCounter >= iData->max_stock)
		iData->frameCounter = 0;

	if (!iData->record_inf){
		if (iData->totalFrameCounter >= iData->totalFrames)
			return 2;
  }
  return 0;
}                

void RtInput::GetBuffer(short *raw) {
	  memcpy(raw, data.buffer + read_offset, shift_size * channels * sizeof(short));
	  read_offset += channels * shift_size;

    //rewind read_offset
    if(read_offset >= data.max_stock* data.channels)
          read_offset = 0;

  // data.stock-=shift_size;
  data.stock.fetch_sub(shift_size);

  if (data.stock.load() > static_cast<int>(max_stock)) {
    printf("*********************************************************\n");
    printf("** FAIL:This program is too slow! Fail to catch up![1] **\n");
    printf("*********************************************************\n");
    exit(-1); 
  } 
}

// arr : double[channels][frame_size]
int RtInput::Convert2ShiftedArray(double **arr) {

#ifndef NDEBUG
// printf("Convert2ShiftedArray::read_offset:%d|stock:%d\n",read_offset,data.stock);
#endif

  // SHIFT
  for (int j = 0; j < channels; j++) {
    for (int i = 0; i < (frame_size - shift_size); i++) {
      arr[j][i] = arr[j][i + shift_size];
    }
  }
  // COPY as doulbe
  //  memcpy(arr,data.buffer+read_offset,shift_size);
  for (int i = 0; i < shift_size; i++) {
    for (int j = 0; j < channels; j++)
      arr[j][i + (frame_size - shift_size)] =
          (double)(data.buffer[i * channels + j + read_offset]);
  }
  read_offset += channels * shift_size;
  // Back to first index
  if (read_offset >= data.totalFrames * channels) {
#ifndef NDEBUG
//    std::cout<<"Convert2Array::Back to first offset\n";
#endif
    read_offset = 0;
  }
  data.stock.fetch_sub(shift_size);
  // FAIL - Real Time
  if (data.stock.load() > static_cast<int>(max_stock)) {
    printf("*******************************************************\n");
    printf("** FAIL:Process is too slow! Failed to catch up!     **\n");
    printf("*******************************************************\n");
    return -1;
  }
  return 0;

}

// arr : double[channels][shift_size]
int RtInput::Convert2Array(double **arr) {
#ifndef NDEBUG
// printf("Convert2Array::read_offset:%d|stock:%d\n",read_offset,data.stock.load());
#endif

  // COPY as doulbe
  //  memcpy(arr,data.buffer+read_offset,shift_size);
  for (int i = 0; i < shift_size; i++) {
    for (int j = 0; j < channels; j++)
      arr[j][i] = (double)(data.buffer[i * channels + j + read_offset]);
  }
  read_offset += channels * shift_size;
  // Back to first index
  if (read_offset >= data.totalFrames * channels) {
#ifndef NDEBUG
  //  std::cout << "Convert2Array::Back to first offset\n";
#endif
    read_offset = 0;
  }
  data.stock.fetch_sub(shift_size);
  // FAIL - Real Time
  if (data.stock.load() > static_cast<int>(max_stock)) {
    printf("*******************************************************\n");
    printf("** FAIL:Process is too slow! Failed to catch up!     **\n");
    printf("*******************************************************\n");
    return -1;
  }
  return 0;
}

void RtInput::CleanUp() {
  if (rtaudio->isStreamOpen())
    rtaudio->closeStream();
  if (data.buffer)
    free(data.buffer);
  if (rtaudio)
    delete rtaudio;
}

int RtInput::Convert2ShiftedArrayInverted(double **arr){
#ifndef NDEBUG
// printf("Convert2ShiftedArray::read_offset:%d|stock:%d\n",read_offset,data.stock);
#endif
  double * t1;
  /* Swap Pairs */
  for(int i =0; i<channels/2;i++){
   t1 = arr[2*i];
   arr[2*i] = arr[2*i+1];
   arr[2*i+1] = t1;
  }


  // SHIFT
  for (int j = 0; j < channels; j++) {
    for (int i = 0; i < (frame_size - shift_size); i++) {
      arr[j][i] = arr[j][i + shift_size];
    }
  }
  // COPY as doulbe
  // AND invert pairs.
  //  memcpy(arr,data.buffer+read_offset,shift_size);
  for (int i = 0; i < shift_size; i++) {
    for (int j = 0; j < channels; j++)

      arr[j][i + (frame_size - shift_size)] =
          (double)(data.buffer[i * channels + j + read_offset]);
  }
  read_offset += channels * shift_size;
  // Back to first index
  if (read_offset >= data.totalFrames * channels) {
#ifndef NDEBUG
//    std::cout<<"Convert2Array::Back to first offset\n";
#endif
    read_offset = 0;
  }
  data.stock.fetch_sub(shift_size);
  /* Recover Pairs */
  for(int i =0; i<channels/2;i++){
   t1 = arr[2*i];
   arr[2*i] = arr[2*i+1];
   arr[2*i+1] = t1;
  }
  // FAIL - Real Time
  if (data.stock > max_stock) {
    printf("*******************************************************\n");
    printf("** FAIL:This program is too slow! Fail to catch up!  **\n");
    printf("*******************************************************\n");
    return -1;
  }
  return 0;
}

void RtInput::SetRecordingTime(double time) {
	record_time = time;
	data.record_inf = false;
	data.totalFrames = static_cast<int>(sample_rate * record_time);
}
void RtInput::SetRecordingInf(){
	data.record_inf = true;
}

void RtInput :: Clear() {
	data.stock.store(0);
	read_offset = 0;
	data.frameCounter = 0;
	data.totalFrameCounter = 0;
}



#endif
