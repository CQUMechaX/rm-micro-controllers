#include "tool/transimition.hpp"

#include <cmsis_os.h>
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_can.h>
#include <stm32f4xx_hal_uart.h>

#include <cstring>

#include "app/device_monitor.hpp"
#include "app/microros_param.h"
#include "override.h"
#include "usart.h"

static double gSafeArray[2];

uint8_t gLegacyCacheArray[2][LEGACY_CACHE_BYTE_LEN], gDbusCacheArray[2][DBUS_CACHE_BYTE_LEN];
CacheBuffer<1000> gAcmCacheArray[2];
double * gLegacyResultArray = gSafeArray;

const uint32_t gCanHeadTarget[17] = {0,     0x200, 0x200, 0x200, 0x200, 0x1FF, 0x1FF, 0x1FF, 0x1FF,
                                     0x2FF, 0x2FF, 0x2FF, 0x000, 0x000, 0x000, 0x000, 0},
               gCanHeadFeedback[17] = {0,     0x201, 0x202, 0x203, 0x204, 0x205,
                                       0x206, 0x207, 0x208, 0x209, 0x20A, 0x20B,
                                       0x000, 0x000, 0x000, 0x000, 0};

static bool jointUpdate(
  std::vector<DeviceStatus> & device_list, uint32_t std_id, uint8_t * rx_data);
static bool dbusUpdate(uint8_t * rx_data);

HAL_StatusTypeDef transimitionIdleHandler(
  UART_HandleTypeDef * huart, uint16_t rx_byte_len, uint16_t buffer_byte_len,
  uint16_t frame_byte_len, HAL_StatusTypeDef (*callback_function)(bool))
{
  DMA_HandleTypeDef *& hdmarx = huart->hdmarx;
  HAL_StatusTypeDef ret_val;

  //__HAL_DMA_DISABLE(hdmarx); // disable DMA in UARTEx_RxEventCallback
  hdmarx->Instance->NDTR = buffer_byte_len; /** Reset buffer_data_length before DMA_ENABLE */

  if (hdmarx->Instance->CR & DMA_SxCR_CT) {  // used is 1, changed to 0
    hdmarx->Instance->CR ^= DMA_SxCR_CT;
    __HAL_DMA_ENABLE(hdmarx);  // enable DMA
    ret_val = ((rx_byte_len ^ frame_byte_len) ? HAL_ERROR : callback_function(1));
  } else {  // used is 0, changed to 1 (== RESET)
    hdmarx->Instance->CR |= DMA_SxCR_CT;
    __HAL_DMA_ENABLE(hdmarx);  // enable DMA
    ret_val = ((rx_byte_len ^ frame_byte_len) ? HAL_ERROR : callback_function(0));
  }

  __HAL_UART_CLEAR_IDLEFLAG(huart);
  ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_IDLEIE); /** Enable idle interrupt. */
  huart->ReceptionType = HAL_UART_RECEPTION_TOIDLE; /** Wait for next IDLE event or full buffer.*/
  return ret_val;
}

bool transimitionDmaInit(
  UART_HandleTypeDef * huart, uint8_t * rx1_buf, uint8_t * rx2_buf, uint16_t buffer_byte_len)
{
  DMA_HandleTypeDef *& hdmarx = huart->hdmarx;

  //disable DMA
  while (hdmarx->Instance->CR & DMA_SxCR_EN) {
    __HAL_DMA_DISABLE(hdmarx);
  }

  //enable double memory buffer
  SET_BIT(hdmarx->Instance->CR, DMA_SxCR_DBM);
  //memory buffer
  hdmarx->Instance->PAR = reinterpret_cast<uint32_t>(&(huart->Instance->DR));
  hdmarx->Instance->M0AR = reinterpret_cast<uint32_t>(rx1_buf);
  hdmarx->Instance->M1AR = reinterpret_cast<uint32_t>(rx2_buf);
  //data length
  hdmarx->Instance->NDTR = buffer_byte_len;

  __HAL_UART_CLEAR_IDLEFLAG(huart);
  __HAL_UART_DISABLE_IT(huart, UART_IT_RXNE);
  ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_IDLEIE); /** Enable idle interrupt. */
  huart->RxXferSize = buffer_byte_len;
  huart->ReceptionType = HAL_UART_RECEPTION_TOIDLE; /** Like HAL_UARTEx_ReceiveToIdle_DMA,
        but omit its M0AR and error handler configs in UART_Start_Receive_DMA.*/

  SET_BIT(
    huart->Instance->CR3, USART_CR3_DMAR); /** Enable the DMA transfer for the receiver request */
  __HAL_DMA_ENABLE(hdmarx);                /** Enable DMA */

  return true;
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

  /*can_filter_st.SlaveStartFilterBank = 14;
    can_filter_st.FilterBank = 14;
    HAL_CAN_ConfigFilter(&HCAN2, &can_filter_st);
    HAL_CAN_Start(&HCAN2);
    HAL_CAN_ActivateNotification(&HCAN2, CAN_IT_RX_FIFO0_MSG_PENDING);*/

  return HAL_OK;
}

HAL_StatusTypeDef transimitionLegacyRx(bool section)
{
  gLegacyResultArray = (double *)gLegacyCacheArray[section];
  return HAL_OK;
}

HAL_StatusTypeDef transimitionDbusRx(bool section)
{
  return dbusUpdate(gDbusCacheArray[section]) ? HAL_OK : HAL_ERROR;
}

bool transimitionUsbCdcRx(uint8_t * buf, uint32_t len, uint8_t index)
{
  return gAcmCacheArray[index].write(buf, len);
}

