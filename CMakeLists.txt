cmake_minimum_required(VERSION 3.5)
project(aquabox)


set(THREE_DP_DIR "${PROJECT_SOURCE_DIR}/3dp")

add_subdirectory("${THREE_DP_DIR}/plog")

SET(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/fff/ttt")


option(PISTACHE_BUILD_DOCS OFF)
option(PISTACHE_BUILD_TESTS OFF)
option(BUILD_TESTING OFF)
option(PISTACHE_BUILD_EXAMPLES OFF)
option(WITH_TESTS OFF)

add_subdirectory("${THREE_DP_DIR}/pistache")
add_subdirectory("${THREE_DP_DIR}/qrencode")
add_subdirectory("${THREE_DP_DIR}/nlohmann")

set(CMAKE_CXX_STANDARD 11)

set(PLOG_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/plog/include")
set(PISTACHE_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/pistache/include")

set(QRENDOCE_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/qrencode")
set(NLOHMANN_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/nlohmann/include")

set(HEADER_SEARCH_PATHS ${PROJECT_INCLUDE_DIR} ${PLOG_INCLUDE_DIR})

configure_file (
        "${PISTACHE_INCLUDE_DIR}/pistache/version.h.in"
        "include/pistache/version.h"
        @ONLY
    )

configure_file (
        "${THREE_DP_DIR}/pistache/libpistache.pc.in"
        "libpistache.pc"
        @ONLY
    )

execute_process(
  COMMAND git log -1 --format=%H
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

include_directories(${PLOG_INCLUDE_DIR})
include_directories(${PISTACHE_INCLUDE_DIR})
include_directories(${QRENDOCE_INCLUDE_DIR})
include_directories(${NLOHMANN_INCLUDE_DIR})

file(GLOB SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})
set(AB_V 1)
set(AB_v 0)
target_compile_definitions(${PROJECT_NAME} PUBLIC AB_VERSION_MAJOR=${AB_V})
target_compile_definitions(${PROJECT_NAME} PUBLIC AB_VERSION_MINOR=${AB_v})
target_compile_definitions(${PROJECT_NAME} PUBLIC AB_VERSION=\"${AB_V}.${AB_v}\")
target_compile_definitions(${PROJECT_NAME} PUBLIC AB_GIT_HASH=\"${GIT_COMMIT_HASH}\")


target_link_libraries(${PROJECT_NAME} pistache_shared)
target_link_libraries(${PROJECT_NAME} qrencode)


find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})


target_link_libraries(${PROJECT_NAME} ${PNG_LIBRARY})
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
