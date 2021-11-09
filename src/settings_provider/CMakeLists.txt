project(ab_settings_provider)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
configure_file(${AQUABOX_ROOT_DIR}/config/aquabox/$ENV{BUILD_PLATFORM}/${PROJECT_NAME}.service ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}.service)
add_executable(${PROJECT_NAME} ${SOURCES} ${AQUABOX_MICROSERVICES_HEADERS})
target_link_libraries (${PROJECT_NAME} rpc types aquabox-common)

install(TARGETS ${PROJECT_NAME}
        COMPONENT ${PROJECT_NAME}
        RUNTIME DESTINATION bin
        EXCLUDE_FROM_ALL)

include(CPack)
