cmake_minimum_required(VERSION 3.5)
project(aquabox-service)

find_package(libmongocxx REQUIRED)

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
include_directories(${LIBMONGOCXX_INCLUDE_DIRS})

file(GLOB AQUABOX_CLOUD_SOURCES "*.cpp")

add_executable(${PROJECT_NAME} ${AQUABOX_CLOUD_SOURCES})

target_link_libraries(${PROJECT_NAME} aquabox-common_static)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${OPENSSL_CRYPTO_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${PNG_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${ZLIB_LIBRARIES})
target_link_libraries(${PROJECT_NAME} Boost::system)
target_link_libraries(${PROJECT_NAME} OpenSSL::SSL)
target_link_libraries(${PROJECT_NAME} ${LIBMONGOCXX_LIBRARIES})
target_link_libraries (${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})


install(TARGETS ${PROJECT_NAME} DESTINATION bin)
