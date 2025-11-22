#include "NetworkManager.h"

bool NetworkManager::connected = false; // definición de la variable estática

NetworkManager::NetworkManager()
    : debug_msgs("NetworkManager.cpp")
{
    // constructor vacío, inicializadores ya en la declaración del header
}

// ----------------------- Begin -----------------------
void NetworkManager::Begin(const uint8_t receiverMac[])
{
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK)
    {
        debug_msgs.msg(debug_msgs.ERROR, "Failed to start ESP-NOW");
        return;
    }

    // registrar callbacks
    esp_now_register_recv_cb(NetworkManager::onReceiveStatic);
    esp_now_register_send_cb(NetworkManager::onDataSentStatic);

    // añadir peer (robot)
    esp_now_peer_info_t peer_info = {};
    memcpy(peer_info.peer_addr, receiverMac, 6);
    peer_info.channel = 0;
    peer_info.encrypt = false;

    if (esp_now_add_peer(&peer_info) != ESP_OK)
    {
        debug_msgs.msg(debug_msgs.ERROR, "Failed to add peer");
        // no return: tal vez el peer ya existe, pero seguimos (podrías comprobar el código)
    }
    memcpy(mac_addr, receiverMac, 6);

    // crear cola para recibir mensajes desde onReceive
    msgQueue = xQueueCreate(10, sizeof(esp_cmd_t));
    if (msgQueue == nullptr)
    {
        debug_msgs.msg(debug_msgs.ERROR, "Failed to create msgQueue");
        return;
    }

    // crear la tarea que procesa la cola
    xTaskCreatePinnedToCore(
        NetworkManager::msgHandler,
        "MsgHandler",
        2048,
        this,
        1,
        nullptr,
        1);

    debug_msgs.msg(debug_msgs.INFO, "NetworkManager started");
}

// ----------------------- Send -----------------------
void NetworkManager::Send(const esp_cmd_t *packet, unsigned long data_delay)
{
    unsigned long time = millis();

    if (time - lastTime >= data_delay)
    {
        lastTime = time;
        memcpy(&msg, packet, sizeof(esp_cmd_t));

        esp_err_t result = esp_now_send(mac_addr, (uint8_t *)&msg, sizeof(msg));
        if (result != ESP_OK)
        {
            debug_msgs.msg(debug_msgs.ERROR, "Failed to send command");
            Serial.println(result);
        }
    }
}

// ----------------------- CreatePacket -----------------------
esp_cmd_t NetworkManager::CreatePacket(const char *cmd, msg_type type)
{
    esp_cmd_t packet;
    strncpy(packet.cmd, cmd, sizeof(packet.cmd));
    packet.cmd[sizeof(packet.cmd) - 1] = '\0';
    packet.type = type;
    return packet;
}

// ----------------------- static wrappers -----------------------
void NetworkManager::onReceiveStatic(const uint8_t *mac, const uint8_t *data, int len)
{
    NetworkManager::getInstance().onReceive(mac, data, len);
}

void NetworkManager::onDataSentStatic(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    NetworkManager::getInstance().onDataSent(mac_addr, status);
}

// ----------------------- onReceive (instancia) -----------------------
void NetworkManager::onReceive(const uint8_t *mac, const uint8_t *data, int len)
{
    // validar longitud
    if (len != sizeof(esp_cmd_t))
    {
        Serial.println("[GORRA][WARN] Wrong packet size");
        return;
    }

    // detectar broadcast
    bool isBroadcast = true;
    for (int i = 0; i < 6; ++i)
        if (mac[i] != 0xFF) { isBroadcast = false; break; }

    // copiar paquete
    esp_cmd_t packet;
    memcpy(&packet, data, sizeof(packet));

    // si no es broadcast y no hemos aprendido la MAC, la aprendemos
    if (!isBroadcast && !macLearned)
    {
        memcpy(mac_addr, mac, 6);
        macLearned = true;
        Serial.println("[GORRA] Learned robot MAC");
    }

    // encolar paquete para procesarlo en la tarea (no bloqueamos aquí)
    if (msgQueue)
    {
        if (xQueueSend(msgQueue, &packet, 0) != pdTRUE)
        {
            Serial.println("[GORRA][WARN] Queue full, discarding packet");
        }
    }
}

// ----------------------- onDataSent (instancia) -----------------------
void NetworkManager::onDataSent(const uint8_t *mac_addr_param, esp_now_send_status_t status)
{
    static int failCount = 0;
    const int MAX_FAILS = 5;

    if (status == ESP_NOW_SEND_SUCCESS)
    {
        failCount = 0;
        connected = true;
    }
    else
    {
        failCount++;
        Serial.print("[GORRA][WARN] send failed count=");
        Serial.println(failCount);

        if (failCount >= MAX_FAILS)
        {
            if (connected)
            {
                Serial.println("[GORRA][ERROR] Connection lost (consecutive failures).");
            }
            connected = false;
            // tomar acción (led, stop, reintentar registrar peer, etc.)
        }
    }
}

// ----------------------- msgHandler (tarea) -----------------------
void NetworkManager::msgHandler(void *pvParam)
{
    NetworkManager *self = reinterpret_cast<NetworkManager *>(pvParam);
    esp_cmd_t packet;

    while (true)
    {
        if (self->msgQueue && xQueueReceive(self->msgQueue, &packet, portMAX_DELAY) == pdTRUE)
        {
            self->processMessage(packet);
        }
    }
}

// ----------------------- processMessage (instancia) -----------------------
void NetworkManager::processMessage(const esp_cmd_t &packet)
{
    switch (packet.type)
    {
        case HEARTBEAT:
            // actualizar timestamp/estado
            lastHeartbeat = millis();
            if (!connected) {
                connected = true;
                Serial.println("[GORRA] Robot connected (heartbeat).");
            }
            break;

        case DATA:
            Serial.print("[GORRA] Data: ");
            Serial.println(packet.cmd);
            break;

        case COMMAND:
            Serial.print("[GORRA] Command (from robot?): ");
            Serial.println(packet.cmd);
            break;

        default:
            Serial.println("[GORRA] Unknown packet type");
            break;
    }
}
