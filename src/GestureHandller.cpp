#include "GestureHandller.h"
#include "NetworkManager.h"

auto& Network_manager = NetworkManager::getInstance();

// Máquina de estados para el movimiento
enum GestureState {
    STATE_NONE,
    STATE_FORWARD,
    STATE_BACKWARD
};

GestureState currentState = STATE_NONE;

// Pequeña zona muerta para evitar cambios por ruido
const float HYSTERESIS = 10.0;  // grados extra

void GestureHandler::GestureTest()
{
    float pitch = IMU::ypr[1];  // eje de inclinación adelante/atrás
    float yaw   = IMU::ypr[0];  // eje lateral izquierda/derecha

    // --- CONTROL LATERAL (Yaw) ---
    if (yaw >= axisThresholds[0]) {
        debug_msgs.msg(debug_msgs.INFO, "Right");
        Network_manager.Send("Right", 100);
    } 
    else if (yaw <= -axisThresholds[0]) {
        debug_msgs.msg(debug_msgs.INFO, "Left");
        Network_manager.Send("Left", 100);
    } 
    else {
        // Solo enviamos NONE lateral si no hay movimiento vertical
        if (currentState == STATE_NONE) {
            debug_msgs.msg(debug_msgs.INFO, "NONE (Lateral)");
            Network_manager.Send("NONE", 100);
        }
    }

    // --- CONTROL VERTICAL (Pitch) ---
    switch (currentState)
    {
        case STATE_NONE:
            if (pitch >= axisThresholds[1] + HYSTERESIS) {
                debug_msgs.msg(debug_msgs.INFO, "Forward");
                Network_manager.Send("Forward", 100);
                currentState = STATE_FORWARD;
            }
            else if (pitch <= -axisThresholds[1] - HYSTERESIS) {
                debug_msgs.msg(debug_msgs.INFO, "Backward");
                Network_manager.Send("Backward", 100);
                currentState = STATE_BACKWARD;
            }
            break;

        case STATE_FORWARD:
            if (pitch <= -HYSTERESIS) {  // Regresa cerca de 0 o inclinación opuesta
                debug_msgs.msg(debug_msgs.INFO, "None (Forward OFF)");
                Network_manager.Send("None", 100);
                currentState = STATE_NONE;
            }
            break;

        case STATE_BACKWARD:
            if (pitch >= HYSTERESIS) {
                debug_msgs.msg(debug_msgs.INFO, "None (Backward OFF)");
                Network_manager.Send("None", 100);
                currentState = STATE_NONE;
            }
            break;
    }
}

