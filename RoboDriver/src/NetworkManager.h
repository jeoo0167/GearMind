#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <esp_now.h>
#include <WiFi.h>
#include <Arduino.h>
#include <Motors.h>
#include "pins.h"

class NetworkManager
{
public:
    // Singleton
    inline static NetworkManager& getInstance()
    {
        static NetworkManager instance;
        return instance;
    }

    void Begin();
    void commandHandler(const char* command);
    
    static int failCount;
    const int MAX_FAILS = 3;
    const int dataDelay = 100;

private:

    
    typedef enum : uint8_t {
        DATA,
        COMMAND,
        HEARTBEAT
    } msg_type;

    typedef struct __attribute__((packed)) {
        char cmd[32];      // 32 bytes
        msg_type type;     // 1 byte
    } esp_cmd_t;

    esp_cmd_t msg;
    Motors motors;
    
    QueueHandle_t msgQueue = nullptr;

    uint8_t receivedMac[6] = {0};

    static void onReceiveStatic(const uint8_t *mac, const uint8_t *data, int len);
    static void onDataSentStatic(const uint8_t *mac_addr, esp_now_send_status_t status);
    void onReceive(const uint8_t *mac, const uint8_t *data, int len);
    void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
    static void msgHandler(void *pvParam);
    static void heartBeatCallback(void *pvParam);

    // Evita duplicaciones del Singleton
    NetworkManager() = default;
    NetworkManager(const NetworkManager&) = delete;
    NetworkManager& operator=(const NetworkManager&) = delete;
    bool macLearned = false;
    bool connected;
};

#endif