#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <esp_now.h>
#include <WiFi.h>
#include <Arduino.h>
#include "DebugMsg.h"

enum msg_type : uint8_t
{
    DATA,
    COMMAND,
    HEARTBEAT
};

typedef struct
{
    char cmd[32];
    msg_type type;
} esp_cmd_t;

class NetworkManager
{
public:
    inline static NetworkManager &getInstance()
    {
        static NetworkManager instance;
        return instance;
    }

    NetworkManager();

    void Begin(const uint8_t receiverMac[]);
    void Send(const esp_cmd_t *packet, unsigned long data_delay = 50);
    
    esp_cmd_t CreatePacket(const char *cmd, msg_type type);

    static bool connected;
    unsigned long lastHeartbeat = 0;

private:
    uint8_t mac_addr[6] = {0};

    QueueHandle_t msgQueue = nullptr;
    esp_cmd_t msg;
    
    DebugMsgs debug_msgs;
    unsigned long lastTime = 0;
    bool macLearned = false;

    NetworkManager(const NetworkManager &) = delete;
    NetworkManager &operator=(const NetworkManager &) = delete;

    static void onReceiveStatic(const uint8_t *mac, const uint8_t *data, int len);
    static void onDataSentStatic(const uint8_t *mac_addr, esp_now_send_status_t status);
    static void getMsg(void *pvParam);        // tarea que procesa cola de mensajes


    void onReceive(const uint8_t *mac, const uint8_t *data, int len);
    void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
    void MessageHandller(const esp_cmd_t &packet);
};

#endif
