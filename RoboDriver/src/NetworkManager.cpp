#include "NetworkManager.h"

void NetworkManager::Begin()
{
    WiFi.mode(WIFI_STA);

    Serial.print("[NetworkManager] MAC local: ");
    Serial.println(WiFi.macAddress());

    if (esp_now_init() != ESP_OK)
    {
        Serial.println("[NetworkManager][ERROR] No se pudo iniciar ESP-NOW");
        return;
    }

    esp_now_register_recv_cb(NetworkManager::onReceiveStatic);
    esp_now_register_send_cb(NetworkManager::onDataSentStatic);

    msgQueue = xQueueCreate(10, sizeof(esp_cmd_t));
    if (msgQueue == nullptr)
    {
        Serial.println("[NetworkManager][ERROR] No se pudo crear la cola de mensajes");
        return;
    }

    // Inicializar flags
    macLearned = false;
    connected = false;

    xTaskCreatePinnedToCore(
        msgHandler,
        "MsgHandlerTask",
        2048,
        this,
        2,
        nullptr,
        1
    );

    xTaskCreatePinnedToCore(
        heartBeatCallback,
        "HeartBeatTask",
        1024,
        this,
        1,
        nullptr,
        1
    );

    Serial.println("[NetworkManager][INFO] Receptor listo para recibir datos");
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
        Serial.println("[NetworkManager][WARN] Tamaño de mensaje inesperado");
        return;
    }

    bool isBroadcast = true;
    for (int i = 0; i < 6; ++i)
        if (mac[i] != 0xFF) { isBroadcast = false; break; }

    esp_cmd_t packet;
    memcpy(&packet, data, sizeof(packet));

    if (!isBroadcast && !macLearned)
    {
        memcpy(receivedMac, mac, 6);
        macLearned = true;

        esp_now_peer_info_t peerInfo = {};
        memcpy(peerInfo.peer_addr, receivedMac, 6);
        peerInfo.channel = 0;
        peerInfo.encrypt = false;
        esp_err_t r = esp_now_add_peer(&peerInfo);
        if (r == ESP_OK) {
            Serial.println("[NetworkManager][INFO] Peer agregado correctamente");
        } else if (r == ESP_ERR_ESPNOW_NOT_INIT) {
            Serial.println("[NetworkManager][ERROR] esp_now no inicializado al agregar peer");
        } else {
            Serial.print("[NetworkManager][INFO] esp_now_add_peer resultado: ");
            Serial.println(r);
        }
    }

    if (xQueueSend(msgQueue, &packet, 0) != pdTRUE)
        Serial.println("[NetworkManager][ERROR] No se pudo encolar el mensaje recibido");
}

void NetworkManager::onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
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
        Serial.print("[NetworkManager][WARN] Envío fallido count=");
        Serial.println(failCount);

        if (failCount >= MAX_FAILS)
        {
            if (connected) {
                Serial.println("[NetworkManager][ERROR] Conexión perdida (fallos consecutivos). Deteniendo motores.");
                motors.Stop();
            }
            connected = false;
        }
    }
}

void NetworkManager::msgHandler(void *pvParam)
{
    NetworkManager *self = (NetworkManager *)pvParam;
    esp_cmd_t packet;

    while (true)
    {
        if (xQueueReceive(self->msgQueue, &packet, portMAX_DELAY) == pdTRUE)
        {
            switch (packet.type)
            {
                case COMMAND:
                    self->commandHandler(packet.cmd);
                    break;

                case DATA:
                    Serial.print("[NetworkManager][INFO] Data: ");
                    Serial.println(packet.cmd);
                    break;

                case HEARTBEAT:
                    break;

                default:
                    Serial.println("[NetworkManager][WARN] Tipo desconocido");
                    break;
            }
        }
    }
}

void NetworkManager::commandHandler(const char *command)
{
    char macStr[18];
    snprintf(macStr, sizeof(macStr),
             "%02X:%02X:%02X:%02X:%02X:%02X",
             receivedMac[0], receivedMac[1], receivedMac[2],
             receivedMac[3], receivedMac[4], receivedMac[5]);

    Serial.print("[NetworkManager][INFO] De: ");
    Serial.print(macStr);
    Serial.print(" | Cmd: ");
    Serial.println(command);

    if (strcmp(command, "NONE") == 0)
        motors.Stop();
    else if (strcmp(command, "Forward") == 0)
        motors.Forward();
    else if (strcmp(command, "Backward") == 0)
        motors.Backward();
    else if (strcmp(command, "Right") == 0)
        motors.Right();
    else if (strcmp(command, "Left") == 0)
        motors.Left();
}


void NetworkManager::heartBeatCallback(void *pvParam)
{
    NetworkManager *self = (NetworkManager *)pvParam;

    const uint8_t broadcastMAC[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

    esp_cmd_t heartbeatMsg;
    heartbeatMsg.type = HEARTBEAT;
    strncpy(heartbeatMsg.cmd, "-", sizeof(heartbeatMsg.cmd));
    heartbeatMsg.cmd[sizeof(heartbeatMsg.cmd)-1] = '\0';

    while (true)
    {
        const uint8_t *target = (self->macLearned ? self->receivedMac : broadcastMAC);

        esp_err_t res = esp_now_send(target, (uint8_t *)&heartbeatMsg, sizeof(heartbeatMsg));
        if (res != ESP_OK) {
            Serial.print("[NetworkManager][WARN] esp_now_send(heartbeat) result=");
            Serial.println(res);
        } else {
            Serial.println("[NetworkManager][INFO] Heartbeat enviado");
        }

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
