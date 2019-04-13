cmake_minimum_required(VERSION 3.5)
project(aquabox)

set(THREE_DP_DIR "${PROJECT_SOURCE_DIR}/3dp")
set(CMAKE_INSTALL_PREFIX "$ENV{RASPB_SYSROOT}/home/ed/aquabox")
set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_CPPCHECK "")
set(PLOG_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/plog/include")
set(THREAD_POOL_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/thread-pool-cpp/include")
set(QRENDOCE_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/qrencode")
set(NLOHMANN_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/nlohmann/include")
set(SWS_HTTP_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/swshttp")
set(SWS_WS_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3dp/swsws")
set(HEADER_SEARCH_PATHS ${PROJECT_INCLUDE_DIR} ${PLOG_INCLUDE_DIR})

option(BUILD_TESTING OFF)
option(WITH_TESTS OFF)

include_directories(${PLOG_INCLUDE_DIR})
include_directories(${QRENDOCE_INCLUDE_DIR})
include_directories(${NLOHMANN_INCLUDE_DIR})
include_directories(${THREAD_POOL_INCLUDE_DIR})
include_directories(${SWS_HTTP_INCLUDE_DIR})
include_directories(${SWS_WS_INCLUDE_DIR})

add_subdirectory("${THREE_DP_DIR}/plog")
add_subdirectory("${THREE_DP_DIR}/swshttp")
add_subdirectory("${THREE_DP_DIR}/swsws")
add_subdirectory("${THREE_DP_DIR}/qrencode")
add_subdirectory("${THREE_DP_DIR}/nlohmann")

execute_process(
  COMMAND git log -1 --format=%H
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

file(GLOB SOURCES "src/service/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})
set(AB_V 1)
set(AB_v 0)
target_compile_definitions(${PROJECT_NAME} PUBLIC AB_VERSION_MAJOR=${AB_V})
target_compile_definitions(${PROJECT_NAME} PUBLIC AB_VERSION_MINOR=${AB_v})
target_compile_definitions(${PROJECT_NAME} PUBLIC AB_VERSION=\"${AB_V}.${AB_v}\")
target_compile_definitions(${PROJECT_NAME} PUBLIC AB_GIT_HASH=\"${GIT_COMMIT_HASH}\")


find_package(PNG REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
include_directories(${PNG_INCLUDE_DIR})


link_directories(${Boost_LIBRARY_DIRS})
target_link_libraries(${PROJECT_NAME} qrencode)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${OPENSSL_CRYPTO_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${PNG_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${ZLIB_LIBRARIES})
target_link_libraries(${PROJECT_NAME} Boost::system)
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
