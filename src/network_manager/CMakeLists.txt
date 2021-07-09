project(ab_network-manager)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES} ${AQUABOX_MICROSERVICES_HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE ${NM_DBUS_INCLUDE_DIR})
target_link_libraries (${PROJECT_NAME} ServiceInterfaces NMDBus)