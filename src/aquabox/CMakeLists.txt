project(aquabox)

MESSAGE(STATUS "AquaBox will be installed to ${CMAKE_INSTALL_PREFIX}")

if (TARGET_DEVICE)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SYSROOT}/aquabox")
    configure_file(${AQUABOX_ROOT_DIR}/config/aquabox/config.json ${CMAKE_BINARY_DIR}/config.json COPYONLY)
    configure_file(${AQUABOX_ROOT_DIR}/config/aquabox/startup.sh ${CMAKE_BINARY_DIR}/startup.sh COPYONLY)
    configure_file(${AQUABOX_ROOT_DIR}/config/aquabox/devices/dev1.json ${CMAKE_BINARY_DIR}/devices/dev1.json COPYONLY)
    configure_file(${AQUABOX_ROOT_DIR}/config/aquabox/devices/dev2.json ${CMAKE_BINARY_DIR}/devices/dev2.json COPYONLY)
    configure_file(${AQUABOX_ROOT_DIR}/config/aquabox/devices/dev3.json ${CMAKE_BINARY_DIR}/devices/dev3.json COPYONLY)
    configure_file(${AQUABOX_ROOT_DIR}/config/aquabox/devices/dev4.json ${CMAKE_BINARY_DIR}/devices/dev4.json COPYONLY)
    configure_file(${AQUABOX_ROOT_DIR}/config/aquabox/devices/temp0.json ${CMAKE_BINARY_DIR}/devices/temp0.json COPYONLY)
    configure_file(${AQUABOX_ROOT_DIR}/config/aquabox/devices/vdev0.json ${CMAKE_BINARY_DIR}/devices/vdev0.json COPYONLY)
    configure_file(${AQUABOX_ROOT_DIR}/config/aquabox/devices/vdev1.json ${CMAKE_BINARY_DIR}/devices/vdev1.json COPYONLY)
else()
    configure_file(${AQUABOX_ROOT_DIR}/config/aquabox/config.dev.json.in ${CMAKE_BINARY_DIR}/config.json)
    configure_file(${AQUABOX_ROOT_DIR}/config/aquabox/devices/vdev0.json ${CMAKE_BINARY_DIR}/devices/vdev0.json COPYONLY)
    configure_file(${AQUABOX_ROOT_DIR}/config/aquabox/devices/vdev1.json ${CMAKE_BINARY_DIR}/devices/vdev1.json COPYONLY)
endif()


file(COPY ${AQUABOX_ROOT_DIR}/config/aquabox/assets DESTINATION ${CMAKE_BINARY_DIR})

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/rules)

if (USE_NM)
    find_package (NetworkManager REQUIRED)
    link_directories(${NETWORKMANAGER_LIBRARY_DIRS})
    include_directories(${GLIB_INCLUDE_DIRS})
    include_directories(${NETWORKMANAGER_INCLUDE_DIRS})
endif()

include_directories(${AQUABOX_COMMON_PATH})
file(GLOB AQUABOX_SOURCES "*.cpp")

add_executable(${PROJECT_NAME} ${AQUABOX_SOURCES})
target_compile_definitions(${PROJECT_NAME} PUBLIC AB_VERSION_MAJOR=${AB_V})
target_compile_definitions(${PROJECT_NAME} PUBLIC AB_VERSION_MINOR=${AB_v})
target_compile_definitions(${PROJECT_NAME} PUBLIC AB_VERSION=\"${AB_V}.${AB_v}\")
target_compile_definitions(${PROJECT_NAME} PUBLIC AB_GIT_HASH=\"${GIT_COMMIT_HASH}\")

find_package(mqtt_cpp)
target_link_libraries(${PROJECT_NAME} qrencode)
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

install(TARGETS ${PROJECT_NAME} DESTINATION bin)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/install)
file(COPY ${AQUABOX_ROOT_DIR}/config/aquabox/assets DESTINATION ${CMAKE_BINARY_DIR})
