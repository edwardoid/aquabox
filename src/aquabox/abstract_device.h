#ifndef ABSTRACT_DEVCE_H
#define ABSTRACT_DEVICE_H


#include <string>
#include <s2s/s2s_property.h>
#include <memory>

namespace aquabox
{
    using ClusterPtr = std::shared_ptr<Cluster>;
    using AbstractDevicePtr = std::shared_ptr<AbstractDevice>;

    class Identifiable
    {
        std::string id;
    };

    class AbstractDevice: public Identifiable
    {
        std::string name;
        std::string type; 
        std::string address; // "cluster/id"
        ClusterPtr cluster;
        uint32_t capabilities;
        SignalSlot::Property<std::string> value;
    };

    class Cluster: public Identifiable
    {
        SignalSlot::Property<bool> online;
        std::vector<AbstractDevicePtr> devices();
    };
}