#ifndef STM32F427IIH6A_BOARD_DRV_CAN_H
#define STM32F427IIH6A_BOARD_DRV_CAN_H
#include "cstdint"
#include "stm32f4xx_hal.h"

void CAN_Init(CAN_HandleTypeDef* hcan);
void CAN_FilterInit(void);
void CAN_YAWSendCurrent(int16_t current);
void CAN_PIHSendCurrent(int16_t current);
void CAN_DriverInit();
class drv_can {

};


#endif //STM32F427IIH6A_BOARD_DRV_CAN_H
