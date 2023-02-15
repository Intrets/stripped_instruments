cmake_minimum_required(VERSION 3.20)

set(MODULE_NAME rings)
project(${MODULE_NAME} LANGUAGES CXX)

file(
	GLOB_RECURSE FILES 
	${CMAKE_CURRENT_SOURCE_DIR}/rings/dsp/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/rings/dsp/*.cc
	${CMAKE_CURRENT_SOURCE_DIR}/rings/dsp/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/rings/dsp/*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/rings/dsp/*.h
)

add_library( ${MODULE_NAME}
	${FILES}
	${CMAKE_CURRENT_SOURCE_DIR}/rings/resources.cc
)
target_include_directories(${MODULE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${MODULE_NAME} PUBLIC stmlib)
