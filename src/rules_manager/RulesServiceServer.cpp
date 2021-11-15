#include "RulesServiceServer.h"
#include <rpc/converters.h>
#include <micro_clients/devices_manager.h>
#include <micro_clients/events.h>
#include <types/rule.h>
#include "rules_map.h"
#include "rules_scheduler.h"

#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <sstream>

using namespace aquabox;
using namespace services;

RulesServiceServer::RulesServiceServer() {
    if(!init()) {
        LOG_ERROR << "Can't init Rules Manager";
    }
 }

RulesServiceServer::~RulesServiceServer() { }

bool RulesServiceServer::init() {
    boost::filesystem::path root = Singleton<ISettings>::i()->path("rules", "./rules");
    if (!boost::filesystem::exists(root)) {
        if (!boost::filesystem::create_directory(root)) {
            LOG_ERROR << "Can't create devices directory" << root;
            return false;
        }
    }

    if (!boost::filesystem::is_directory(root)) {
        LOG_ERROR << root << " is not a directory!";
        return false;
    }
    for(auto const& file: boost::filesystem::directory_iterator(root))
    {
        if (boost::filesystem::is_regular_file(file))
        {
            auto p = boost::filesystem::path(file);

            if (p.extension() != ".json") {
                continue;
            }

            LOG_INFO << "Loading rule: " << file;
            std::shared_ptr<Rule> rule = std::make_shared<Rule>();
            if (rule->parseFile(p)) {
                if (Singleton<IDevicesManager>::i()->has(rule->device())) {
                    Singleton<RulesMap>::i()->insert(rule->id(), rule);
                } else {
                    LOG_ERROR << "Can't find device " << rule->device() << " for rule " << rule->id();
                }
            }
        }
        else {
            LOG_ERROR << file << " is not a file!";
            return false;
        }
    }


    Singleton<RuleScheduler>::i()->recover();

    return true;
}

::grpc::Status RulesServiceServer::Add(::grpc::ServerContext* context, const ::aquabox::services::RuleData* request, ::google::protobuf::BoolValue* response)
{
    LOG_INFO << "Adding new rule";
    Rule src = Converter<RuleData, Rule>::convert(*request);
    if (!src.id().empty()) {
        std::shared_ptr<Rule> rule = std::make_shared<Rule>(src);


        if(Singleton<RulesMap>::i()->addRule(rule, false)) {
            return ::grpc::Status::OK;
        }

        return grpc::Status(::grpc::StatusCode::INTERNAL, "Can't add rule");
    }

    return grpc::Status(::grpc::StatusCode::INVALID_ARGUMENT, "Rule data is invalid");
}

::grpc::Status RulesServiceServer::Update(::grpc::ServerContext* context, const ::aquabox::services::RuleData* request, ::google::protobuf::BoolValue* response)
{
    LOG_INFO << "Updating rule";
    if (!Singleton<RulesMap>::i()->has(request->id())) {
        LOG_ERROR << "Rule not found!";
        return grpc::Status(grpc::StatusCode::NOT_FOUND, std::string("Not found rule") + request->id());
    }


    std::shared_ptr<Rule> rule = std::make_shared<Rule>(Converter<RuleData, Rule>::convert(*request));

    if(Singleton<RulesMap>::i()->addRule(rule, true)) {
        return ::grpc::Status::OK;
    }

    return grpc::Status(::grpc::StatusCode::INTERNAL, "Can't update rule");
}

::grpc::Status RulesServiceServer::Remove(::grpc::ServerContext* context, const ::google::protobuf::StringValue* request, ::google::protobuf::BoolValue* response)
{
    if (!Singleton<RulesMap>::i()->has(request->value())) {
        return grpc::Status(grpc::StatusCode::NOT_FOUND, std::string("Not found rule") + request->value());
    }

    if(Singleton<RulesMap>::i()->removeRule(request->value())) {
        return ::grpc::Status::OK;
    }

    return grpc::Status(::grpc::StatusCode::INTERNAL, "Can't delete rule");
}

::grpc::Status RulesServiceServer::GetAll(
    ::grpc::ServerContext* /*context*/,
    const ::google::protobuf::Empty* /*request*/,
    ::grpc::ServerWriter<::aquabox::services::RuleData>* writer)
{
    LOG_DEBUG << "Reqested all rules!";
    Singleton<RulesMap>::i()->for_each([this, writer](const auto id, const auto rule) {
        writer->Write(Converter<Rule, RuleData>::convert(*rule));
    });
    return ::grpc::Status::OK;
}

::grpc::Status RulesServiceServer::Get(::grpc::ServerContext* context, const ::google::protobuf::StringValue* request, ::aquabox::services::RuleData* response)
{
    LOG_DEBUG << "Requested rule " << request->value();
    bool found = Singleton<RulesMap>::i()->on(request->value(), [response](const auto& id, auto& dev) {
        *response = Converter<Rule, RuleData>::convert(*dev);
        return true;
    });

    if (found) {
        return ::grpc::Status::OK;
    }
    return grpc::Status(grpc::StatusCode::NOT_FOUND, std::string("Rule") + request->value() + "not found");
}