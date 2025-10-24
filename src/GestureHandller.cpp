#include "GestureHandller.h"
#include "NetworkManager.h"

auto& Network_manager = NetworkManager::getInstance();

void GestureHandler::GestureTest()
{
    if(IMU::ypr[0] >= axisThresholds[0])
    {
        debug_msgs.msg(debug_msgs.INFO,"Right");
        Network_manager.Send("Right",100);
    }
    else if(IMU::ypr[0] <= -axisThresholds[0])
    {
        debug_msgs.msg(debug_msgs.INFO,"Left");
        Network_manager.Send("Left",100);
    }
    else if(IMU::ypr[1] >= axisThresholds[1])
    {
        debug_msgs.msg(debug_msgs.INFO,"Backward");
        Network_manager.Send("Backward",100);
    }
    else if(IMU::ypr[1] <= -axisThresholds[1])
    {
        debug_msgs.msg(debug_msgs.INFO,"Forward");
        Network_manager.Send("Forward",100);

    }
    else{
        debug_msgs.msg(debug_msgs.INFO,"NONE");
        Network_manager.Send("NONE",100);
    }
}