static bool jointUpdate(std::vector<DeviceStatus> & device_list, uint32_t std_id, uint8_t * rx_data)
{
  for (DeviceStatus & joint_device : device_list) {
    // JointData *& joint = *iter_joint_device.ptr.joint;
    if (joint_device.ptr.joint->head_feedback == std_id) {
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

static bool dbusUpdate(uint8_t * rx_data)
{
  DbusData & rc_ctrl = gDeviceMonitor.device_dbus_.data.dbus;
  rc_ctrl.rc.channel[0] = (rx_data[0] | (rx_data[1] << 8)) & 0x07FF;         //!< Channel 0
  rc_ctrl.rc.channel[1] = ((rx_data[1] >> 3) | (rx_data[2] << 5)) & 0x07FF;  //!< Channel 1
  rc_ctrl.rc.channel[2] = ((rx_data[2] >> 6) | (rx_data[3] << 2) |           //!< Channel 2
                           (rx_data[4] << 10)) &
                          0x07FF;
  rc_ctrl.rc.channel[3] = ((rx_data[4] >> 1) | (rx_data[5] << 7)) & 0x07FF;  //!< Channel 3
  rc_ctrl.rc.channel[0] -= RC_CH_VALUE_OFFSET;
  rc_ctrl.rc.channel[1] -= RC_CH_VALUE_OFFSET;
  rc_ctrl.rc.channel[2] -= RC_CH_VALUE_OFFSET;
  rc_ctrl.rc.channel[3] -= RC_CH_VALUE_OFFSET;
  rc_ctrl.rc.button[0] = ((rx_data[5] >> 4) & 0x0003);       //!< Switch left
  rc_ctrl.rc.button[1] = ((rx_data[5] >> 4) & 0x000C) >> 2;  //!< Switch right
  if (
    abs(rc_ctrl.rc.channel[0]) > RC_CHANNEL_VALUE_ERROR ||
    abs(rc_ctrl.rc.channel[1]) > RC_CHANNEL_VALUE_ERROR ||
    abs(rc_ctrl.rc.channel[2]) > RC_CHANNEL_VALUE_ERROR ||
    abs(rc_ctrl.rc.channel[3]) > RC_CHANNEL_VALUE_ERROR || !rc_ctrl.rc.button[0] ||
    !rc_ctrl.rc.button[1]) {
    memset(&rc_ctrl, 0, sizeof(rc_ctrl));
    gDeviceMonitor.update_single_isr(gDeviceMonitor.device_dbus_, DeviceErrorList::kDataMismatch);
    return false;
  }

  rc_ctrl.mouse.x = rx_data[6] | (rx_data[7] << 8);          //!< Mouse X axis
  rc_ctrl.mouse.y = rx_data[8] | (rx_data[9] << 8);          //!< Mouse Y axis
  rc_ctrl.mouse.z = rx_data[10] | (rx_data[11] << 8);        //!< Mouse Z axis
  rc_ctrl.mouse.press_l = rx_data[12];                       //!< Mouse Left Is Press ?
  rc_ctrl.mouse.press_r = rx_data[13];                       //!< Mouse Right Is Press ?
  rc_ctrl.key.v = rx_data[14] | (rx_data[15] << 8);          //!< KeyBoard value
  rc_ctrl.rc.channel[4] = rx_data[16] | (rx_data[17] << 8);  //NULL
  rc_ctrl.rc.channel[4] -= RC_CH_VALUE_OFFSET;
  // rc_ctrl.rc.channel[4] -= RC_CH_VALUE_OFFSET;

  gDeviceMonitor.update_single_isr(gDeviceMonitor.device_dbus_);
  return true;
}

HAL_StatusTypeDef transimitionCanTx(
  CAN_HandleTypeDef * hcan, uint32_t std_id, int16_t motor1, int16_t motor2, int16_t motor3,
  int16_t motor4)
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

  while (HAL_CAN_AddTxMessage(hcan, &tx_message, can_send_data, send_mail_box) == HAL_ERROR) {
    hcan->ErrorCode = 0;
  }
  return HAL_OK;
}

uint32_t transimitionUsbCdcReadBlock(uint8_t * buf, uint32_t len, uint8_t index, uint32_t time_out)
{
  // static uint32_t time_stamp;
  return gAcmCacheArray[index].read(buf, len, [time_out]() -> bool {
    static uint32_t stamp = xTaskGetTickCount() + time_out;
    uint32_t now = xTaskGetTickCount();
    return stamp > now;
  });
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef * hcan)
{
  CAN_RxHeaderTypeDef rx_header;
  uint8_t rx_data[8], can_num;

  HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_data);
  if ((can_num = gDeviceMonitor.can_cast_to_num(hcan))) {
    jointUpdate(gDeviceMonitor.device_joint_[can_num], rx_header.StdId, rx_data);
  } else {
    freertosErrorHandler(__FILE__, __LINE__);
  }
  return;
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef * huart, uint16_t size)
{
  __HAL_DMA_DISABLE(
    huart
      ->hdmarx);  // Disable DMA, as in CIRCULAR mode, DMA xfer and HAL UART Rx process is not ended. (L2514)
  if (huart == &HUART_LEGACY) {
    transimitionIdleHandler(
      huart, size, LEGACY_CACHE_BYTE_LEN, LEGACY_FRAME_BYTE_LEN, transimitionLegacyRx);
  } else if (huart == &HUART_DBUS) {
    transimitionIdleHandler(
      huart, size, DBUS_CACHE_BYTE_LEN, DBUS_FRAME_BYTE_LEN, transimitionDbusRx);
  }
}