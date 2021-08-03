project(aquabox-client)

MESSAGE(STATUS "AquaBox will be installed to ${CMAKE_INSTALL_PREFIX}")

find_package(nlohmann_json_schema_validator REQUIRED)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/rules)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/devices)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/assets)

configure_file(${AQUABOX_ROOT_DIR}/config/aquabox/$ENV{BUILD_PLATFORM}/config.dev.json.in ${CMAKE_BINARY_DIR}/bin/config.json)
configure_file(${AQUABOX_ROOT_DIR}/config/aquabox/$ENV{BUILD_PLATFORM}/aquabox.service ${CMAKE_BINARY_DIR}/bin/aquabox-client.service)
configure_file(${AQUABOX_ROOT_DIR}/config/aquabox/$ENV{BUILD_PLATFORM}/manifest.json ${CMAKE_BINARY_DIR}/manifest-client.json)
file(COPY ${AQUABOX_ROOT_DIR}/config/aquabox/$ENV{BUILD_PLATFORM}/assets DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${AQUABOX_ROOT_DIR}/config/aquabox/$ENV{BUILD_PLATFORM}/devices DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/rules)

include_directories(${NM_DBUS_INCLUDE_DIR})
include_directories(${AQUABOX_COMMON_PATH})
include_directories(${AQUABOX_PROTO_PATH})

file(GLOB AQUABOX_SOURCES "*.cpp" "*/*.cpp" "*/hermes/*.cpp")

add_executable(${PROJECT_NAME} ${AQUABOX_SOURCES})
target_compile_definitions(${PROJECT_NAME} PUBLIC AB_VERSION_MAJOR=${AB_V})
target_compile_definitions(${PROJECT_NAME} PUBLIC AB_VERSION_MINOR=${AB_v})
target_compile_definitions(${PROJECT_NAME} PUBLIC AB_VERSION=\"${AB_V}.${AB_v}\")
target_compile_definitions(${PROJECT_NAME} PUBLIC AB_GIT_HASH=\"${GIT_COMMIT_HASH}\")

find_package(mqtt_cpp REQUIRED)
find_package(libqrencode REQUIRED)

target_link_libraries(${PROJECT_NAME} NMDBus fb w1temp stats)
target_link_libraries(${PROJECT_NAME} aquabox-common)
target_link_libraries(${PROJECT_NAME} aquabox-proto)
target_link_libraries(${PROJECT_NAME} CONAN_PKG::tgbot)
target_link_libraries(${PROJECT_NAME} libqrencode::libqrencode)
target_link_libraries(${PROJECT_NAME} CONAN_PKG::mqtt_cpp)
target_link_libraries(${PROJECT_NAME} CONAN_PKG::json-schema-validator)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
target_link_libraries(${PROJECT_NAME} OpenSSL::OpenSSL)
target_link_libraries(${PROJECT_NAME} PNG::PNG)
target_link_libraries(${PROJECT_NAME} CONAN_PKG::freetype)
target_link_libraries(${PROJECT_NAME} Threads::Threads)
target_link_libraries(${PROJECT_NAME} CONAN_PKG::simppl)
target_link_libraries(${PROJECT_NAME} CONAN_PKG::libserial)
target_link_libraries(${PROJECT_NAME} dl)
target_link_libraries(${PROJECT_NAME} rt)
target_link_libraries(${PROJECT_NAME} rf24meshgw)
target_link_libraries(${PROJECT_NAME} types)


set_target_properties(${PROJECT_NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "AquaBoxClient")
set(CPACK_PACKAGE_VENDOR "aquabox.me")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Client Service for aquabox")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Eduard Sargsyan") # required
set(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE 1)
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")


include(CPack)


#install(TARGETS ${PROJECT_NAME} DESTINATION bin)