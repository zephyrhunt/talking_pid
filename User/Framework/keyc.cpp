//
// Created by LIU on 2022/2/19.
//

#include "keyc.h"
//测试用
#include "usart.h"
#include "keyio.h"
uint8_t (*readKeyLevel)(KeyC key);

/**
  * @brief 按键循环处理，哪个按键需要就把他的loop函数放入循环中 1ms
  * @retval None
  */
void KeyC::handleLoop()
{
    uint8_t current_level = readKeyLevel(*this);
    if ((this->_state) > 0) ++this->ticks;

    //debounce去抖动
    if (current_level != this->_last_level)
    {
        //连续再检测几次
        if (++(this->_debounce_cnt) >= DEBOUNCE_TICKS)
        {
            this->_last_level = current_level;
            this->_debounce_cnt = 0;
        }
    } else
    {
        this->_debounce_cnt = 0;
    }

    // 状态处理
    switch (this->_state)
    {
        case 0: //状态0，现在只有等待按下，否则就是未按下
            if (current_level == this->_trigger_level)
            {
                //开始按下
                usart_printf("PRESS\r\n");
				
                this->key_event = PRESS_DOWN;
//                callBack();
                this->ticks = 0;
                this->_repeat = 1;
                this->_state = 1; //准备下次进循环判断
            }else{
				key_tri_event = NONE_PRESS;
                this->key_event = NONE_PRESS;
            }
            break;

        case 1: //状态1，从状态0来，已经按下，如果弹起了，说明是按下一次
            if (current_level != this->_trigger_level)
            {
                this->key_event = PRESS_DOWN;
//                callBack();
                this->ticks = 0;
                this->_repeat = 1;
                this->_state = 2; //判断是否再次按下
            }else if (this->ticks > LONG_TICKS)
            {
                // 一直在状态1，直到超时，说明是长按
				 usart_printf("LONG_PRESS\r\n");
				key_tri_event = LONG_PRESS_START;
                this->key_event = LONG_PRESS_START; //长按
//                callBack();
                this->_state = 5; // 长按弹起
            }
            break;

        case 2: //状态2，从状态1来，状态1检测按下了一次，判断下次是否触发，是否双击
            if (current_level == this->_trigger_level)
            {
                //如果间隔时间为超过双击间隔时间，记录一次连击
                this->key_event = PRESS_DOWN;
//                callBack();
                ++this->_repeat;
                this->ticks = 0;
                this->_state = 3; //转到状态3，有机会再次回来
            }else if (this->ticks > SHORT_TICKS)
            {
                //超过双击间隔时间，
                if (this->_repeat == 1)
                {
                    // 如果只记录了一次点击，单击
					
                    usart_printf("SINGLE_CLICK\r\n");
					key_tri_event = SINGLE_CLICK;
                    this->key_event = SINGLE_CLICK;
//                    callBack();
                }else if (this->_repeat == 2)
                {
                    usart_printf("DOUBLE_CLICK\r\n");
					key_tri_event = DOUBLE_CLICK;
                    this->key_event = DOUBLE_CLICK;
//                    callBack();
                }
                this->_state = 0; //确定了一次状态，回到初始状态
            }
            break;

        case 3: //状态3，从状态2来，已经记录了一次连击，并且处于按下阶段，判断下次是否弹起
            if (current_level != this->_trigger_level)
            {
                this->key_event = PRESS_UP;
//                callBack();
                if (this->ticks < SHORT_TICKS)
                {
                    // 又是一次短按，回到状态2记录一次连击
                    this->ticks = 0;
                    this->_state = 2;
                } else
                {
                    // 并没有短按，前面至少已经执行了一次singleClick，回到初始状态
                    this->_state = 0;
                }
            }
            break;

        case 5: //状态5，从状态2来，已经记录开始长按，判断是否达到长按不放的状态
            if (current_level == this->_trigger_level)
            {
                usart_printf("PRESS_HOLD\r\n");
                this->key_event = LONG_PRESS_HOLD;
//                callBack();
            } else
            {
                this->key_event = PRESS_UP;
//                callBack();
                this->_state = 0; //否则松开，前面已经触发了一次长按，复位
            }
            break;
    }
//	if(key_tri_event != key_event)
//		key_tri_event = NONE_PRESS; // 每一个循环都情况当前状态
}
KEY_Event_e KeyC::getKeyStatus()
{
	KEY_Event_e tmp;
	tmp = key_tri_event;
//	key_tri_event =  NONE_PRESS;
	return tmp;
}
void KeyC::attachCallBack(KeyCallback cbfun, KEY_Event_e event)
{
//	callBack = cbfun;
}
/**
  * @brief  按键延时卡住，不需要开启任务
  * @retval None
  */
void KeyC::keyDelay() {
    uint8_t key_level = _trigger_level + 10;

    while(key_level != _trigger_level)
    {
        key_level = readKeyLevel(*this);
//        usb_printf("tims\n");
        HAL_Delay(10);
    }

}
