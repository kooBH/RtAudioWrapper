#ifndef _H_SOUND_PLAY__
#define _H_SOUND_PLAY__

#include "RtBase.h"
#include "libsamplerate/samplerate.h"
#include <atomic>

struct OutputData {
  //FILE *fd;
  unsigned int channels;
  long size;
  unsigned int sample_rate;
  unsigned int frameCounter; // 재생 카운터
  unsigned int appendCounter;// append 카운터
  unsigned int totalFrames;
  float *buf;  //리샘플링 때문에 항상 buffer 는 float.
  unsigned int size_unit;
  std::atomic<int> stock;
};

class RtOutput:public RtBase {
private:
	SRC_DATA src_data;
	RtAudio::StreamOptions options;

  // shift frame
  //unsigned long buffer_bytes;
  //unsigned long frame_counter;
  unsigned int bufferFrames;
  unsigned int sample_rate_input;
  unsigned long format;

  float* ring_output;
  float* float_full_buffer;
  float* resampled_output_buffer;

  /**/
  int len_max_out_buf = 0; //max output buffer length
  int len_max_src_buf = 0; //max resampling buffer length

  inline void InitResampler(int len);
  inline void Resample(float* src_in);
  inline void OpenOutputStream();

public:

  OutputData data;

  inline RtOutput(unsigned int _device,
            unsigned int _channels,
            unsigned int _sample_rate_input,
            unsigned int _sample_rate_output, 
					  unsigned int _shift_size,
            unsigned int _frame_size,
// static const RtAudioFormat RTAUDIO_SINT16 = 0x2;   // 16-bit signed integer.
// Default float : 0x10
            unsigned long _format = 0x10);
  inline ~RtOutput();

  /* 재생할 버퍼를 쌓으면서 재생*/
  // WIP
  inline void PrepStream(int size_buf);
  inline void BufAppend(float *input,int len);
  inline void BufAppend(short *input);

  /* 재생할 내용을 한번에 올려서 재생 */
  inline void FullBufLoad(short *buf, long bufsize);
  inline void FullBufLoad(float*buf, long bufsize);

  inline void CleanUp();
};

void errorCallback( RtAudioError::Type type, const std::string &errorText );

int output_call_back( void *outputBuffer, void * /*inputBuffer*/, unsigned int nBufferFrames,
            double /*streamTime*/, RtAudioStreamStatus /*status*/, void *data );


int ring_call_back(void *outputBuffer, void * /*inputBuffer*/, unsigned int nBufferFrames,
	double /*streamTime*/, RtAudioStreamStatus /*status*/, void *data);

// Two-channel sawtooth wave generator.
RtOutput::RtOutput(
    unsigned int _device,
    unsigned int _channels,
    unsigned int _sample_rate_input,
    unsigned int _sample_rate_output,
    unsigned int _shift_size,
    unsigned int _frame_size,
    unsigned long _format
    )
  :RtBase(_device, _channels, _sample_rate_output,_shift_size,_frame_size){
    sample_rate_input = _sample_rate_input;
    sample_rate = _sample_rate_output;
    format = _format;

    float_full_buffer = nullptr;
    resampled_output_buffer = nullptr;

    data.buf = nullptr;
    data.size_unit = sizeof(float);
    data.channels = channels;
    data.sample_rate = _sample_rate_output;

    bufferFrames = frame_size;
    data.stock.store(0);

    src_data.src_ratio = data.sample_rate / (double)sample_rate_input;
    //printf("src ratio %lf\n",src_data.src_ratio);
  }


RtOutput::~RtOutput() {
  printf("Rt Output destructor\n");
  if(float_full_buffer)delete[] float_full_buffer;
  if(resampled_output_buffer)delete[] resampled_output_buffer;
  CleanUp();
}

void RtOutput::InitResampler(int len){
#ifndef NDEBUG
//  printf("RtOutput::InitResampler\n");
#endif
  int temp_len;
  src_data.input_frames = len / channels;
  src_data.output_frames = (long)(src_data.input_frames*src_data.src_ratio);
  
  temp_len = src_data.output_frames * channels;
  
  if(temp_len > len_max_src_buf){
    len_max_src_buf = temp_len;
    if(resampled_output_buffer)delete[] resampled_output_buffer;
      resampled_output_buffer = new float[len_max_src_buf];
  }
}

