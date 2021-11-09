project(ab_network_manager)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
configure_file(${AQUABOX_ROOT_DIR}/config/aquabox/$ENV{BUILD_PLATFORM}/${PROJECT_NAME}.service ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}.service)
add_executable(${PROJECT_NAME} ${SOURCES} ${AQUABOX_MICROSERVICES_HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE ${NM_DBUS_INCLUDE_DIR})
target_link_libraries (${PROJECT_NAME} aquabox-common rpc types NMDBus)