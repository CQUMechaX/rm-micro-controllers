#include <stm32f4xx_hal.h>
#include "app/connectivity.h"
#include "usart.h"
#include "stm32f4xx_it.h"

#include "dji/chassis_task.h"

int8_t cacheArray[2][LEGACY_CACHE_INT8_LEN];
double *resultArray;

//串口中断
void dmaProcessHandler(UART_HandleTypeDef UART, DMA_HandleTypeDef rxDma, uint8_t frameCharLen, uint8_t cacheCharLen)
{
    if(UART.Instance->SR & UART_FLAG_RXNE)//接收到数据
    {
        __HAL_UART_CLEAR_PEFLAG(&UART);
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

extern DMA_HandleTypeDef hdma_usart1_rx;
void initDmaCache(uint8_t *rx1_buf, uint8_t *rx2_buf, uint16_t dma_buf_num)
{
    //enable the DMA transfer for the receiver request
    //使能DMA串口接收
    SET_BIT(huart1.Instance->CR3, USART_CR3_DMAR);

    //enalbe idle interrupt
    //使能空闲中断
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);

    //disable DMA
    //失效DMA
    __HAL_DMA_DISABLE(&hdma_usart1_rx);
    while(hdma_usart1_rx.Instance->CR & DMA_SxCR_EN)
    {
        __HAL_DMA_DISABLE(&hdma_usart1_rx);
    }

    hdma_usart1_rx.Instance->PAR = (uint32_t) & (USART3->DR);
    //memory buffer 1
    //内存缓冲区1
    hdma_usart1_rx.Instance->M0AR = (uint32_t)(rx1_buf);
    //memory buffer 2
    //内存缓冲区2
    hdma_usart1_rx.Instance->M1AR = (uint32_t)(rx2_buf);
    //data length
    //数据长度
    hdma_usart1_rx.Instance->NDTR = dma_buf_num;
    //enable double memory buffer
    //使能双缓冲区
    SET_BIT(hdma_usart1_rx.Instance->CR, DMA_SxCR_DBM);

    //enable DMA
    //使能DMA
    __HAL_DMA_ENABLE(&hdma_usart1_rx);

}