/**
  ******************************************************************************
  * @author         : Liu
  * @file           : ledc.cpp
  * @brief          : led config配置led外设
  * @time           : 2022-2-19
  ******************************************************************************
  * @attention
  * 这是led的外设，与所使用系统无关，通过使用函数指针的方式进行类函数绑定
  *
  *
  ******************************************************************************
  */
/* Private includes ----------------------------------------------------------*/
#include "ledc.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

void (*setLedLevel)(LedC led, uint8_t level);
/* Private user code ---------------------------------------------------------*/
void setLedOn(LedC &led)
{
    led.state = LED_ON;
    setLedLevel(led, led.on_level);
}

void setLedOff(LedC &led)
{
    led.state = LED_OFF;
    setLedLevel(led, !led.on_level);
}

void setLedToggle(LedC &led)
{
    led.state = LED_TOGGLE;
    setLedLevel(led, 3);
}

/**
  * @brief  设置led的状态
  * @param  status: LED_OFF, LED_ON, LED_TOGGLE
  * @retval None
  */
void LedC::setStatus(LED_State_e status) {
    this->state = status;
    switch (status) {
        case LED_ON:
            setLedOn(*this);
            break;
        case LED_OFF:
            setLedOff(*this);
            break;
        case LED_TOGGLE:
            setLedToggle(*this);
            break;
    }
}
