project(aquabox)

MESSAGE(STATUS "AquaBox will be installed to ${CMAKE_INSTALL_PREFIX}")


file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/rules)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/devices)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/assets)

configure_file(${AQUABOX_ROOT_DIR}/config/aquabox/$ENV{BUILD_PLATFORM}/config.dev.json.in ${CMAKE_BINARY_DIR}/config.json)
configure_file(${AQUABOX_ROOT_DIR}/config/aquabox/$ENV{BUILD_PLATFORM}/startup.sh ${CMAKE_BINARY_DIR}/startup.sh)
configure_file(${AQUABOX_ROOT_DIR}/config/aquabox/$ENV{BUILD_PLATFORM}/aquabox.service ${CMAKE_BINARY_DIR}/aquabox.service)
file(COPY ${AQUABOX_ROOT_DIR}/config/aquabox/$ENV{BUILD_PLATFORM}/assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${AQUABOX_ROOT_DIR}/config/aquabox/$ENV{BUILD_PLATFORM}/devices DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${AQUABOX_ROOT_DIR}/config/aquabox/$ENV{BUILD_PLATFORM}/rules DESTINATION ${CMAKE_BINARY_DIR})

include_directories(${NM_DBUS_INCLUDE_DIR})
include_directories(${AQUABOX_COMMON_PATH})
file(GLOB AQUABOX_SOURCES "*.cpp")

add_executable(${PROJECT_NAME} ${AQUABOX_SOURCES})
target_compile_definitions(${PROJECT_NAME} PUBLIC AB_VERSION_MAJOR=${AB_V})
target_compile_definitions(${PROJECT_NAME} PUBLIC AB_VERSION_MINOR=${AB_v})
target_compile_definitions(${PROJECT_NAME} PUBLIC AB_VERSION=\"${AB_V}.${AB_v}\")
target_compile_definitions(${PROJECT_NAME} PUBLIC AB_GIT_HASH=\"${GIT_COMMIT_HASH}\")

find_package(mqtt_cpp REQUIRED)

target_link_libraries(${PROJECT_NAME} qrencode NMDBus fb w1temp)
target_link_libraries(${PROJECT_NAME} aquabox-common)
target_link_libraries(${PROJECT_NAME} JsonSchemaValidator::JsonSchemaValidator)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
target_link_libraries(${PROJECT_NAME} OpenSSL::OpenSSL)
target_link_libraries(${PROJECT_NAME} PNG::PNG)
target_link_libraries(${PROJECT_NAME} ZLIB::ZLIB)
target_link_libraries(${PROJECT_NAME} freetype::freetype)
target_link_libraries(${PROJECT_NAME} Threads::Threads)
target_link_libraries(${PROJECT_NAME} dbus::dbus)
target_link_libraries(${PROJECT_NAME} dl)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
