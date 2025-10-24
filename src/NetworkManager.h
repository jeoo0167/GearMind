#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <esp_now.h>
#include <WiFi.h>
#include <Arduino.h>
#include "DebugMsg.h"

class NetworkManager
{
    public:
        inline static NetworkManager& getInstance()
        {
            static NetworkManager instance;
            return instance;
        }
        NetworkManager() : debug_msgs("NetworkManager.cpp") {};
        void Begin(const uint8_t receiverMac[]);
        void Send(const char *data,unsigned long data_delay);
    private:
        typedef struct
        {
            char cmd[32];
        } CommandMessage;
        
        CommandMessage msg;
        uint8_t mac_addr[6];
        NetworkManager(const NetworkManager&) = delete;
        NetworkManager& operator=(const NetworkManager&) = delete;
        DebugMsgs debug_msgs;
        unsigned long lastTime = 0;
        int counter=0;
};

#endif