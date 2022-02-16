#include "app/device_monitor.hpp"
#include "override.h"
#include <cmsis_os2.h>
#include <FreeRTOS.h>
#include <task.h>
#include <utility>
#include <cstring>

DeviceMonitor gDeviceMonitor;

void deviceMonitor(void * pvParameters)
{
    while (true)
    {
        gDeviceMonitor.update();
        osDelay(gDeviceMonitor.tick_ms_);
    }
    
}

bool DeviceMonitor::update(void)
{
    this->update_to_controller();
    return true;
}

bool DeviceMonitor::update_single_isr(DeviceStatus & device)
{
    return this->update_single_isr(device, (DeviceErrorList)0);
}

bool DeviceMonitor::update_single_isr(DeviceStatus & device, DeviceErrorList error)
{
    device.tick = xTaskGetTickCountFromISR();
    device.error = error;
    return true;
}

bool DeviceMonitor::register_and_init(CAN_HandleTypeDef & hcan, JointData & joint)
{
    uint8_t at_location = this->can_cast_to_num(hcan);

    joint.head_feedback = gCanHeadFeedback[joint.id];
    joint.head_target = gCanHeadTarget[joint.id];
    for(DeviceStatus & device_iter : this->device_joint_[at_location])
    {
        if(device_iter.ptr.joint->head_feedback == joint.head_feedback)
        {
            freertosErrorHandler(__FILE__, __LINE__);
        }
    }

    /** bad practice: probably lose the pointer.
     *  @brief Refer to STL vector, end() point to the next to last element, which is
     *  the element to be pushed back. 
     */
    this->device_dict_add(static_cast<void*>(&joint), &*(this->device_joint_[at_location].end()));
    this->device_joint_[at_location].push_back(
        DeviceStatus{10, &joint}
        );

    return true;
}

bool DeviceMonitor::set_target(CAN_HandleTypeDef & hcan, JointData & joint)
{
    this->device_target_[this->can_cast_to_num(hcan)][joint.id] = joint.target;
    return true;
}

std::vector<DeviceStatus> & DeviceMonitor::get_register_list(CAN_HandleTypeDef & hcan)
{
    return this->device_joint_[this->can_cast_to_num(hcan)];
}

bool DeviceMonitor::get_online(JointData & joint)
{
    return this->device_dict_find(static_cast<void*>(&joint)).online;
}

bool DeviceMonitor::device_dict_add(void * key, DeviceStatus * value)
{
    return this->
        device_dict_
        .insert(std::make_pair(key, value))
        .second;
}
DeviceStatus & DeviceMonitor::device_dict_find(void * key)
{
    return *(this->device_dict_.find(key)->second);
    /** @todo error catch */
}


bool DeviceMonitor::update_to_controller(void)
{
    for(auto & list_iter : this->device_joint_)
    {
        for(DeviceStatus & device : list_iter)
        {
            if((device.offline_time = xTaskGetTickCount() - device.tick) > 10)
            {
                device.online = false;
            }
            else
            {
                JointData *& joint = device.ptr.joint;
                taskENTER_CRITICAL();
                memmove(&joint->feedback[1], &joint->feedback[0],
                    sizeof(JointData::CtrlInfo) * 4);
                memmove(&joint->feedback[0], &device.data.joint,
                    sizeof(JointData::CtrlInfo));
                taskEXIT_CRITICAL();
                device.online = true;
            }
        }
    }
    return true;
}