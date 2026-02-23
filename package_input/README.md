# RtAudio 마이크 입력 예제  

RtAudio를 사용한 마이크 입력 샘플 프로젝트  

[RtAudio](https://caml.music.mcgill.ca/~gary/rtaudio/),  
```test_RTA_input.cpp``` 참조.  

## 빌드  
- 요구사항  
  - [CMake](https://cmake.org/download/)  

1. build 폴더 생성  
2. build 폴더에서 터미널 실행 후 cmake 빌드(또는 CMake GUI사용)  
``` cmake .. ```
3. 생성된 MSVC 솔루션 시작    
4. RtAudioTest 프로젝트를 시작 프로젝트로 설정  

## 예제 코드 동작  

1. ```AudioProbe``` 를 통해 연결된 오디오 장치 목록을 받음  
2. 목록에서 입력 장치의 index를 입력하여 녹음 시작  
3. 5초 동안의 마이크 입력을 ```test.wav```로 저장함.  
