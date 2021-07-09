project(aquabox-common)

file(GLOB AQUABOX_COMMON_SOURCES "*.cpp")

add_library(${PROJECT_NAME} SHARED ${AQUABOX_COMMON_SOURCES})
target_link_libraries(${PROJECT_NAME} CONAN_PKG::nlohmann_json)
target_link_libraries(${PROJECT_NAME} CONAN_PKG::json-schema-validator)
target_link_libraries(${PROJECT_NAME} CONAN_PKG::croncpp)
target_link_libraries(${PROJECT_NAME} CONAN_PKG::boost)
target_link_libraries(${PROJECT_NAME} CONAN_PKG::openssl)
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE 1)
