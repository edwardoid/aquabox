project(ab_devices_manager)

add_definitions(-DMQTT_USE_WS)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
                  "${CMAKE_CURRENT_LIST_DIR}/events/*.cpp"
                  "${CMAKE_CURRENT_LIST_DIR}/hermes/*.cpp"
                  "${CMAKE_CURRENT_LIST_DIR}/bus/*.cpp"
                  "${CMAKE_CURRENT_LIST_DIR}/layer/*.cpp"
                  "${CMAKE_CURRENT_LIST_DIR}/mqtt/*.cpp"
    )

configure_file(${AQUABOX_ROOT_DIR}/config/aquabox/$ENV{BUILD_PLATFORM}/${PROJECT_NAME}.service ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}.service)
add_executable(${PROJECT_NAME} ${SOURCES} ${AQUABOX_MICROSERVICES_HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/..)

target_link_libraries (${PROJECT_NAME}
        micro-clients
        aquabox-common
        aquabox-proto
        rpc
        types
        rf24meshgw
        w1temp
        mqtt_cpp::mqtt_cpp
    )

cpack_add_component(${PROJECT_NAME}
                         DISPLAY_NAME "${PROJECT_NAME}")

