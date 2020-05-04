#include "network_manager_internal.h"
#include "log.h"
#include <algorithm>
#include <thread>

namespace aquabox {

#ifdef USE_NM

WifiNetwork GetWifiNetworksInfo(NMAccessPoint* ap, bool& ok)
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

std::vector<WifiNetwork> GetAvailableWifiNetworks (Data* data)
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
        WifiNetwork wifi = GetWifiNetworksInfo(ap, ok);
        if (ok) {
            auto it = std::find(nets.begin(), nets.end(), wifi);
            if (it != nets.end()) {
                it->signal = std::max(it->signal, wifi.signal);
            } else {
                nets.push_back(wifi);
            }
        }
    }
    std::sort(nets.begin(), nets.end());
    
    if (!sameList(data->networks, nets)) {
        data->NetworksChanged->emit(nets.size());
    }

    data->networks = nets;
    return nets;
}

void RequestRescan (GObject *object, GAsyncResult *result, gpointer user_data)
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

    GetAvailableWifiNetworks(data);
}

NMAccessPoint* GetAccessPoint(NMClient* client, std::string iface, std::string SSID)
{
    NMDevice *dev = nm_client_get_device_by_iface(client, iface.c_str());
    if (dev == NULL) {
        LOG_ERROR << "Can't find device " << iface;
        return NULL;
    }

    if (!NM_IS_DEVICE_WIFI(dev)) {
        LOG_ERROR << "Interface " << iface << " is not WiFi device";
        return NULL;
    }

    GError *err = NULL;
    if (!nm_device_wifi_request_scan(NM_DEVICE_WIFI(dev), NULL, &err) && err->code != 6) {
        LOG_ERROR << "Can't perform scan Error:" << err->code << " " << err->message;
        g_error_free(err);
        return NULL;
    }

    if (err == NULL) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
    } else {
        g_error_free(err);
    }

    const GPtrArray *aps = nm_device_wifi_get_access_points(NM_DEVICE_WIFI(dev));

    for (int i = 0; i < aps->len; i++) {
        NMAccessPoint *ap = NM_ACCESS_POINT(g_ptr_array_index(aps, i));
        if (!ap) {
            continue;
        }

        GBytes *ssid = nm_access_point_get_ssid(ap);
        if (ssid == NULL) {
            continue;
        }
        if (SSID == std::string((const char *)g_bytes_get_data(ssid, NULL), g_bytes_get_size(ssid))) {
            return ap;
        }
    }
    return NULL;
} 

void OnAddedNewConnection(GObject *client, GAsyncResult *result, gpointer user_data)
{
    AddConnectionData *data = (AddConnectionData *)user_data;
    GError *error = NULL;

    NMRemoteConnection* remote = nm_client_add_connection_finish(NM_CLIENT(data->Client), result, &error);

    bool succeeded = error == nullptr;
    if (error) {
        LOG_ERROR << "Error adding connection:" << error->message;
        g_error_free(error);
    } else {
        LOG_DEBUG << "Added: " << nm_connection_get_path(NM_CONNECTION(remote));
    }
    data->ready(succeeded);
}

void OnConnectionActivated(GObject *client, GAsyncResult *result, gpointer user_data)
{
    ActivateConnectionData *data = (ActivateConnectionData *)user_data;
    GError *error = NULL;

    NMActiveConnection* active = nm_client_activate_connection_finish(NM_CLIENT(client), result, &error);

    bool succeeded = error == nullptr;
    if (error) {
        LOG_ERROR << "Error activating connection: " << error->message;
        g_error_free(error);
    } else {
        NMRemoteConnection* remote = nm_active_connection_get_connection(active);
        LOG_DEBUG << "Activated: " << nm_connection_get_path(NM_CONNECTION(remote));
    }
    data->ready(succeeded);
}

#endif // USE_NM

} // namespace aquabox