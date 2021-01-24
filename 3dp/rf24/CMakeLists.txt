cmake_minimum_required(VERSION 3.5)
project(rf24meshgw VERSION 1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 11)

file(GLOB CPP_SOURCES "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp")
file(GLOB C_SOURCES "${CMAKE_CURRENT_LIST_DIR}/src/*.c")
file(GLOB HEADERS "${CMAKE_CURRENT_LIST_DIR}/include/*.h")
file(GLOB UTIL_HEADERS "${CMAKE_CURRENT_LIST_DIR}/include/utility/*.h")
file(GLOB SPI_HEADERS "${CMAKE_CURRENT_LIST_DIR}/include/utility/SPIDEV/*.h")


add_library(${PROJECT_NAME} SHARED ${CPP_SOURCES} ${C_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/include/utility/SPIDEV")

set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION 1)

set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE 1)
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION 1)
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER HEADERS)

INSTALL(TARGETS ${PROJECT_NAME} 
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include
)


include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
include_directories(${FREETYPE_INCLUDE_DIRS})
include_directories(${PNG_INCLUDE_DIRS})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -marm -march=armv6zk -mtune=arm1176jzf-s -mfpu=vfp -mfloat-abi=hard -Ofast")

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE 1)
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION 1)

