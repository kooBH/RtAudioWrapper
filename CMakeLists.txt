# CMAKE version of Ubuntu 16.04 is 3.5.
cmake_minimum_required(VERSION 3.5)
project(test)

set(LINKLIBS)
set(COMPILE_OPTION)
set(SRC)
set(INCL)
  
if(NOT MSVC)
	list(APPEND LINKLIBS
	m
	)
endif()

list(APPEND SRC
	RtBase.h
	RtInput.h
	RtOutput.h
	
	test/WAV/WAV.h
)
	
list(APPEND INCL
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/test/WAV
)

list(APPEND COMPILE_OPTION
  -g
)

include(RtAudio.cmake)
include(Libsamplerate.cmake)

add_executable(${PROJECT_NAME}_output test/test_output.cpp ${SRC})
target_include_directories(${PROJECT_NAME}_output	PRIVATE	${INCL}  )
target_link_libraries(${PROJECT_NAME}_output ${LINKLIBS})
target_compile_options(${PROJECT_NAME}_output PRIVATE ${COMPILE_OPTION} )
set_target_properties(${PROJECT_NAME}_output PROPERTIES  CXX_STANDARD 14 )