project(ab_ui)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
configure_file(${AQUABOX_ROOT_DIR}/config/aquabox/$ENV{BUILD_PLATFORM}/${PROJECT_NAME}.service ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}.service)
add_executable(${PROJECT_NAME} ${SOURCES} ${AQUABOX_MICROSERVICES_HEADERS})

find_package(libqrencode REQUIRED)

target_link_libraries (${PROJECT_NAME} aquabox-common micro-clients types types rpc fb libqrencode::libqrencode CONAN_PKG::freetype)
