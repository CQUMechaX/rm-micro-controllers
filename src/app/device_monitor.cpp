#include "app/device_monitor.hpp"
#include "tool/part_control.hpp"
#include "override.h"
#include <cmsis_os.h>
#include <utility>
#include <cstring>

DeviceMonitor gDeviceMonitor;

void deviceMonitor(void * arguments)
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
    uint8_t can_location = this->can_cast_to_num(hcan);

    joint.head_feedback = gCanHeadFeedback[joint.id];
    joint.head_target = gCanHeadTarget[joint.id];
    for(DeviceStatus & device_iter : this->device_joint_[can_location])
    {
        if(device_iter.ptr.joint->head_feedback == joint.head_feedback)
        {
            freertosErrorHandler(__FILE__, __LINE__); /** Same feedback StdId in one CAN location */
        }
    }

    /** bad practice: probably lose the pointer.
     *  @brief Refer to STL vector, end() point to the next to last element, which is
     *  the element to be pushed back. 
     */
    this->device_dict_add(static_cast<void*>(&joint), &*(this->device_joint_[can_location].end()));
    this->device_joint_[can_location].push_back(
        DeviceStatus{10, &joint}
        );

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
    if((device_dbus_.offline_time = xTaskGetTickCount() - device_dbus_.tick) > 30)
    {
        device_dbus_.online = false;
    }
    else
    {
        device_dbus_.online = true;
    }

    for(auto & list_iter : this->device_joint_)
    {
        for(DeviceStatus & device : list_iter)
        {
            bool device_online_before = device.online;
            JointData * & joint = device.ptr.joint;
            if((device.offline_time = xTaskGetTickCount() - device.tick) > 30)
            {
                device.online = false;
            }
            else
            {
                taskENTER_CRITICAL();
                memmove(&joint->feedback[1], &joint->feedback[0],
                    sizeof(JointData::CtrlInfo) * 4);
                memmove(&joint->feedback[0], &device.data.joint,
                    sizeof(JointData::CtrlInfo));
                taskEXIT_CRITICAL();
                device.online = true;
            }
            if(device_online_before != device.online)
            {
                PartControl::set_online_cnt(joint->can_num, joint->id, device.online ? 1 : -1);
            }
        }
    }
    return true;
}