cmake_minimum_required(VERSION 3.5)
project(aquabox-server)

include_directories(${PLOG_INCLUDE_DIR})
include_directories(${SWS_HTTP_INCLUDE_DIR})
include_directories(${SWS_WS_INCLUDE_DIR})
include_directories(${PSTREAMS_INCLUDE_DIR})
include_directories(${FB_INCLUDE_DIR})
include_directories(${OWT_INCLUDE_DIR})
include_directories(${S2S_INCLUDE_DIR})
include_directories(${AQUABOX_COMMON_PATH})

add_definitions(-DMQTT_USE_WS)

file(GLOB AQUABOX_CLOUD_SOURCES "*.cpp")
include_directories(${AQUABOX_COMMON_PATH})
add_executable(${PROJECT_NAME} ${AQUABOX_CLOUD_SOURCES})

find_package(mongocxx REQUIRED)

target_link_libraries(${PROJECT_NAME}
                                        aquabox-common
                                        dl
                                        rt
                                        mongo::mongocxx_shared
                                        nlohmann_json_schema_validator
                                        boost::boost
                                        openssl::openssl
                                        tgbot::tgbot
                                        mqtt_cpp::mqtt_cpp
                                        Threads::Threads
    ) 

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

configure_file(${AQUABOX_ROOT_DIR}/config/aquabox-server/$ENV{BUILD_PLATFORM}/aquabox.service ${CMAKE_BINARY_DIR}/bin/aquabox-server.service)
configure_file(${AQUABOX_ROOT_DIR}/config/aquabox-server/$ENV{BUILD_PLATFORM}/manifest.json ${CMAKE_BINARY_DIR}/manifest-server.json)