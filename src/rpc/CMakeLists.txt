project(rpc)


find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)
find_package(Threads)

file(GLOB PROTOS "${CMAKE_CURRENT_LIST_DIR}/*.proto")
file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(${PROJECT_NAME} SHARED ${AB_MS_SRCS} ${AB_MS_HDRS} ${PROTOS} ${SOURCES})
protobuf_generate(TARGET ${PROJECT_NAME} LANGUAGE cpp)
protobuf_generate(TARGET ${PROJECT_NAME} LANGUAGE grpc GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc PLUGIN "protoc-gen-grpc=/usr/local/bin/grpc_cpp_plugin")

protobuf_generate(TARGET ${PROJECT_NAME} LANGUAGE python)
protobuf_generate(TARGET ${PROJECT_NAME} LANGUAGE grpc GENERATE_EXTENSIONS .grpc.pb.py PLUGIN "protoc-gen-grpc=/usr/local/bin/grpc_python_plugin")

set(AQUABOX_MICROSERVICES_HEADERS ${CMAKE_CURRENT_BINARY_DIR}
    CACHE INTERNAL "Path to generated protobuf files.")

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        protobuf::libprotobuf
        CONAN_PKG::nlohmann_json
        CONAN_PKG::boost
        gRPC::grpc
        gRPC::grpc++
)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

#
# Compile protobuf and grpc files in myproto target to cpp
#

