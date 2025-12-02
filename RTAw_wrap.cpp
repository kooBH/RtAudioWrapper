#include "RTAw.h"
#include "RtInput.h"
#include "RtOutput.h"

void RTAw_audioprobe() {
  AudioProbe();
}

void* RTAw_input_construct(int device, int channels, int samplerate, int buf_size) {
  return static_cast<void*>(new RtInput(device, channels, samplerate, buf_size));
}

void RTAw_input_start(void*obj) {
  static_cast<RtInput*>(obj)->Start();
}

int RTAw_input_buf_size(void*obj) {
  RtInput* input = static_cast<RtInput*>(obj);
  return input->GetInputSize();
}

int RTAw_input_buffer_length(void*obj) {
  RtInput* input = static_cast<RtInput*>(obj);
  return input->data.stock.load();
}

void RTAw_input_buffer_read(void*obj, short*buf) {
  RtInput* input = static_cast<RtInput*>(obj);
  input->GetBuffer(buf);
}

void RTAw_input_stop(void* obj) {
  static_cast<RtInput*>(obj)->Stop();
}

void RTAw_input_release(void*obj) {
  delete static_cast<RtInput*>(obj);

}