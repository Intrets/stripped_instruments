cmake_minimum_required(VERSION 3.20)

set(MODULE_NAME plaits)
project(${MODULE_NAME} LANGUAGES CXX)

file(
	GLOB_RECURSE FILES 
	${CMAKE_CURRENT_SOURCE_DIR}/plaits/dsp/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/plaits/dsp/*.cc
	${CMAKE_CURRENT_SOURCE_DIR}/plaits/dsp/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/plaits/dsp/*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/plaits/dsp/*.h
)

add_library( ${MODULE_NAME}
	${FILES}
	${CMAKE_CURRENT_SOURCE_DIR}/plaits/resources.cc
)
target_include_directories(${MODULE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${MODULE_NAME} PUBLIC stmlib)
