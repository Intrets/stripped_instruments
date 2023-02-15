cmake_minimum_required(VERSION 3.20)

set(MODULE_NAME stmlib)
project(${MODULE_NAME} LANGUAGES CXX)

file(
	GLOB_RECURSE FILES 
	${CMAKE_CURRENT_SOURCE_DIR}/stmlib/stmlib.h
	${CMAKE_CURRENT_SOURCE_DIR}/stmlib/dsp/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/stmlib/dsp/*.cc
	${CMAKE_CURRENT_SOURCE_DIR}/stmlib/dsp/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/stmlib/dsp/*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/stmlib/dsp/*.h

	${CMAKE_CURRENT_SOURCE_DIR}/stmlib/utils/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/stmlib/utils/*.cc
	${CMAKE_CURRENT_SOURCE_DIR}/stmlib/utils/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/stmlib/utils/*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/stmlib/utils/*.h
)

add_library(${MODULE_NAME} ${FILES})
target_include_directories(${MODULE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(${MODULE_NAME} PUBLIC TEST)
target_compile_definitions(${MODULE_NAME} PUBLIC M_PI=3.141592653589793)
