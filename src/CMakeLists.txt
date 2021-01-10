
set(AB_V 1)
set(AB_v 0)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(AQUABOX_COMMON_PATH ${CMAKE_CURRENT_LIST_DIR}/common)
set(AQUABOX_PROTO_PATH ${CMAKE_CURRENT_LIST_DIR}/proto/include)
add_subdirectory(common)
add_subdirectory(proto)
add_subdirectory(client)
add_subdirectory(server)