/*                  resampling
 * buf[len]   ---------------------> resampled_output_buffer[len*ratio]
 * */
void RtOutput::Resample(float* src_in){
#ifndef NDEBUG
 // printf("RtOutput::Resample\n");
#endif
  src_data.data_in = src_in;
  src_data.data_out = resampled_output_buffer;
  int error;
  if ((error = src_simple(&src_data, SRC_ZERO_ORDER_HOLD, channels))){
    printf("\n\nLine %d : %s\n\n", __LINE__, src_strerror(error));
    exit(1);
  };
}


void RtOutput::OpenOutputStream(){
#if defined __LINUX_ALSA__	
  //options.flags = RTAUDIO_ALSA_USE_DEFAULT | RTAUDIO_HOG_DEVICE;
#endif
  if(!rtaudio->isStreamOpen()){
    try {
      rtaudio->openStream(&ioParams, NULL, format, sample_rate, &bufferFrames, &output_call_back, (void *)&data, &options);
      //rtaudio->openStream(&ioParams, NULL, FORMAT, sample_rate, &bufferFrames, &float_samplerate_convert_output_call_back, (void *)&data, );
    }
    catch (RtAudioError& e) {
      std::cout << "ERROR::" << e.getMessage() << '\n' << std::endl;
      CleanUp();
    }
  }
}

// WIP
/* 버퍼 크기를 어떻게 할 것인가?
 * exeption 상황에는 어떤 것들이 있을 것인가? 
 * */
void RtOutput::PrepStream(int size_buf){
  data.frameCounter = 0;
  data.appendCounter = 0;

  /* 큰 버퍼 */

  /* 재생과 저장 모두 OutputData.totalFrames 기준으로 동작한다. 
   *
   * */
  ring_output = new float[size_buf * data.channels];
  data.totalFrames = size_buf*data.channels*data.size_unit;

}

// WIP
/*
 * OutputData.buf 를 순환하며 저장
 * */
void RtOutput::BufAppend(float* input, int len ){
  //  InitResampler(len);
  // Resample(buf);

  // TODO 로컬 변수 너무 많나? 
  int size_write =  data.channels * data.size_unit * len ;
  int avail =  data.totalFrames - data.appendCounter;
  int left = size_write - left;

  /* 한 바퀴 돎. */
  if( data.appendCounter + len*data.channels * data.size_unit > data.totalFrames ){
    memcpy(reinterpret_cast<float*>(data.buf) + data.appendCounter,input, avail);
    memcpy(reinterpret_cast<float*>(data.buf),input + avail ,left);
    data.appendCounter = left;
  }else{
    memcpy(reinterpret_cast<float*>(data.buf) + data.appendCounter,input, size_write);

    data.appendCounter +=size_write;
  }
   data.stock.fetch_add(size_write);
}

void RtOutput::BufAppend(short* input ){
  // src_short_to_float_array(buf, float_full_buffer, len);
  // InitResampler(len);
  //  Resample(float_full_buffer);
}

/* FullBufLoad for float*  */
void RtOutput::FullBufLoad( float *buf, long len) {
#ifndef NDEBUG
 // printf("RtOutput::FullBufload(float)\n");
#endif
  data.frameCounter = 0;
  InitResampler(len);
  Resample(buf);


  data.size = src_data.output_frames*channels;
  data.buf =  resampled_output_buffer;

  data.totalFrames = data.size * sizeof(float);

  OpenOutputStream();
}

/* FullBufLoad for short*
 * Note : need to convet short array to float array for resampling.  */
