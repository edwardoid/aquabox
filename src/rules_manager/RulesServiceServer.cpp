#include "RulesServiceServer.h"
#include "../rpc/converters.h"
#include "../types/rule.h"

#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <sstream>

using namespace aquabox;
using namespace services;

RulesServiceServer::RulesServiceServer()
{
}

RulesServiceServer::~RulesServiceServer()
{}

::grpc::Status RulesServiceServer::GetAll(::grpc::ServerContext* /*context*/, const ::google::protobuf::Empty* /*request*/, ::grpc::ServerWriter< ::aquabox::services::RuleData>* writer)
{
    LOG_DEBUG << "Reqested all rules!";
    std::vector<Rule> data;
    data.resize(11);
    std::for_each(data.begin(), data.end(), [&writer](const auto& d) {
        writer->Write(Converter<Rule, RuleData>::convert(d));
    });
    return ::grpc::Status::OK;
}