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
project(AquaboxProject)

#set(INSTALL_RPATH_USE_LINK_PATH true)

set(CMAKE_INSTALL_RPATH "/aquabox/lib")

set(LIBSTATS_SHARED ON)

# Build Types
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE}
    CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel tsan asan lsan msan ubsan"
    FORCE)

# ThreadSanitizer
set(CMAKE_C_FLAGS_TSAN
    "-fsanitize=thread -g -O1"
    CACHE STRING "Flags used by the C compiler during ThreadSanitizer builds."
    FORCE)
set(CMAKE_CXX_FLAGS_TSAN
    "-fsanitize=thread -g -O1"
    CACHE STRING "Flags used by the C++ compiler during ThreadSanitizer builds."
    FORCE)

# AddressSanitize
set(CMAKE_C_FLAGS_ASAN
    "-fsanitize=address -g -O1"
    CACHE STRING "Flags used by the C compiler during AddressSanitizer builds."
    FORCE)
set(CMAKE_CXX_FLAGS_ASAN
    "-fsanitize=address -fno-optimize-sibling-calls -fsanitize-address-use-after-scope -fno-omit-frame-pointer -g -O1"
    CACHE STRING "Flags used by the C++ compiler during AddressSanitizer builds."
    FORCE)

# LeakSanitizer
set(CMAKE_C_FLAGS_LSAN
    "-fsanitize=leak -fno-omit-frame-pointer -g -O1"
    CACHE STRING "Flags used by the C compiler during LeakSanitizer builds."
    FORCE)
set(CMAKE_CXX_FLAGS_LSAN
    "-fsanitize=leak -fno-omit-frame-pointer -g -O1"
    CACHE STRING "Flags used by the C++ compiler during LeakSanitizer builds."
    FORCE)

# MemorySanitizer
set(CMAKE_C_FLAGS_MSAN
    "-fsanitize=memory -fno-optimize-sibling-calls -fsanitize-memory-track-origins=2 -fno-omit-frame-pointer -g -O2"
    CACHE STRING "Flags used by the C compiler during MemorySanitizer builds."
    FORCE)
set(CMAKE_CXX_FLAGS_MSAN
    "-fsanitize=memory -fno-optimize-sibling-calls -fsanitize-memory-track-origins=2 -fno-omit-frame-pointer -g -O2"
    CACHE STRING "Flags used by the C++ compiler during MemorySanitizer builds."
    FORCE)

# UndefinedBehaviour
set(CMAKE_C_FLAGS_UBSAN
    "-fsanitize=undefined"
    CACHE STRING "Flags used by the C compiler during UndefinedBehaviourSanitizer builds."
    FORCE)
set(CMAKE_CXX_FLAGS_UBSAN
    "-fsanitize=undefined"
    CACHE STRING "Flags used by the C++ compiler during UndefinedBehaviourSanitizer builds."
    FORCE)
    

set(ENV{PKG_CONFIG_PATH}  "/usr/lib/arm-linux-gnueabihf/pkgconfig")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH})
conan_basic_setup()

if(("$ENV{BUILD_PLATFORM}" EQUAL "armv6") OR ("$ENV{BUILD_PLATFORM}" EQUAL "armv7"))
  option(TARGET_DEVICE ON)
endif()


add_definitions(-DTARGET_DEVICE)

if (TARGET_DEVICE)
endif()

set(THREE_DP_DIR "${PROJECT_SOURCE_DIR}/3dp")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_CPPCHECK "")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_TESTING OFF)
set(LIBFB_SHARED ON)
set(LIBOWT_SHARED ON)

set(PLOG_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/plog/include")
set(THREAD_POOL_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/thread-pool-cpp/include")
set(SWS_HTTP_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/swshttp")
set(SWS_WS_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/swsws")
set(PSTREAMS_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/pstreams")
set(HEADER_SEARCH_PATHS ${PROJECT_INCLUDE_DIR} ${PLOG_INCLUDE_DIR})
set(S2S_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/s2s")
set(NM_DBUS_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/nm-dbus/include")
set(LIBSTATS_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/libstats/include")
set(AQUABOX_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})

add_definitions(-DBOOST_STACKTRACE_USE_ADDR2LINE)

if (NOT CLOUD_ONLY)
  set(FB_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/libfb/include")
  set(OWT_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/owt/include")
  option(FREETYPE ON)
endif()

find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(libqrencode REQUIRED)
find_package(simppl REQUIRED)
find_package(dbus REQUIRED)
find_package(JsonSchemaValidator REQUIRED)
find_package(tgbot REQUIRED)
find_package(libserial REQUIRED)

include_directories(${PLOG_INCLUDE_DIR})
include_directories(${THREAD_POOL_INCLUDE_DIR})
include_directories(${SWS_HTTP_INCLUDE_DIR})
include_directories(${SWS_WS_INCLUDE_DIR})
include_directories(${PSTREAMS_INCLUDE_DIR})
include_directories(${S2S_INCLUDE_DIR})
include_directories(${LIBSTATS_INCLUDE_DIR})

include_directories(${FB_INCLUDE_DIR})
include_directories(${OWT_INCLUDE_DIR})
include_directories(${PNG_INCLUDE_DIR})

add_subdirectory("${THREE_DP_DIR}/plog")
add_subdirectory("${THREE_DP_DIR}/libfb")
add_subdirectory("${THREE_DP_DIR}/owt")
add_subdirectory("${THREE_DP_DIR}/nm-dbus")
add_subdirectory("${THREE_DP_DIR}/libstats")

execute_process(
  COMMAND git log -1 --format=%H
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

find_package(PkgConfig REQUIRED)
find_package(Boost REQUIRED COMPONENTS system stacktrace)
find_package(OpenSSL)
find_package(mongo-cxx-driver REQUIRED)
find_package(PNG REQUIRED)
find_package(freetype REQUIRED)

link_directories(${Boost_LIBRARY_DIRS})

add_subdirectory("src")

if (BUILD_TESTING)
  add_subdirectory("tests")
endif()
