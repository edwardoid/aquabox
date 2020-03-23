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

if (TARGET_DEVICE)
  add_definitions(-DTARGET_DEVICE)
endif()

#set(PKG_CONFIG_USE_CMAKE_PREFIX_PATH YES)
set(THREE_DP_DIR "${PROJECT_SOURCE_DIR}/3dp")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_CPPCHECK "")
set(PLOG_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/plog/include")
set(THREAD_POOL_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/thread-pool-cpp/include")
set(NLOHMANN_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/nlohmann/include")
set(JSV_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/json-schema-validator/src")
set(SWS_HTTP_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/swshttp")
set(SWS_WS_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/swsws")
set(PSTREAMS_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/pstreams")
set(HEADER_SEARCH_PATHS ${PROJECT_INCLUDE_DIR} ${PLOG_INCLUDE_DIR})
set(NLOHMANN_JSON_DIR ${NLOHMANN_INCLUDE_DIR})
set(S2S_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/s2s")
add_library(json-hpp INTERFACE)
set(NLOHMANN_JSON_DIR ${NLOHMANN_INCLUDE_DIR}/nlohmann)
set(AQUABOX_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})

if (NOT CLOUD_ONLY)
  set(QRENDOCE_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/qrencode")
  set(FB_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/libfb/include")
  set(OWT_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/owt/include")

  option(FREETYPE ON)

endif()

option(BUILD_TESTING OFF)
option(WITH_TESTS OFF)
option(WITH_TOOLS OFF)

find_package(OpenSSL REQUIRED)
find_package (Threads)

include_directories(${PLOG_INCLUDE_DIR})
include_directories(${NLOHMANN_INCLUDE_DIR})
include_directories(${JSV_INCLUDE_DIR})
include_directories(${THREAD_POOL_INCLUDE_DIR})
include_directories(${SWS_HTTP_INCLUDE_DIR})
include_directories(${SWS_WS_INCLUDE_DIR})
include_directories(${PSTREAMS_INCLUDE_DIR})
include_directories(${S2S_INCLUDE_DIR})

if (NOT CLOUD_ONLY)
  include_directories(${QRENDOCE_INCLUDE_DIR})
  include_directories(${FB_INCLUDE_DIR})
  include_directories(${IWS_INCLUDE_DIR})
  include_directories(${OWT_INCLUDE_DIR})
  include_directories(${PNG_INCLUDE_DIR})
endif()

add_subdirectory("${THREE_DP_DIR}/plog")
add_subdirectory("${THREE_DP_DIR}/swshttp")
add_subdirectory("${THREE_DP_DIR}/swsws")
add_subdirectory("${THREE_DP_DIR}/nlohmann")
add_subdirectory("${THREE_DP_DIR}/json-schema-validator")
add_subdirectory("${THREE_DP_DIR}/backward-cpp")

if (NOT CLOUD_ONLY)
  add_subdirectory("${THREE_DP_DIR}/qrencode")
  add_subdirectory("${THREE_DP_DIR}/libfb")
  add_subdirectory("${THREE_DP_DIR}/owt")
endif()

execute_process(
  COMMAND git log -1 --format=%H
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

find_package(PkgConfig REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(OpenSSL)

if (NOT CLOUD_ONLY)
  find_package(PNG REQUIRED)
  find_package(Freetype REQUIRED)
  link_directories(${NETWORKMANAGER_LIBRARY_DIRS})
endif()

link_directories(${Boost_LIBRARY_DIRS})

add_subdirectory("src")

if (NOT TARGET_DEVICE)
  add_subdirectory("tests")
endif()
