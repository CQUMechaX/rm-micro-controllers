#include <stm32f4xx_hal.h>
#include <stm32f4xx_it.h>
#include <stm32f4xx_hal_can.h>
#include <cstring>

#include "usart.h"
#include "override.h"
// #include "dji/chassis_task.h"
// #include "app/gimbal_control.hpp"
#include "app/microros_param.h"
#include "tool/transimition.hpp"

uint8_t gLegacyCacheArray[2][LEGACY_CACHE_BYTE_LEN], acmCacheArray[LEGACY_CACHE_BYTE_LEN];
uint8_t gDbusCacheArray[2][DBUS_CACHE_BYTE_LEN];
static double gSafeArray[2];
double *gLegacyResultArray = gSafeArray;

const uint32_t gCanHeadTarget[17] = {
 0, 0x200, 0x200, 0x200, 0x200, 0x1FF, 0x1FF, 0x1FF, 0x1FF,
    0x2FF, 0x2FF, 0x2FF, 0x000, 0x000, 0x000, 0x000, 0
},
gCanHeadFeedback[17] = {
 0, 0x201, 0x202, 0x203, 0x204, 0x205, 0x206, 0x207, 0x208,
    0x209, 0x20A, 0x20B, 0x000, 0x000, 0x000, 0x000, 0
};



//串口中断
void dmaProcessHandler(
    UART_HandleTypeDef uart, DMA_HandleTypeDef rx_dma, uint8_t cache_byte_len,
    uint8_t frame_byte_len, HAL_StatusTypeDef( * callback_function)(bool)
    )
{
    if(uart.Instance->SR & UART_FLAG_RXNE)//接收到数据
    {
        __HAL_UART_CLEAR_FLAG(&uart, UART_FLAG_RXNE);

        // __HAL_UART_CLEAR_PEFLAG(&uart);
    }
    else if(uart.Instance->SR & UART_FLAG_IDLE)
    {
        __HAL_UART_CLEAR_PEFLAG(&uart);

        __HAL_DMA_DISABLE(&rx_dma); // disable DMA
        static uint16_t this_time_rx_len = 0;
        this_time_rx_len = cache_byte_len - rx_dma.Instance->NDTR; // get receive data length, length = set_data_length - remain_length
        rx_dma.Instance->NDTR = cache_byte_len; // reset set_data_length
        if(rx_dma.Instance->CR & DMA_SxCR_CT)
        {// used is 1, changed to 0
            rx_dma.Instance->CR ^= DMA_SxCR_CT;
            __HAL_DMA_ENABLE(&rx_dma); // enable DMA
            ((this_time_rx_len == frame_byte_len) ? callback_function(1) : HAL_ERROR);
        }
        else
        {// used is 0, changed to 1 (== RESET)
            rx_dma.Instance->CR |= DMA_SxCR_CT;
            __HAL_DMA_ENABLE(&rx_dma); // enable DMA
            ((this_time_rx_len == frame_byte_len) ? callback_function(0) : HAL_ERROR);
        }
    }
}


void initDmaCache(UART_HandleTypeDef huart, DMA_HandleTypeDef hdma, uint8_t *rx1_buf, uint8_t *rx2_buf, uint16_t dma_buf_num)
{
    //enable the DMA transfer for the receiver request
    SET_BIT(huart.Instance->CR3, USART_CR3_DMAR);
    //enalbe idle interrupt
    __HAL_UART_ENABLE_IT(&huart, UART_IT_IDLE);
    //disable DMA
    //__HAL_DMA_DISABLE(&hdma);
    while(hdma.Instance->CR & DMA_SxCR_EN)
    {
        __HAL_DMA_DISABLE(&hdma);
    }

    //memory buffer
    hdma.Instance->PAR = (uint32_t) & (huart.Instance->DR);
    hdma.Instance->M0AR = (uint32_t)(rx1_buf);
    hdma.Instance->M1AR = (uint32_t)(rx2_buf);
    //data length
    hdma.Instance->NDTR = dma_buf_num;

    //enable double memory buffer
    SET_BIT(hdma.Instance->CR, DMA_SxCR_DBM);
    //enable DMA
    __HAL_DMA_ENABLE(&hdma);
}

