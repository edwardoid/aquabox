project(ab_rules_manager)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
configure_file(${AQUABOX_ROOT_DIR}/config/aquabox/$ENV{BUILD_PLATFORM}/${PROJECT_NAME}.service ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}.service)
add_executable(${PROJECT_NAME} ${SOURCES} ${AQUABOX_MICROSERVICES_HEADERS})

target_link_libraries (${PROJECT_NAME}
    micro-clients
    types
    rpc
    croncpp::croncpp
    nlohmann_json::nlohmann_json
)
