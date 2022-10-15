project(aquabox-common)

file(GLOB AQUABOX_COMMON_SOURCES "*.cpp")

add_definitions(-DMQTT_USE_WS)
add_library(${PROJECT_NAME} SHARED ${AQUABOX_COMMON_SOURCES})
target_link_libraries(${PROJECT_NAME}   
                                        rpc
                                        micro-clients
                                        CONAN_PKG::nlohmann_json
                                        CONAN_PKG::json-schema-validator
                                        CONAN_PKG::boost
                                        CONAN_PKG::openssl
                                        CONAN_PKG::mqtt_cpp
    )

set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE 1)
