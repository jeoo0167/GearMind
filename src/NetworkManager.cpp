#include "NetworkManager.h"
#define TRACE()  Serial.printf("[TRACE] %s:%d\n", __FUNCTION__, __LINE__);

extern IMU Imu;
bool NetworkManager::connected = false;

NetworkManager::NetworkManager() : debug_msgs("NetworkManager.cpp") {}

void NetworkManager::Begin(const uint8_t receiverMac[])
{
    dnsServer.stop();
    WiFi.disconnect(true);
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);
    delay(100);
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
    espnowEnable = true;

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
    if(!espnowEnable) return;

    if(Imu.timer(data_delay) && espnowEnable)
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
    if(espnowEnable)
    {
        esp_cmd_t packet;
        strncpy(packet.cmd, cmd, sizeof(packet.cmd));
        packet.cmd[sizeof(packet.cmd) - 1] = '\0';
        packet.type = type;
        return packet;
    }
}

void NetworkManager::onReceiveStatic(const uint8_t *mac, const uint8_t *data, int len)
{
    if(!NetworkManager::getInstance().espnowEnable) return;
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

    while (!self->espnowEnable) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

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
        {
            String msg = String(packet.cmd);

            if (msg == "Ready") {
                msgReceived = true;
                Serial.println("[GORRA] Receiver ready!");
            }

            if (msg.startsWith("IP=")) {
                external_ip = msg.substring(3); // si external_ip es String
                Serial.print("[GORRA] External IP received: ");
                Serial.println(external_ip);
            }
        }
        break;

        default:
            Serial.println("[GORRA] Unknown packet type");
            break;
    }
}


void NetworkManager::serverInit(const char *ssid, const char *password)
{
    esp_cmd_t pck[4]; 
    pck[0] = CreatePacket("SERVER", COMMAND); 
    pck[1] = CreatePacket(ssid, COMMAND); 
    pck[2] = CreatePacket(password, COMMAND); 
    pck[3] = CreatePacket("END", COMMAND);

    for (int i = 0; i < 4; i++) {
        Send(&pck[i], 0);  // envío inmediato
        delay(20);
    }

    espnowEnable = false;
    esp_now_unregister_recv_cb();
    esp_now_unregister_send_cb();
    esp_now_deinit();

    macLearned = false;
    connected = false;
    msgReceived = false;
    external_ip = "";
    if (msgQueue) xQueueReset(msgQueue);

    WiFi.mode(WIFI_OFF);
    delay(100);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);

    apIP = WiFi.softAPIP();

    dnsServer.start(53, "*", apIP);

    server.on("/", HTTP_GET, [&](AsyncWebServerRequest *request){
        request->send(LittleFS, "/index.html", "text/html");
    });

    server.on("/generate_204", HTTP_GET, [&](AsyncWebServerRequest *request){
        request->send(200, "text/plain", "OK");
    });

    server.on("/fwlink", HTTP_GET, [&](AsyncWebServerRequest *request){
        request->redirect("/");
    });

    server.on("/hotspot-detect.html", HTTP_GET, [&](AsyncWebServerRequest *request){
        request->redirect("/");
    });

    server.on("/captive.txt", HTTP_GET, [&](AsyncWebServerRequest *request){
        request->redirect("/");
    });

    server.serveStatic("/", LittleFS, "/");

    // Redirigir TODO lo demás al portal
    server.onNotFound([](AsyncWebServerRequest *request){
        request->redirect("/");
    });

    server.on("/config.json", HTTP_GET, [](AsyncWebServerRequest *request){
        Serial.println("GET /config.json");

        if (!LittleFS.exists("/config.json")) {
            request->send(404, "application/json", "{\"error\":\"config.json not found\"}");
            return;
        }

        request->send(LittleFS, "/config.json", "application/json");
    });


    server.on("/saveConfig", HTTP_POST, 
        [](AsyncWebServerRequest *request){},
        NULL,
        [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) 
        {
            Serial.println("POST /saveConfig recibido");

            String body = "";
            for (size_t i = 0; i < len; i++) body += (char)data[i];

            Serial.println("Datos recibidos:");
            Serial.println(body);

            File f = LittleFS.open("/config.json", "w");
            if (!f) {
                request->send(500, "application/json", "{\"status\":\"error saving file\"}");
                return;
            }
            f.print(body);
            f.close();

            request->send(200, "application/json", "{\"status\":\"ok\"}");
        }
    );


    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");

    server.begin();
}

bool NetworkManager::SendUdp(String udpMessage)
{
    if(msgReceived && external_ip.length() >0)
    {
        udp.beginPacket(external_ip.c_str(), port);
        udp.print(udpMessage);
        udp.endPacket();
        return true;
    }
    else {return false;}

}