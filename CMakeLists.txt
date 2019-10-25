# AquaBox
# Copyright (C) 2019  Eduard Sargsyan
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


cmake_minimum_required(VERSION 3.5)
project(aquabox)

if (TARGET_DEVICE)
  add_definitions(-DTARGET_DEVICE)
  add_definitions(-DW1_BUS_ROOT="/sys/bus/w1/devices")
  set(CMAKE_INSTALL_PREFIX "${CMAKE_SYSROOT}/aquabox")
endif()

MESSAGE(STATUS "AquaBox will be installed to ${CMAKE_INSTALL_PREFIX}")

set(PKG_CONFIG_USE_CMAKE_PREFIX_PATH YES)
set(THREE_DP_DIR "${PROJECT_SOURCE_DIR}/3dp")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_CPPCHECK "")
set(PLOG_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/plog/include")
set(THREAD_POOL_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/thread-pool-cpp/include")
set(QRENDOCE_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/qrencode")
set(NLOHMANN_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/nlohmann/include")
set(JSV_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/json-schema-validator/src")
set(SWS_HTTP_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/swshttp")
set(SWS_WS_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/swsws")
set(PSTREAMS_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/pstreams")
set(FB_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/libfb/include")
set(IWS_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/iws/include")
set(OWT_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/owt/include")
set(HEADER_SEARCH_PATHS ${PROJECT_INCLUDE_DIR} ${PLOG_INCLUDE_DIR})
set(NLOHMANN_JSON_DIR ${NLOHMANN_INCLUDE_DIR})
add_library(json-hpp INTERFACE)

option(BUILD_TESTING OFF)
option(WITH_TESTS OFF)
option(WITH_TOOLS OFF)
option(FREETYPE ON)

find_package(OpenSSL REQUIRED)

include_directories(${PLOG_INCLUDE_DIR})
include_directories(${QRENDOCE_INCLUDE_DIR})
include_directories(${NLOHMANN_INCLUDE_DIR})
include_directories(${JSV_INCLUDE_DIR})
include_directories(${THREAD_POOL_INCLUDE_DIR})
include_directories(${SWS_HTTP_INCLUDE_DIR})
include_directories(${SWS_WS_INCLUDE_DIR})
include_directories(${PSTREAMS_INCLUDE_DIR})
include_directories(${FB_INCLUDE_DIR})
include_directories(${IWS_INCLUDE_DIR})
include_directories(${OWT_INCLUDE_DIR})
include_directories(${PNG_INCLUDE_DIR})
include_directories(${PNG_INCLUDE_DIR})

add_subdirectory("${THREE_DP_DIR}/plog")
add_subdirectory("${THREE_DP_DIR}/swshttp")
add_subdirectory("${THREE_DP_DIR}/swsws")
add_subdirectory("${THREE_DP_DIR}/qrencode")
add_subdirectory("${THREE_DP_DIR}/nlohmann")
add_subdirectory("${THREE_DP_DIR}/json-schema-validator")
add_subdirectory("${THREE_DP_DIR}/iws")
add_subdirectory("${THREE_DP_DIR}/libfb")
add_subdirectory("${THREE_DP_DIR}/owt")

execute_process(
  COMMAND git log -1 --format=%H
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

file(GLOB SOURCES "src/*.cpp")

set(AB_V 1)
set(AB_v 0)

find_package(PkgConfig REQUIRED)
find_package(PNG REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(Freetype REQUIRED)
find_package(OpenSSL)

#set(CMAKE_C_FLAGS ${NETWORKMANAGER_CFLAGS})

link_directories(${Boost_LIBRARY_DIRS})
link_directories(${NETWORKMANAGER_LIBRARY_DIRS})

add_executable(${PROJECT_NAME} ${SOURCES})
target_compile_definitions(${PROJECT_NAME} PUBLIC AB_VERSION_MAJOR=${AB_V})
target_compile_definitions(${PROJECT_NAME} PUBLIC AB_VERSION_MINOR=${AB_v})
target_compile_definitions(${PROJECT_NAME} PUBLIC AB_VERSION=\"${AB_V}.${AB_v}\")
target_compile_definitions(${PROJECT_NAME} PUBLIC AB_GIT_HASH=\"${GIT_COMMIT_HASH}\")


if(OPENSSL_FOUND)
    target_compile_definitions(${PROJECT_NAME} INTERFACE HAVE_OPENSSL)
    target_link_libraries(${PROJECT_NAME} ${OPENSSL_LIBRARIES})
    include_directories(${PROJECT_NAME} ${OPENSSL_INCLUDE_DIR})
endif()

target_link_libraries(${PROJECT_NAME} qrencode)
target_link_libraries(${PROJECT_NAME} fb_static)
target_link_libraries(${PROJECT_NAME} iotwifi_static)
target_link_libraries(${PROJECT_NAME} w1temp_static)
target_link_libraries(${PROJECT_NAME} json-schema-validator)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${OPENSSL_CRYPTO_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${PNG_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${ZLIB_LIBRARIES})
target_link_libraries(${PROJECT_NAME} Boost::system)
target_link_libraries(${PROJECT_NAME} ${NETWORKMANAGER_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${FREETYPE_LIBRARIES})
target_link_libraries(${PROJECT_NAME} OpenSSL::SSL)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
