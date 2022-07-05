/**
  ******************************************************************************
  * @author         : Liu
  * @file           : keyc.h
  * @brief          : Header for key.cpp file.
  *                   按键控制，包括单击，连击，长按，以及反馈函数
  * @time           :2022-2-19
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __KEYC_H
#define __KEYC_H

/* Includes ------------------------------------------------------------------*/
#include "cstdint"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"

/* Exported defines -----------------------------------------------------------*/
#define KEY_Port_IO         GPIO_TypeDef*
#define KEY_Port_Pin        uint16_t

#define TICKS_INTERVAL    5	//ms
#define DEBOUNCE_TICKS    3	//MAX 8, 双击间隔时间
#define SHORT_TICKS       (300 /TICKS_INTERVAL)
#define LONG_TICKS        (1000 /TICKS_INTERVAL)

/* Exported types ------------------------------------------------------------*/
typedef void (*KeyCallback)(void*);
typedef enum {
    PRESS_DOWN = 0, //按键每次按下触发
    PRESS_UP,       //按键每次松开触发
    PRESS_REPEAT,   //重复按下触发，repeat记录连击次数
    SINGLE_CLICK,   //单击按键事件
    DOUBLE_CLICK,   //双击按键事件
    LONG_PRESS_START, //长按触发
    LONG_PRESS_HOLD,  //长按期间一直触发
    number_of_event,
    NONE_PRESS
}KEY_Event_e;

class KeyC {
public:
    uint64_t ticks;
    KEY_Event_e key_event;

    KEY_Port_IO key_port;
    KEY_Port_Pin key_pin;

    KeyC(KEY_Port_IO key_Io, KEY_Port_Pin key_Pin, uint8_t trigger_level = 0):
            key_port(key_Io), key_pin(key_Pin), _trigger_level(trigger_level){}

    void handleLoop();
    void attachCallBack(KeyCallback cbfun, KEY_Event_e event);
    void keyTicks();
    void keyDelay();

    KeyCallback callBack();
    uint32_t last_time = 0; //持续时间
private:

    uint8_t _state = 0;
    uint8_t _last_state = (uint8_t)NONE_PRESS;
    uint8_t _repeat = 0;
    uint8_t _trigger_level;
    uint8_t _last_level = 0;
    uint8_t _debounce_cnt = 0;
};

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/

/* Exported functions  --------------------------------------------------------*/
extern uint8_t (*readKeyLevel)(KeyC key);

#endif

