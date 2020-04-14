#ifndef NETWORK_MANAGER_INTERNAL_H
#define NETWORK_MANAGER_INTERNAL_H

#include "network_manager.h"
#include <NetworkManager.h>
#include <mutex>
#include <chrono>
#include <condition_variable>

namespace aquabox
{
    struct Data
    {
        NMClient* Client;
        std::string Interface;  
        bool requestScanResult;
        std::vector<WifiNetwork> networks;
        SignalSlot::Signal<size_t>* NetworksChanged;
    };

    template<typename ResultType>
    struct Syncronizer
    {
        std::mutex mx;
        std::condition_variable cv;
        NMClient* Client;
        ResultType Result; 
        bool waitForResult(std::chrono::milliseconds timeout = std::chrono::milliseconds(0))
        {
            std::unique_lock<std::mutex> lk(mx);
            if (timeout.count() < 1) {
                cv.wait(lk);
            } else {
                return std::cv_status::no_timeout == cv.wait_for(lk, timeout);
            }

            return true;
        }

        void ready(ResultType result)
        {
            Result = result;
            cv.notify_all();
        }
    };

    using ActivateConnectionData = Syncronizer<bool>; 
    using AddConnectionData = Syncronizer<bool>;

    WifiNetwork GetWifiNetworksInfo(NMAccessPoint* ap, bool& ok);
    NMAccessPoint *GetAccessPoint(NMClient* client, std::string iface, std::string SSID);
    bool sameList(std::vector<WifiNetwork> left, std::vector<WifiNetwork> right);
    std::vector<WifiNetwork> GetAvailableWifiNetworks(Data* data);
    void RequestRescan (GObject *object, GAsyncResult *result, gpointer user_data);
    void OnAddedNewConnection(GObject *client, GAsyncResult *result, gpointer user_data);
    void OnConnectionActivated(GObject *client, GAsyncResult *result, gpointer user_data);
}

#endif // NETWORK_MANAGER_INTERNAL_H