#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <esp_now.h>
#include <WiFi.h>
#include <Arduino.h>
#include "DebugMsg.h"

// tipos de mensaje
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

    // inicialización con la MAC destino (robot)
    void Begin(const uint8_t receiverMac[]);
    // envío (envía packet respetando data_delay)
    void Send(const esp_cmd_t *packet, unsigned long data_delay = 50);
    esp_cmd_t CreatePacket(const char *cmd, msg_type type);

    // estado público de conexión (se mantiene por la instancia, pero la usamos estática)
    static bool connected;
    unsigned long lastHeartbeat = 0;

private:
    // MAC del robot (peer)
    uint8_t mac_addr[6] = {0};

    // buffer para enviar
    esp_cmd_t msg;
    DebugMsgs debug_msgs;
    unsigned long lastTime = 0;

    // Cola y tareas (emisor también recibe heartbeats del robot)
    QueueHandle_t msgQueue = nullptr;

    // No copiar singleton
    NetworkManager(const NetworkManager &) = delete;
    NetworkManager &operator=(const NetworkManager &) = delete;

    // ---- callbacks y tareas (estáticos para poder registrarlos en C APIs) ----
    static void onReceiveStatic(const uint8_t *mac, const uint8_t *data, int len);
    static void onDataSentStatic(const uint8_t *mac_addr, esp_now_send_status_t status);
    static void msgHandler(void *pvParam);        // tarea que procesa cola de mensajes
    static void onReceive_task_wrapper(void *pv); // (opcional)

    // métodos de instancia que hacen el trabajo real
    void onReceive(const uint8_t *mac, const uint8_t *data, int len);
    void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
    void processMessage(const esp_cmd_t &packet);

    // flag local para saber si ya aprendimos la MAC de peer (evitar overwrite con broadcast)
    bool macLearned = false;
};

#endif
