cmake_minimum_required(VERSION 2.8)
project(WBMpluss)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

include_directories(/usr/local/share/ghaas/include)
link_directories(/usr/local/share/ghaas/lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

FILE(GLOB sources src/*.c)

add_executable(WBMplus.bin ${sources})
if(${CMAKE_HOST_APPLE})
	target_link_libraries(WBMplus.bin CM10 MF10 m)
else(${CMAKE_HOST_APPLE})
	target_link_libraries(WBMplus.bin CM10 MF10 m -pthread)
endif(${CMAKE_HOST_APPLE})