void RtOutput::FullBufLoad( short *buf, long len) {
#ifndef NDEBUG
 // printf("RtOutput::FullBufload(short)\n");
#endif

  if(len > len_max_out_buf){
    len_max_out_buf = len;
    if(float_full_buffer) delete[] float_full_buffer;

    float_full_buffer = new float[len_max_out_buf];
  }

  data.frameCounter = 0;
  src_short_to_float_array(buf, float_full_buffer, len);
  InitResampler(len);
  Resample(float_full_buffer);

  data.size = src_data.output_frames*channels;
  data.buf =  resampled_output_buffer;
  data.totalFrames = data.size * sizeof(float);

  OpenOutputStream();
}

int output_call_back(void *outputBuffer, void * /*inputBuffer*/, unsigned int nBufferFrames,
    double /*streamTime*/, RtAudioStreamStatus /*status*/, void *data){

  //return 1;
  OutputData *oData = (OutputData*)data;
  unsigned int frames = nBufferFrames;
  if (oData->totalFrames - oData->frameCounter <= frames * oData->channels * oData->size_unit) {
#ifndef NDEBUG
    //		printf("NOTE::RtOutput %d - %d (%d) < %d\n", oData->totalFrames, oData->frameCounter, 
    //		oData->totalFrames - oData->frameCounter, frames * oData->channels * oData->size_unit);
#endif
    memcpy(outputBuffer, (char*)(oData->buf) + oData->frameCounter, (oData->totalFrames - oData->frameCounter) );
    oData->frameCounter = oData->size * oData->size_unit;

    return 1;
  }
  memcpy(outputBuffer, (char*)(oData->buf) + oData->frameCounter, frames * oData->channels * oData->size_unit);
  oData->frameCounter += (frames * oData->channels  * oData->size_unit);
  //	printf("CUR %d - %d (%d) < %d\n", oData->totalFrames, oData->frameCounter, oData->totalFrames - oData->frameCounter, oData->size * sizeof(short));
  //printf("CALLBACK : pos %d | frame %d | delta %d\n",oData->frameCounter, frames, frames * oData->channels * sizeof(short));

  return 0;
}

/*
 * OutputData.buf를 순환하며 재생. 
 *
 * */
int ring_call_back(void *outputBuffer, void * /*inputBuffer*/, unsigned int nBufferFrames,
    double /*streamTime*/, RtAudioStreamStatus /*status*/, void *data){

  /* 순환 하여야한다. */
  OutputData *oData = (OutputData*)data;
  unsigned int frames = nBufferFrames;
  int size_play = frames * oData->channels * oData->size_unit;

  /* There is thing to play?  */
  if(oData->stock.load() < size_play){
    printf("*******************************************************\n");
    printf("** FAIL:Nothing to Play!                             **\n");
    printf("*******************************************************\n");
    exit(-1);
  }
  
  /* 한바퀴 다 돎. */
  if (oData->totalFrames - oData->frameCounter <= size_play) {
    // 끄트머리에 남은 양
    int left = oData->totalFrames - oData->frameCounter;
    // 처음에서 채워야할 양
    int leftover = frames * oData->channels * oData->size_unit - left;

    memcpy(outputBuffer, (char*)(oData->buf) + oData->frameCounter, left );
    memcpy((char*)(outputBuffer) + left, (char*)(oData->buf) , leftover );
    //oData->frameCounter = oData->size * oData->size_unit;
    oData->frameCounter = leftover;
    oData->stock.fetch_sub(size_play);
    return 0;
  }

  memcpy(outputBuffer, (char*)(oData->buf) + oData->frameCounter, frames * oData->channels * oData->size_unit);
  oData->frameCounter += size_play;
  return 0;
  

}

void RtOutput::CleanUp() {

  if (rtaudio->isStreamOpen())
    rtaudio->closeStream();
  delete rtaudio;
}

void errorCallback( RtAudioError::Type type, const std::string &errorText ){
  // This example error handling function does exactly the same thing
  // as the embedded RtAudio::error() function.
  std::cout << "in errorCallback" << std::endl;
  if ( type == RtAudioError::WARNING )
    std::cerr << '\n' << errorText << "\n\n";
  else if ( type != RtAudioError::WARNING )
    throw( RtAudioError( errorText, type ) );
}
#endif
