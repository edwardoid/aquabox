#include "network_manager_device_state.h"
#include <common/enumerations.h>

using namespace aquabox;

template<>
Serializable<NetworkDeviceState>::Enum2StrMap Serializable<NetworkDeviceState>::val2Str(
    {{NetworkDeviceState::NM_DEVICE_STATE_UNKNOWN, "unknown"},
     {NetworkDeviceState::NM_DEVICE_STATE_UNMANAGED, "unmanaged"},
     {NetworkDeviceState::NM_DEVICE_STATE_UNAVAILABLE, "unavailable"},
     {NetworkDeviceState::NM_DEVICE_STATE_DISCONNECTED, "disconnected"},
     {NetworkDeviceState::NM_DEVICE_STATE_PREPARE, "prepare"},
     {NetworkDeviceState::NM_DEVICE_STATE_CONFIG, "config"},
     {NetworkDeviceState::NM_DEVICE_STATE_NEED_AUTH, "need_auth"},
     {NetworkDeviceState::NM_DEVICE_STATE_IP_CONFIG, "ip_config"},
     {NetworkDeviceState::NM_DEVICE_STATE_IP_CHECK, "ip_check"},
     {NetworkDeviceState::NM_DEVICE_STATE_SECONDARIES, "secondaries"},
     {NetworkDeviceState::NM_DEVICE_STATE_ACTIVATED, "activated"},
     {NetworkDeviceState::NM_DEVICE_STATE_DEACTIVATING, "deactivating"},
     {NetworkDeviceState::NM_DEVICE_STATE_FAILED, "failed"}});

template<>
Serializable<NetworkDeviceState>::Str2EnumMap Serializable<NetworkDeviceState>::str2Val({
    {"unknown", NetworkDeviceState::NM_DEVICE_STATE_UNKNOWN},
    {"unmanaged", NetworkDeviceState::NM_DEVICE_STATE_UNMANAGED},
    {"unavailable", NetworkDeviceState::NM_DEVICE_STATE_UNAVAILABLE},
    {"disconnected", NetworkDeviceState::NM_DEVICE_STATE_DISCONNECTED},
    {"prepare", NetworkDeviceState::NM_DEVICE_STATE_PREPARE},
    {"config", NetworkDeviceState::NM_DEVICE_STATE_CONFIG},
    {"need_auth", NetworkDeviceState::NM_DEVICE_STATE_NEED_AUTH},
    {"ip_config", NetworkDeviceState::NM_DEVICE_STATE_IP_CONFIG},
    {"ip_check", NetworkDeviceState::NM_DEVICE_STATE_IP_CHECK},
    {"secondaries", NetworkDeviceState::NM_DEVICE_STATE_SECONDARIES},
    {"activated", NetworkDeviceState::NM_DEVICE_STATE_ACTIVATED},
    {"deactivating", NetworkDeviceState::NM_DEVICE_STATE_DEACTIVATING},
    {"failed", NetworkDeviceState::NM_DEVICE_STATE_FAILED},
});