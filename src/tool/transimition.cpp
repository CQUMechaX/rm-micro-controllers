#include <stm32f4xx_hal.h>
#include "usart.h"
#include "stm32f4xx_it.h"
#include <stm32f4xx_hal_can.h>
#include <cstring>

#include "dji/chassis_task.h"
#include "app/gimbal_control.hpp"
#include "app/microros_param.h"
#include "tool/transimition.hpp"

uint8_t cacheArray[2][LEGACY_CACHE_BYTE_LEN], acmCacheArray[LEGACY_CACHE_BYTE_LEN];
double *resultArray;

//串口中断
void dmaProcessHandler(UART_HandleTypeDef UART, DMA_HandleTypeDef rxDma, uint8_t frameCharLen, uint8_t cacheCharLen)
{
    if(UART.Instance->SR & UART_FLAG_RXNE)//接收到数据
    {
        __HAL_UART_CLEAR_FLAG(&UART, UART_FLAG_RXNE);
    }
    else if(UART.Instance->SR & UART_FLAG_IDLE)
    {
        static uint16_t this_time_rx_len = 0;

        __HAL_UART_CLEAR_PEFLAG(&UART);

        if ((rxDma.Instance->CR & DMA_SxCR_CT) == RESET)
        {
            /* Current memory buffer used is Memory 0 */
            //disable DMA
            __HAL_DMA_DISABLE(&rxDma);
            //get receive data length, length = set_data_length - remain_length
            //获取接收数据长度,长度 = 设定长度 - 剩余长度
            this_time_rx_len = cacheCharLen - rxDma.Instance->NDTR;
            //reset set_data_lenght //重新设定数据长度
            rxDma.Instance->NDTR = cacheCharLen;
            //set memory buffer 1 //设定缓冲区1
            rxDma.Instance->CR |= DMA_SxCR_CT;
            //enable DMA //使能DMA
            __HAL_DMA_ENABLE(&rxDma);

            if(this_time_rx_len == frameCharLen)
            {
                //editCallback(sbus_rx_buf[0], &rc_ctrl);
                //chassis_move.
                resultArray = (double*)cacheArray[0];
            }
        }
        else
        {
            /* Current memory buffer used is Memory 1 */
            //disable DMA
            __HAL_DMA_DISABLE(&rxDma);
            //get receive data length, length = set_data_length - remain_length
            this_time_rx_len = cacheCharLen - rxDma.Instance->NDTR;
            //reset set_data_length
            rxDma.Instance->NDTR = cacheCharLen;
            //set memory buffer 0
            DMA1_Stream1->CR &= ~(DMA_SxCR_CT);
            //enable DMA
            __HAL_DMA_ENABLE(&rxDma);

            if(this_time_rx_len == frameCharLen)
            {
                //处理遥控器数据
                //sbus_to_rc(sbus_rx_buf[1], &rc_ctrl);
                resultArray = (double*)cacheArray[1];
            }
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
    // __HAL_DMA_DISABLE(&hdma);
    while(hdma.Instance->CR & DMA_SxCR_EN)
    {
        __HAL_DMA_DISABLE(&hdma);
    }

    //memory buffer
    hdma.Instance->PAR = (uint32_t) & (USART3->DR);
    hdma.Instance->M0AR = (uint32_t)(rx1_buf);
    hdma.Instance->M1AR = (uint32_t)(rx2_buf);
    //data length
    hdma.Instance->NDTR = dma_buf_num;

    //enable double memory buffer
    SET_BIT(hdma.Instance->CR, DMA_SxCR_DBM);
    //enable DMA
    __HAL_DMA_ENABLE(&hdma);
}

/**
  * @brief          hal库CAN回调函数,接收电机数据
  * @param[in]      hcan:CAN句柄指针
  * @retval         none
  */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef rx_header;
    uint8_t rx_data[8];

    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_data);
    if(hcan == g_ros_param.hcan_gimbal)
    {
        jointUpdate(&g_gimbal, rx_header.StdId, rx_data);
    }

}

template<typename T>
bool jointUpdate(T * baseControl, uint32_t std_id, uint8_t * rx_data)
{
    for(JointData & iter_joint : baseControl->joint)
    {
        if(iter_joint.head_feedback == std_id)
        {
            iter_joint.locked = true;

            memmove(&iter_joint.feedback[1], &iter_joint.feedback[0],
                sizeof(JointData::CtrlInfo));
            JointData::CtrlInfo & ref = iter_joint.feedback[0];
            ref.angle = (static_cast<uint16_t>(rx_data[0]) << 8 | rx_data[1]);
            ref.speed = (static_cast<uint16_t>(rx_data[2]) << 8 | rx_data[3]);
            ref.current = (static_cast<uint16_t>(rx_data[4]) << 8 | rx_data[5]);
            ref.temperature = rx_data[6];
            
            iter_joint.locked = false;
            return true;
        }
    }
    return false;
}