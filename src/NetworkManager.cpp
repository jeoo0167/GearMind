#include "NetworkManager.h"

void  NetworkManager::Begin(const uint8_t reciverMac[])
{
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK)
    {
        debug_msgs.msg(debug_msgs.ERROR,"Falied to star ESP-NOW");
        return;
    }

    esp_now_peer_info_t peer_info = {};
    memcpy(peer_info.peer_addr, reciverMac,6);
    peer_info.channel = 0;
    peer_info.encrypt = false;

    if(esp_now_add_peer(&peer_info) != ESP_OK)
    {
        debug_msgs.msg(debug_msgs.ERROR,"Falied to add peer");
        return;
    }
    debug_msgs.msg(debug_msgs.INFO,"Succesfully add peer");
    memcpy(mac_addr, reciverMac, 6);

}

void NetworkManager::Send(const char *data,unsigned long data_delay=100)
{
    unsigned long time = millis();

    if(time -lastTime >= data_delay)
    {
        lastTime = time;
        strncpy(msg.cmd, data, sizeof(msg.cmd));
        msg.cmd[sizeof(msg.cmd) - 1] = '\0';
        esp_err_t result = esp_now_send(mac_addr,(uint8_t *)&msg,sizeof(msg));
        if(result == ESP_OK)
        {
            debug_msgs.msg(debug_msgs.INFO,"Command succesfully sent");
        }else
        {
            debug_msgs.msg(debug_msgs.ERROR,"Falied to send the command");
            Serial.println(result);
        }
    }
}