/** This function enables HCAN1 & HCAN2's function with filter and notification.
 * @see override.h
 */
HAL_StatusTypeDef transimitionCanStart(void)
{

    CAN_FilterTypeDef can_filter_st;
    can_filter_st.FilterActivation = ENABLE;
    can_filter_st.FilterMode = CAN_FILTERMODE_IDMASK;
    can_filter_st.FilterScale = CAN_FILTERSCALE_32BIT;
    can_filter_st.FilterIdHigh = 0x0000;
    can_filter_st.FilterIdLow = 0x0000;
    can_filter_st.FilterMaskIdHigh = 0x0000;
    can_filter_st.FilterMaskIdLow = 0x0000;
    can_filter_st.FilterBank = 0;
    can_filter_st.FilterFIFOAssignment = CAN_RX_FIFO0;
    HAL_CAN_ConfigFilter(&HCAN1, &can_filter_st);
    HAL_CAN_Start(&HCAN1);
    HAL_CAN_ActivateNotification(&HCAN1, CAN_IT_RX_FIFO0_MSG_PENDING);

    can_filter_st.SlaveStartFilterBank = 14;
    can_filter_st.FilterBank = 14;
    HAL_CAN_ConfigFilter(&HCAN2, &can_filter_st);
    HAL_CAN_Start(&HCAN2);
    HAL_CAN_ActivateNotification(&HCAN2, CAN_IT_RX_FIFO0_MSG_PENDING);

    return HAL_OK;
}

HAL_StatusTypeDef transimitionLegacyRx(bool section)
{
    gLegacyResultArray = (double*)gLegacyCacheArray[section];
    return HAL_OK;
}

HAL_StatusTypeDef transimitionDbusRx(bool section)
{
    return dbusUpdate(gDbusCacheArray[section]) ? HAL_OK : HAL_ERROR;
}

/**
  * @brief          hal库CAN回调函数,接收电机数据
  * @param[in]      hcan:CAN句柄指针
  * @retval         none
  */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef rx_header;
    uint8_t rx_data[8], can_num;

    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_data);
    if((can_num = gDeviceMonitor.can_cast_to_num(hcan)))
    {
        jointUpdate(gDeviceMonitor.device_joint_[can_num], rx_header.StdId, rx_data);
    }
    else
    {
        freertosErrorHandler(__FILE__, __LINE__);
    }
    return;
}

bool jointUpdate(std::vector<DeviceStatus> & device_list, uint32_t std_id, uint8_t * rx_data)
{
    for(DeviceStatus & joint_device : device_list)
    {
        // JointData *& joint = *iter_joint_device.ptr.joint;
        if(joint_device.ptr.joint->head_feedback == std_id)
        {
            JointData::CtrlInfo & ref = joint_device.data.joint;
            ref.angle = (static_cast<uint16_t>(rx_data[0]) << 8 | rx_data[1]);
            ref.speed = (static_cast<uint16_t>(rx_data[2]) << 8 | rx_data[3]);
            ref.current = (static_cast<uint16_t>(rx_data[4]) << 8 | rx_data[5]);
            ref.temperature = rx_data[6];
            gDeviceMonitor.update_single_isr(joint_device);
            return true;
        }
    }
    return false;
}

