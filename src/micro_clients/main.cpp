#include <iostream>

#include "micro_service.h"
#include <grpcpp/client_context.h>
#include <iostream>
#include <network.grpc.pb.h>
/*
int main()
{
    aquabox::services::MicroService<aquabox::services::NetworkManager>
client("123"); grpc::ClientContext cxt;
    ::google::protobuf::Empty request;
    ::google::protobuf::BoolValue response;
    auto status = client->IsConnectedToInternet(&cxt, request, &response);
    std::cout << "Connected? " << response.value() << " status: " <<
status.error_message(); return 0;
}
*/