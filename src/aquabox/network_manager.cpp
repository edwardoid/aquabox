#include "network_manager.h"
#include <NetworkManager.h>
#include "log.h"
#include <mutex>
#include <thread>

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
}

using namespace aquabox;

WifiNetwork getWifiNetworkInfo(NMAccessPoint* ap, bool& ok)
{
    ok = false;
    WifiNetwork wifi;
    GBytes *ssid = nm_access_point_get_ssid(ap);
    if (ssid == NULL) {
        return wifi;
    }
    wifi.ssid = std::string((const char *)g_bytes_get_data(ssid, NULL), g_bytes_get_size(ssid));
    wifi.signal = nm_access_point_get_strength(ap);

    auto flags = nm_access_point_get_flags(ap);
    auto wpa_flags = nm_access_point_get_wpa_flags(ap);
    auto rsn_flags = nm_access_point_get_rsn_flags(ap);

    wifi.encrypted = !(flags & NM_802_11_AP_FLAGS_PRIVACY) && (wpa_flags != NM_802_11_AP_SEC_NONE) && (rsn_flags != NM_802_11_AP_SEC_NONE);

    wifi.auth = Authentication::None;
    if ((flags & NM_802_11_AP_FLAGS_PRIVACY) && (wpa_flags == NM_802_11_AP_SEC_NONE) && (rsn_flags == NM_802_11_AP_SEC_NONE)) {
        wifi.auth = Authentication::WEP;
    }

    if (wpa_flags != NM_802_11_AP_SEC_NONE) {
        wifi.auth = Authentication::WPA;
    }

    if (rsn_flags != NM_802_11_AP_SEC_NONE) {
        wifi.auth = Authentication::WPA2;
    }

    if ((wpa_flags & NM_802_11_AP_SEC_KEY_MGMT_802_1X) || (rsn_flags & NM_802_11_AP_SEC_KEY_MGMT_802_1X)) {
        wifi.auth = Authentication::Enterprise;
    }
    ok = true;
    return wifi;
}

bool sameList(std::vector<WifiNetwork> left, std::vector<WifiNetwork> right)
{
    if (left.size() != right.size()) {
        return false;
    }

    for (auto l : left) {
        if (std::find(right.begin(), right.end(), l) == right.end()) {
            return false;
        }
    }
    return true;
}

std::vector<WifiNetwork> updateList(Data* data)
{
    std::vector<WifiNetwork> nets;
    NMDevice *dev = nm_client_get_device_by_iface(data->Client, data->Interface.c_str());
    if (dev == NULL) {
        LOG_ERROR << "Can't find device " << data->Interface;
        return nets;
    }

    if (!NM_IS_DEVICE_WIFI(dev)) {
        LOG_ERROR << "Interface " << data->Interface << " is not WiFi device";
        return nets;
    }

    const GPtrArray *aps = aps = nm_device_wifi_get_access_points(NM_DEVICE_WIFI(dev));
    
    for (int i = 0; i < aps->len; i++) {
        NMAccessPoint *ap = NM_ACCESS_POINT(g_ptr_array_index(aps, i));
        if (!ap) {
            continue;
        }

        bool ok = false;
        WifiNetwork wifi = getWifiNetworkInfo(ap, ok);
        if (ok) {
            nets.push_back(wifi);
        }
    }
    std::sort(nets.begin(), nets.end());
    
    if (!sameList(data->networks, nets)) {
        data->NetworksChanged->emit(nets.size());
    }

    data->networks = nets;
    return nets;
}

void request_rescan_cb (GObject *object, GAsyncResult *result, gpointer user_data)
{
	Data *data = (Data *) user_data;
	GError *error = NULL;

	nm_device_wifi_request_scan_finish (NM_DEVICE_WIFI (object), result, &error);
	if (error) {
        LOG_ERROR << error->message;
		g_error_free (error);
        data->requestScanResult = false;
	}
    else {
        data->requestScanResult = true;
    }

    updateList(data);
}

NetworkManager::NetworkManager()
    : m_data(new Data())
{
    m_data->Client = nm_client_new(NULL, NULL);
    m_data->NetworksChanged = & this->NetworksChanged;

    static std::thread loopThread([this]() {
        g_main_loop_run(g_main_loop_new(NULL, FALSE));
    });
    loopThread.detach();
}

NetworkManager::~NetworkManager()
{
    if (m_data->Client)
    {
        g_object_unref(m_data->Client);
    }

    delete m_data;
}

NetworkManager& NetworkManager::i()
{
    static NetworkManager instance;
    return instance;
}

void NetworkManager::setInterface(std::string interface)
{
    m_data->Interface = interface; 

    requestScan();
}

bool NetworkManager::requestScan(bool force)
{
    if (m_data->Client == NULL || m_data->Interface.empty()) {
        return false;
    }
    NMDevice *dev = nm_client_get_device_by_iface(m_data->Client, m_data->Interface.c_str());
    if (dev == NULL) {
        LOG_ERROR << "Can't find device " << m_data->Interface;
        return false;
    }

    if (!NM_IS_DEVICE_WIFI(dev)) {
        LOG_ERROR << "Interface " << m_data->Interface << " is not WiFi device";
        return false;
    }

    if (force) {
        std::string forceScanCommand = "iwlist ";
        forceScanCommand += m_data->Interface;
        forceScanCommand += " scan";
        system(forceScanCommand.c_str());
    }

    nm_device_wifi_request_scan_async(NM_DEVICE_WIFI(dev), NULL, request_rescan_cb, m_data);
    
    return m_data->requestScanResult;
}

void NetworkManager::updateCurrentNetwork()
{
    WifiNetwork network;
    NMDevice *device = nm_client_get_device_by_iface(m_data->Client, m_data->Interface.c_str());
    if (!NM_IS_DEVICE_WIFI(device)) {
        currentNetwork = network;
        return;
    }

    NMAccessPoint* ap = nm_device_wifi_get_active_access_point(NM_DEVICE_WIFI(device));
    if (ap == NULL) {
        currentNetwork = network;
        return;
    }
    bool ok;
    network = getWifiNetworkInfo(ap, ok);
    if (!ok) {
        currentNetwork = network;
        return;
    }

    NMActiveConnection* connection = nm_device_get_active_connection(device);

    if (connection == NULL) {
        currentNetwork = network;
        return;
    }
    
    NMRemoteConnection* remote = nm_active_connection_get_connection(connection);
    NMIPConfig* cfg = nm_active_connection_get_ip4_config(connection);
    if (cfg != NULL) {
        const GPtrArray *ips = nm_ip_config_get_addresses(cfg);
        for (int i = 0; i < ips->len; i++) {
            NMIPAddress *ip = (NMIPAddress*)(g_ptr_array_index(ips, i));
            if (ip != NULL) {
                network.ip = nm_ip_address_get_address(ip);
            }

            if (!network.ip.empty()) {
                break;
            }
        }
    }

    currentNetwork = network;
}

std::vector<WifiNetwork> NetworkManager::networks()
{
    return updateList(m_data);
}

bool NetworkManager::connect(WifiNetwork network)
{
    return false;
}