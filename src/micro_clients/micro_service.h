#pragma once

#include <common/log.h>
#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

namespace aquabox {
namespace services {
    template <class MicroServiceInterface>
    class MicroService {
    public:
        MicroService(std::uint32_t port)
            : MicroService(std::string("127.0.0.1:") + std::to_string(port))
        {
        }
        MicroService(std::string address)
            : MicroService(grpc::CreateChannel(address, grpc::InsecureChannelCredentials()))
        {
        }
        MicroService(std::shared_ptr<grpc::Channel> channel)
            : iface(MicroServiceInterface ::NewStub(channel))
        {
        }
        std::shared_ptr<class MicroServiceInterface::Stub> operator->()
        {
            return iface;
        }
        template <typename Function, typename... Args>
        grpc::Status call(Function fn, Args... args)
        {
            auto p = iface.get();
            grpc::ClientContext cxt;
            return (p->*fn)(&cxt, args...);
        }
        virtual ~MicroService() { }

    protected:
        std::shared_ptr<class MicroServiceInterface::Stub> iface;
    };
}
}
