project(aquabox-common)

file(GLOB AQUABOX_COMMON_SOURCES "*.cpp")

add_definitions(-DMQTT_USE_WS)
add_library(${PROJECT_NAME} SHARED ${AQUABOX_COMMON_SOURCES})
target_link_libraries(${PROJECT_NAME}   
                                        rpc
                                        micro-clients
                                        nlohmann_json::nlohmann_json
                                        nlohmann_json_schema_validator
                                        boost::boost
                                        openssl::openssl
                                        mqtt_cpp::mqtt_cpp
    )

set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE 1)
