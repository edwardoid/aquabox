#include "network_manager.h"
#include <NetworkManager.h>
#include "log.h"
#include "network_manager_internal.h"
#include <mutex>
#include <thread>

using namespace aquabox;

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

    nm_device_wifi_request_scan_async(NM_DEVICE_WIFI(dev), NULL, RequestRescan, m_data);
    
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
    network = GetWifiNetworksInfo(ap, ok);
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
    return GetAvailableWifiNetworks(m_data);
}

bool NetworkManager::connect(const std::string& iface, WifiNetwork authData)
{
    int count = 0;
    NMDevice *device = nm_client_get_device_by_iface(m_data->Client, iface.c_str());
    if (!NM_IS_DEVICE_WIFI(device)) {
        return false;
    }

    GError *error = NULL;

    NMActiveConnection *activeConnection = nm_device_get_active_connection(device);
    if (activeConnection != NULL) {
        NMRemoteConnection *remote = nm_active_connection_get_connection(activeConnection);
        if (remote == NULL) {
            return false;
        }

        NMSettingWireless *s = nm_connection_get_setting_wireless(NM_CONNECTION(remote));
        if (s == NULL) {
            return false;
        }
    }

    NMAccessPoint *ap = GetAccessPoint(m_data->Client, iface, authData.ssid);

    if (ap == NULL) {
        LOG_ERROR << "Network not found";
        return false;
    }

    bool ok = true;
    WifiNetwork network = GetWifiNetworksInfo(ap, ok);
    if (!ok) {
        LOG_ERROR << "Can't get required info!";
        return false;
    }

    network.password = authData.password;


    LOG_DEBUG << "Network found, connecting...";

    NMConnection *connection = nm_simple_connection_new();

    NMSettingConnection *settingConnection = (NMSettingConnection *)nm_setting_connection_new();
    char *uuid = nm_utils_uuid_generate();
    g_object_set(G_OBJECT(settingConnection),
                 NM_SETTING_CONNECTION_UUID, uuid,
                 NM_SETTING_CONNECTION_ID, network.ssid.c_str(),
                 NM_SETTING_CONNECTION_TYPE, NM_SETTING_WIRELESS_SETTING_NAME,
                 NM_SETTING_CONNECTION_AUTOCONNECT, TRUE,
                 NULL);
    nm_connection_add_setting(connection, NM_SETTING(settingConnection));

    auto bssid = nm_access_point_get_bssid(ap);

    /* Build up the 'wired' Setting */
    NMSettingWireless *wireless = (NMSettingWireless *)nm_setting_wireless_new();
    g_object_set(G_OBJECT(wireless),
                 NM_SETTING_WIRELESS_SSID, nm_access_point_get_ssid(ap),
                 NM_SETTING_WIRELESS_BSSID, nm_access_point_get_bssid(ap),
                 NM_SETTING_WIRELESS_MODE, NM_SETTING_WIRELESS_MODE_INFRA,
                 NM_SETTING_WIRELESS_BAND, "bg",
                 NULL);

    nm_connection_add_setting(connection, NM_SETTING(wireless));

    if (network.auth == Authentication::Enterprise) {
        g_free(uuid);
        return false;
    }

    NMSettingWirelessSecurity *security = NULL;
    if (network.auth == Authentication::WEP)
    {
        security = (NMSettingWirelessSecurity *)nm_setting_wireless_security_new();
        g_object_set(G_OBJECT(security),
                     NM_SETTING_WIRELESS_SECURITY_KEY_MGMT, "none",
                     NM_SETTING_WIRELESS_SECURITY_WEP_TX_KEYIDX, 0,
                     NM_SETTING_WIRELESS_SECURITY_WEP_KEY_TYPE, 1,
                     NM_SETTING_WIRELESS_SECURITY_WEP_KEY0, network.password.c_str(),
                     NULL);
        if (network.password.size() == 32) {
            g_object_set(G_OBJECT(security),
                         NM_SETTING_WIRELESS_SECURITY_WEP_KEY_TYPE, 2,
                         NULL);
        }
    }
    else if (network.auth == Authentication::WPA || network.auth == Authentication::WPA2) {
        security = (NMSettingWirelessSecurity *)nm_setting_wireless_security_new();
        g_object_set(G_OBJECT(security),
                     NM_SETTING_WIRELESS_SECURITY_KEY_MGMT, "wpa-psk",
                     NM_SETTING_WIRELESS_SECURITY_PSK, network.password.c_str(),
                     NULL);
    }

    if (security != NULL) {
        nm_connection_add_setting(connection, NM_SETTING(security));
    }

    NMSettingIP4Config *ip4 = (NMSettingIP4Config *)nm_setting_ip4_config_new();
    g_object_set(G_OBJECT(ip4),
                 NM_SETTING_IP_CONFIG_METHOD, NM_SETTING_IP4_CONFIG_METHOD_AUTO,
                 NULL);
    nm_connection_add_setting(connection, NM_SETTING(ip4));

    if (nm_connection_verify(connection, &error) == FALSE) {
        LOG_ERROR << "Verification failed: " << error->message;
        g_free(uuid);
        return false;
    }

    if (nm_access_point_connection_valid(ap, connection) == FALSE) {
        LOG_ERROR << "Access point is not valid and can't be used";
        g_free(uuid);
        return false;
    }

    AddConnectionData addOptions;
    addOptions.Client = m_data->Client;

    nm_client_add_connection_async(m_data->Client, connection, true, NULL, OnAddedNewConnection, &addOptions);

    // TODO: Wait for activation
    addOptions.waitForResult();

    if (addOptions.Result) {
        ActivateConnectionData activateOptions;
        if(!activateConnection(uuid)) {
            LOG_ERROR << "Activation failed!";
            addOptions.Result = false;
        }
    } else {
        LOG_INFO << "Created connection: " << uuid;
    }

    g_free(uuid);
    g_object_unref(connection);
    return addOptions.Result;
}

bool NetworkManager::activateConnection(std::string uuid, bool async)
{
    NMRemoteConnection *conn = nm_client_get_connection_by_uuid(m_data->Client, uuid.c_str());
    if (conn == NULL) {
        return false;
    }

    ActivateConnectionData data;
    data.Client = m_data->Client;
    data.Result = async;
    if (async) {
        nm_client_activate_connection_async(m_data->Client, NM_CONNECTION(conn),
                                        NULL, NULL, NULL, NULL, NULL);
    } else {
        nm_client_activate_connection_async(m_data->Client, NM_CONNECTION(conn),
                                            NULL, NULL, NULL,
                                            OnConnectionActivated, &data);
        data.waitForResult();
    }

    return data.Result;
}
