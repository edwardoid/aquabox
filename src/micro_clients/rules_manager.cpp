/*
 * AquaBox
 * Copyright (C) 2019  Eduard Sargsyan
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "rules_manager.h"
#include "micro_service.h"
#include "services.h"
#include <rpc/converters.h>
#include <rules.grpc.pb.h>
#include <services/service_identifier.h>
#include <services/settings.h>

using namespace aquabox;

class RulesManagerImpl : public IRulesManager
{
public:
    RulesManagerImpl();
    virtual ~RulesManagerImpl() {}
    virtual Rule get(const std::string& id) override;
    virtual bool add(const Rule& rule) override;
    virtual bool update(const Rule& rule) override;
    virtual bool remove(const std::string& id) override;
    virtual std::vector<Rule> all() override;
};

template<>
IRulesManager* Singleton<IRulesManager>::create()
{
    return new RulesManagerImpl();
}

namespace
{
    services::MicroService<services::RulesService>& client()
    {
        static services::MicroService<services::RulesService> c(
            Singleton<ISettings>::i()->port(services::RulesService::service_full_name()));
        return c;
    }
} // namespace

RulesManagerImpl::RulesManagerImpl() {}

Rule RulesManagerImpl::get(const std::string& id)
{
    google::protobuf::StringValue _id;
    _id.set_value(id);
    services::RuleData data;

    if (client().call(&services::RulesService::Stub::Get, _id, &data).ok()) {
        return services::Converter<services::RuleData, Rule>::convert(data);
    }

    return Rule();
}

bool RulesManagerImpl::add(const Rule& rule)
{
    google::protobuf::BoolValue res;
    services::RuleData data = services::Converter<Rule, services::RuleData>::convert(rule);
    const grpc::Status ret = client().call(&services::RulesService::Stub::Add, data, &res);
    if (ret.ok()) {
        return true;
    }

    LOG_ERROR << ret.error_message() << ": " << ret.error_details();

    return false;
}

bool RulesManagerImpl::update(const Rule& rule)
{
    google::protobuf::BoolValue res;
    services::RuleData data = services::Converter<Rule, services::RuleData>::convert(rule);
    const grpc::Status ret = client().call(&services::RulesService::Stub::Update, data, &res);
    if (ret.ok()) {
        return true;
    }

    LOG_ERROR << ret.error_message() << ": " << ret.error_details();

    return false;
}

bool RulesManagerImpl::remove(const std::string& id)
{
    google::protobuf::StringValue _id;
    google::protobuf::BoolValue res;
    _id.set_value(id);
    if (client().call(&services::RulesService::Stub::Remove, _id, &res).ok()) {
        return true;
    }
    return false;
}

std::vector<Rule> RulesManagerImpl::all()
{
    grpc::ClientContext cxt;
    auto c = client();
    std::vector<Rule> res;
    google::protobuf::Empty e;
    std::unique_ptr<grpc::ClientReader<services::RuleData>> reader(c->GetAll(&cxt, e));
    services::RuleData data;
    while (reader->Read(&data)) {
        res.push_back(services::Converter<services::RuleData, Rule>::convert(data));
    }
    grpc::Status status = reader->Finish();
    return res;
}
