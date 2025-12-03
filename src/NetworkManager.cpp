#include "NetworkManager.h"

extern IMU Imu;
bool NetworkManager::connected = false;

NetworkManager::NetworkManager() : debug_msgs("NetworkManager.cpp") {}

void NetworkManager::Begin(const uint8_t receiverMac[])
{
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK)
    {
        debug_msgs.msg(debug_msgs.ERROR, "Failed to start ESP-NOW");
        return;
    }

    esp_now_register_recv_cb(NetworkManager::onReceiveStatic);
    esp_now_register_send_cb(NetworkManager::onDataSentStatic);

    esp_now_peer_info_t peer_info = {};
    memcpy(peer_info.peer_addr, receiverMac, 6);
    peer_info.channel = 0;
    peer_info.encrypt = false;

    if (esp_now_add_peer(&peer_info) != ESP_OK)
    {
        debug_msgs.msg(debug_msgs.ERROR, "Failed to add peer");
    }
    memcpy(mac_addr, receiverMac, 6);

    msgQueue = xQueueCreate(10, sizeof(esp_cmd_t));
    if (msgQueue == nullptr)
    {
        debug_msgs.msg(debug_msgs.ERROR, "Failed to create msgQueue");
        return;
    }

    xTaskCreatePinnedToCore(
        NetworkManager::getMsg,
        "GetMsgTask",
        2048,
        this,
        1,
        nullptr,
        1);

    debug_msgs.msg(debug_msgs.INFO, "NetworkManager started");
}

void NetworkManager::Send(const esp_cmd_t *packet, unsigned long data_delay)
{
    if(Imu.timer(data_delay))
    {
        memcpy(&msg, packet, sizeof(esp_cmd_t));

        esp_err_t result = esp_now_send(mac_addr, (uint8_t *)&msg, sizeof(msg));
        if (result != ESP_OK)
        {
            debug_msgs.msg(debug_msgs.ERROR, "Failed to send command");
            Serial.println(result);
        }
    }
}


esp_cmd_t NetworkManager::CreatePacket(const char *cmd, msg_type type)
{
    esp_cmd_t packet;
    strncpy(packet.cmd, cmd, sizeof(packet.cmd));
    packet.cmd[sizeof(packet.cmd) - 1] = '\0';
    packet.type = type;
    return packet;
}

void NetworkManager::onReceiveStatic(const uint8_t *mac, const uint8_t *data, int len)
{
    NetworkManager::getInstance().onReceive(mac, data, len);
}

void NetworkManager::onDataSentStatic(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    NetworkManager::getInstance().onDataSent(mac_addr, status);
}

void NetworkManager::onReceive(const uint8_t *mac, const uint8_t *data, int len)
{
    if (len != sizeof(esp_cmd_t))
    {
        Serial.println("[GORRA][WARN] Wrong packet size");
        return;
    }

    bool isBroadcast = true;
    for (int i = 0; i < 6; ++i)
        if (mac[i] != 0xFF) { isBroadcast = false; break; }

    esp_cmd_t packet;
    memcpy(&packet, data, sizeof(packet));

    if (!isBroadcast && !macLearned)
    {
        memcpy(mac_addr, mac, 6);
        macLearned = true;
        Serial.println("[GORRA] Learned robot MAC");
    }

    if (msgQueue)
    {
        if (xQueueSend(msgQueue, &packet, 0) != pdTRUE)
        {
            Serial.println("[GORRA][WARN] Queue full, discarding packet");
        }
    }
}

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
        }
    }
}

void NetworkManager::getMsg(void *pvParam)
{
    NetworkManager *self = reinterpret_cast<NetworkManager *>(pvParam);
    esp_cmd_t packet;

    while (true)
    {
        if (self->msgQueue && xQueueReceive(self->msgQueue, &packet, portMAX_DELAY) == pdTRUE)
        {
            self->MessageHandller(packet);
        }
    }
}

void NetworkManager::MessageHandller(const esp_cmd_t &packet)
{
    switch (packet.type)
    {
        case HEARTBEAT:
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
