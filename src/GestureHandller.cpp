#include "GestureHandller.h"
#include "NetworkManager.h"

auto& Network_manager = NetworkManager::getInstance();

const unsigned long debounceTime = 200;   // ms para confirmar una dirección
const float hysteresis = 0.05;            // margen adicional para estabilidad

// --- VARIABLES ESTÁTICAS (persisten entre llamadas) ---
static String lastDirection = "NONE";
static unsigned long lastChangeTime = 0;

void GestureHandler::GestureTest()
{
    String currentDirection = "NONE";

    // --- EVALUAR CON HISTERESIS ---
    if (IMU::acc[2] >= (zThreshold[0] + hysteresis))
        currentDirection = "FORWARD";
    else if (IMU::acc[2] <= -(zThreshold[1] + hysteresis))
        currentDirection = "BACKWARD";
    else if (IMU::acc[1] >= (yThreshold[0] + hysteresis))
        currentDirection = "LEFT";
    else if (IMU::acc[1] <= -(yThreshold[1] + hysteresis))
        currentDirection = "RIGHT";
    else
        currentDirection = "NONE";

    // --- FILTRO ANTIRREBOTE TEMPORAL ---
    unsigned long now = millis();

    if (currentDirection != lastDirection)
    {
        // Solo cambiar si ha pasado suficiente tiempo con la nueva dirección
        if (now - lastChangeTime > debounceTime)
        {
            lastDirection = currentDirection;
            lastChangeTime = now;

            debug_msgs.msg(debug_msgs.INFO, currentDirection.c_str());
        }
    }
    else
    {
        // Si se mantiene igual, actualiza tiempo base
        lastChangeTime = now;
    }
}

