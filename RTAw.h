
#ifndef RTAW_H
#define RTAW_H

#if _WIN32
#ifdef _EXPORT_
#define DLL_PREFIX extern "C" __declspec(dllexport)
#else
#define DLL_PREFIX extern "C" __declspec(dllimport)
#endif
#else
#define DLL_PREFIX
#endif

DLL_PREFIX void RTAw_audioprobe();
DLL_PREFIX int RTAw_count_devices();
DLL_PREFIX void RTAw_get_device_name(int, char*);

DLL_PREFIX void* RTAw_input_construct(int device,int channels,int samplerate,int buf_size);
DLL_PREFIX void RTAw_input_start(void*);
DLL_PREFIX int RTAw_input_buf_size(void*);
DLL_PREFIX int RTAw_input_buffer_length(void*);
DLL_PREFIX void RTAw_input_buffer_read(void*,short*);
DLL_PREFIX void RTAw_input_stop(void*);
DLL_PREFIX void RTAw_input_release(void*);

#endif
