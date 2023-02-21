project(ab_client)

MESSAGE(STATUS "AquaBox will be installed to ${CMAKE_INSTALL_PREFIX}")

find_package(nlohmann_json_schema_validator REQUIRED)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/../data/rules)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/../data/rules)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/../data/rules)

configure_file(${AQUABOX_ROOT_DIR}/config/aquabox/$ENV{BUILD_PLATFORM}/config.dev.json.in ${CMAKE_BINARY_DIR}/bin/config.json)
configure_file(${AQUABOX_ROOT_DIR}/config/aquabox/$ENV{BUILD_PLATFORM}/${PROJECT_NAME}.service ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}.service)
configure_file(${AQUABOX_ROOT_DIR}/config/aquabox/$ENV{BUILD_PLATFORM}/manifest.json ${CMAKE_BINARY_DIR}/manifest-client.json)
file(COPY ${AQUABOX_ROOT_DIR}/config/aquabox/$ENV{BUILD_PLATFORM}/assets DESTINATION ${CMAKE_BINARY_DIR}/data)
file(COPY ${AQUABOX_ROOT_DIR}/config/aquabox/$ENV{BUILD_PLATFORM}/devices DESTINATION ${CMAKE_BINARY_DIR}/data)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/data/rules)

file(GLOB AQUABOX_SOURCES "*.cpp")

add_executable(${PROJECT_NAME} ${AQUABOX_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/.. ${NM_DBUS_INCLUDE_DIR})

target_compile_definitions(${PROJECT_NAME} PUBLIC AB_VERSION_MAJOR=${AB_V})
target_compile_definitions(${PROJECT_NAME} PUBLIC AB_VERSION_MINOR=${AB_v})
target_compile_definitions(${PROJECT_NAME} PUBLIC AB_VERSION=\"${AB_V}.${AB_v}\")
target_compile_definitions(${PROJECT_NAME} PUBLIC AB_GIT_HASH=\"${GIT_COMMIT_HASH}\")

find_package(libqrencode REQUIRED)

target_link_libraries(${PROJECT_NAME}   stats
                                        aquabox-common
                                        micro-clients
                                        rt
                                        dl
                                        rf24meshgw
                                        services
                                        types
                                        Threads::Threads
                                        CONAN_PKG::libqrencode
                                        CONAN_PKG::tgbot
                                        CONAN_PKG::json-schema-validator
                                        CONAN_PKG::boost
                                        CONAN_PKG::libpng
                                        CONAN_PKG::simppl
                                        CONAN_PKG::libserial
                                        CONAN_PKG::openssl
    )


set_target_properties(${PROJECT_NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)