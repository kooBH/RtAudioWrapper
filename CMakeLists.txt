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

list(APPEND SRC
	RtBase.h
	RtInput.h
	RtOutput.h
	
	RTAw.h
	
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


## RtAudioWrapper Library
add_library(RTAw SHARED 
	${SRC} 
	RTAw_wrap.cpp)
target_include_directories(RTAw	PRIVATE	${INCL}  )
target_link_libraries(RTAw ${LINKLIBS})
target_compile_options(RTAw PRIVATE ${COMPILE_OPTION} )
set_target_properties(RTAw PROPERTIES  CXX_STANDARD 17 )

## RtAduioWrapper Test
add_executable(RTAw_test test/test_RTAw.cpp )
target_include_directories(RTAw_test	PRIVATE	${INCL}  )
target_link_libraries(RTAw_test
	debug ${LINKLIBS} ${CMAKE_CURRENT_LIST_DIR}/build/Debug/RTAw.lib
	optimized ${LINKLIBS} ${CMAKE_CURRENT_LIST_DIR}/build/Release/RTAw.lib
)
target_compile_options(RTAw_test PRIVATE ${COMPILE_OPTION} )
set_target_properties(RTAw_test PROPERTIES  CXX_STANDARD 14 )




## RtOutput Test
add_executable(${PROJECT_NAME}_output test/test_output.cpp ${SRC})
target_include_directories(${PROJECT_NAME}_output	PRIVATE	${INCL}  )
target_link_libraries(${PROJECT_NAME}_output ${LINKLIBS})
target_compile_options(${PROJECT_NAME}_output PRIVATE ${COMPILE_OPTION} )
set_target_properties(${PROJECT_NAME}_output PROPERTIES  CXX_STANDARD 14 )