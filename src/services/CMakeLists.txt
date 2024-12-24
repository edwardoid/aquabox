project(services)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${AQUABOX_MICROSERVICES_HEADERS})
target_link_libraries(${PROJECT_NAME}   boost::boost
                                        nlohmann_json::nlohmann_json)