bool dbusUpdate(uint8_t * rx_data)
{
    DbusData & rc_ctrl = gDeviceMonitor.device_dbus_.data.dbus;
    rc_ctrl.rc.channel[0] = (rx_data[0] | (rx_data[1] << 8)) & 0x07FF;        //!< Channel 0
    rc_ctrl.rc.channel[1] = ((rx_data[1] >> 3) | (rx_data[2] << 5)) & 0x07FF; //!< Channel 1
    rc_ctrl.rc.channel[2] = ((rx_data[2] >> 6) | (rx_data[3] << 2) |          //!< Channel 2
                             (rx_data[4] << 10)) &0x07FF;
    rc_ctrl.rc.channel[3] = ((rx_data[4] >> 1) | (rx_data[5] << 7)) & 0x07FF; //!< Channel 3
    rc_ctrl.rc.channel[0] -= RC_CH_VALUE_OFFSET;
    rc_ctrl.rc.channel[1] -= RC_CH_VALUE_OFFSET;
    rc_ctrl.rc.channel[2] -= RC_CH_VALUE_OFFSET;
    rc_ctrl.rc.channel[3] -= RC_CH_VALUE_OFFSET;
    rc_ctrl.rc.button[0] = ((rx_data[5] >> 4) & 0x0003);                  //!< Switch left
    rc_ctrl.rc.button[1] = ((rx_data[5] >> 4) & 0x000C) >> 2;                       //!< Switch right
    if(
        abs(rc_ctrl.rc.channel[0]) > RC_CHANNEL_VALUE_ERROR ||
        abs(rc_ctrl.rc.channel[1]) > RC_CHANNEL_VALUE_ERROR ||
        abs(rc_ctrl.rc.channel[2]) > RC_CHANNEL_VALUE_ERROR ||
        abs(rc_ctrl.rc.channel[3]) > RC_CHANNEL_VALUE_ERROR ||
        !rc_ctrl.rc.button[0] || !rc_ctrl.rc.button[1]
    )
    {
        memset(&rc_ctrl, 0, sizeof(rc_ctrl));
        gDeviceMonitor.update_single_isr(gDeviceMonitor.device_dbus_, DeviceErrorList::kDataMismatch);
        return false;
    }

    rc_ctrl.mouse.x = rx_data[6] | (rx_data[7] << 8);                    //!< Mouse X axis
    rc_ctrl.mouse.y = rx_data[8] | (rx_data[9] << 8);                    //!< Mouse Y axis
    rc_ctrl.mouse.z = rx_data[10] | (rx_data[11] << 8);                  //!< Mouse Z axis
    rc_ctrl.mouse.press_l = rx_data[12];                                  //!< Mouse Left Is Press ?
    rc_ctrl.mouse.press_r = rx_data[13];                                  //!< Mouse Right Is Press ?
    rc_ctrl.key.v = rx_data[14] | (rx_data[15] << 8);                    //!< KeyBoard value
    rc_ctrl.rc.channel[4] = rx_data[16] | (rx_data[17] << 8);                 //NULL
    // rc_ctrl.rc.channel[4] -= RC_CH_VALUE_OFFSET;

    gDeviceMonitor.update_single_isr(gDeviceMonitor.device_dbus_);
    return true;
}

HAL_StatusTypeDef transimitionCanTx(
    CAN_HandleTypeDef * hcan, uint32_t std_id,
    int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4
    )
{
    static CAN_TxHeaderTypeDef tx_message;
    static uint8_t can_send_data[8];
    static uint32_t * send_mail_box;
    tx_message.StdId = std_id;
    tx_message.IDE = CAN_ID_STD;
    tx_message.RTR = CAN_RTR_DATA;
    tx_message.DLC = 0x08;
    can_send_data[0] = motor1 >> 8;
    can_send_data[1] = motor1;
    can_send_data[2] = motor2 >> 8;
    can_send_data[3] = motor2;
    can_send_data[4] = motor3 >> 8;
    can_send_data[5] = motor3;
    can_send_data[6] = motor4 >> 8;
    can_send_data[7] = motor4;

    HAL_CAN_AddTxMessage(hcan, &tx_message, can_send_data, send_mail_box);
    return HAL_OK;
}