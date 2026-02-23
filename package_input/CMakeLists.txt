cmake_minimum_required(VERSION 3.21)
project(RtAudioTest)

set(LINKLIBS)
set(COMPILE_OPTION)
set(SRC)
set(INCL)
  
if(NOT MSVC)
	list(APPEND LINKLIBS
	m
	)
endif()


if(APPLE)
 set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64")
endif()

list(APPEND SRC
	RtBase.h
	RtInput.h
	WAV.h
)
	
list(APPEND INCL
  ${CMAKE_CURRENT_SOURCE_DIR}
)

list(APPEND COMPILE_OPTION
  -g
)

include(RtAudio.cmake)


## RtAudio C++ Input Test
add_executable(${PROJECT_NAME} test_RTA_input.cpp ${SRC})
target_include_directories(${PROJECT_NAME}	PRIVATE	${INCL}  )

target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_OPTION} )
set_target_properties(${PROJECT_NAME} PROPERTIES  CXX_STANDARD 14 )