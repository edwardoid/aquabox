project(ab_settings-provider)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES} ${AQUABOX_MICROSERVICES_HEADERS})
target_link_libraries (${PROJECT_NAME} rpc types)