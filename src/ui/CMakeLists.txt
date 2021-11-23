project(ab_ui)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
configure_file(${AQUABOX_ROOT_DIR}/config/aquabox/$ENV{BUILD_PLATFORM}/${PROJECT_NAME}.service ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}.service)
add_executable(${PROJECT_NAME} ${SOURCES} ${AQUABOX_MICROSERVICES_HEADERS})

find_package(libqrencode REQUIRED)
find_package(PNG REQUIRED)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_link_libraries (${PROJECT_NAME} aquabox-common
                                        micro-clients
                                        types
                                        rpc
                                        fb
                                        libqrencode::libqrencode
                                        CONAN_PKG::freetype
                                        PNG::PNG)
