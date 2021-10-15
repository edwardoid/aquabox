project(ab_devices_manager)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
                  "${CMAKE_CURRENT_LIST_DIR}/events/*.cpp"
                  "${CMAKE_CURRENT_LIST_DIR}/hermes/*.cpp"
                  "${CMAKE_CURRENT_LIST_DIR}/bus/*.cpp"
                  "${CMAKE_CURRENT_LIST_DIR}/layer/*.cpp"
                  "${CMAKE_CURRENT_LIST_DIR}/mqtt/*.cpp"
    )
add_executable(${PROJECT_NAME} ${SOURCES} ${AQUABOX_MICROSERVICES_HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/..)
target_link_libraries (${PROJECT_NAME} micro-clients aquabox-common aquabox-proto rpc types CONAN_PKG::mqtt_cpp rf24meshgw w1temp)