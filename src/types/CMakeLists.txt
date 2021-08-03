project(types)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_link_libraries (${PROJECT_NAME} CONAN_PKG::nlohmann_json)