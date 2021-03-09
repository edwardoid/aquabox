project(mock_hardware)

include_directories(${AQUABOX_PROTO_PATH})

file(GLOB SOCKET_FEEDER_SOURCES "socket_feeder.cpp")

add_executable("socket_feeder" ${SOCKET_FEEDER_SOURCES})

target_link_libraries(socket_feeder aquabox-proto)
target_link_libraries(socket_feeder Threads::Threads)

set_target_properties(socket_feeder
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
