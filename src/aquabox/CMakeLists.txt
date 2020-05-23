project(aquabox)

MESSAGE(STATUS "AquaBox will be installed to ${CMAKE_INSTALL_PREFIX}")


configure_file(${AQUABOX_ROOT_DIR}/config/aquabox/config.dev.json.in ${CMAKE_BINARY_DIR}/config.json)
configure_file(${AQUABOX_ROOT_DIR}/config/aquabox/devices/vdev0.json ${CMAKE_BINARY_DIR}/devices/vdev0.json COPYONLY)
configure_file(${AQUABOX_ROOT_DIR}/config/aquabox/devices/vdev1.json ${CMAKE_BINARY_DIR}/devices/vdev1.json COPYONLY)


file(COPY ${AQUABOX_ROOT_DIR}/config/aquabox/assets DESTINATION ${CMAKE_BINARY_DIR})

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/rules)

include_directories(${NM_DBUS_INCLUDE_DIR})
include_directories(${AQUABOX_COMMON_PATH})
file(GLOB AQUABOX_SOURCES "*.cpp")

add_executable(${PROJECT_NAME} ${AQUABOX_SOURCES})
target_compile_definitions(${PROJECT_NAME} PUBLIC AB_VERSION_MAJOR=${AB_V})
target_compile_definitions(${PROJECT_NAME} PUBLIC AB_VERSION_MINOR=${AB_v})
target_compile_definitions(${PROJECT_NAME} PUBLIC AB_VERSION=\"${AB_V}.${AB_v}\")
target_compile_definitions(${PROJECT_NAME} PUBLIC AB_GIT_HASH=\"${GIT_COMMIT_HASH}\")

find_package(mqtt_cpp REQUIRED)

target_link_libraries(${PROJECT_NAME} qrencode NMDBus)
target_link_libraries(${PROJECT_NAME} fb_static)
target_link_libraries(${PROJECT_NAME} w1temp_static)
target_link_libraries(${PROJECT_NAME} aquabox-common_static)
target_link_libraries(${PROJECT_NAME} nlohmann_json_schema_validator)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
target_link_libraries(${PROJECT_NAME} OpenSSL::OpenSSL)
target_link_libraries(${PROJECT_NAME} PNG::PNG)
target_link_libraries(${PROJECT_NAME} ZLIB::ZLIB)
target_link_libraries(${PROJECT_NAME} freetype::freetype)
target_link_libraries(${PROJECT_NAME} Threads::Threads)
target_link_libraries(${PROJECT_NAME} dbus::dbus)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/install)
file(COPY ${AQUABOX_ROOT_DIR}/config/aquabox/assets DESTINATION ${CMAKE_BINARY_DIR})
