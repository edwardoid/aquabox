#ifndef COMMAND_EXECUTOR_H
#define COMMAND_EXECUTOR_H

#include "command.h"

namespace aquabox
{
    class CommandExecutor
    {
    public:
        bool exec(const Command& cmd);
    private:
        bool GetStatus();
        bool GetDevices();
        bool GetDevice();
        bool UpdateDevice();
        bool GetRules();
        bool GetRule();
        bool CreateOrUpdateRule(bool update);
        bool DeleteRule();
        bool DeviceRules();
        bool SupportedRepeatTimeouts();
        bool TurnOn();
        bool TurnOff();
        bool ScanWiFiNetworks();
        bool ConnectToWiFi();
    private:
        Command m_cmd;
    };
}

#endif // COMMAND_EXECUTOR_H