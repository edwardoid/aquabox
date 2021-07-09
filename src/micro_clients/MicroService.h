#pragma once


#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

namespace aquabox
{
    namespace services
    {
        template<class MicroServiceInterface>
        class MicroService
        {
        public:
            MicroService()
            : MicroService( grpc::CreateChannel("localhost:50051",
                            grpc::InsecureChannelCredentials()))
             {}
            MicroService(std::shared_ptr<grpc::Channel> channel)
                : iface(MicroServiceInterface :: NewStub(channel))
            {}
            std::shared_ptr<class MicroServiceInterface::Stub> operator-> () {
                return iface;
            }
            virtual ~MicroService() {}
        protected:
            std::shared_ptr<class MicroServiceInterface::Stub> iface;
        };
    }
}
