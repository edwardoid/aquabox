project(types)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${AQUABOX_PROTO_PATH} ${NM_DBUS_INCLUDE_DIR} ${CMAKE_CURRENT_LIST_DIR}/..)

target_link_libraries (${PROJECT_NAME}  nlohmann_json::nlohmann_json
                                        boost::boost
                                        rpc
)