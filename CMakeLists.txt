cmake_minimum_required(VERSION 2.8)
project(WBM30)

set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if(${GHAASDIR})
    include_directories(${GHAASDIR}/include)
    link_directories(${GHAASDIR}/lib)
else(${GHAASDIR})
    include_directories(/usr/local/share/ghaas/include)
    link_directories(/usr/local/share/ghaas/lib)
endif(${GHAASDIR})

FILE(GLOB sources src/*.c)
add_executable(WBM30 ${sources})

if(${CMAKE_HOST_APPLE})
    target_link_libraries(WBM30 CM30 MF30 m)
else(${CMAKE_HOST_APPLE})
    target_link_libraries(WBM30 CM30 MF30 m -pthread)
endif(${CMAKE_HOST_APPLE})

target_include_directories(WBM30 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
