/**MADE BY ME
  ****************************************************************************** 	
  * @author  LIU_Standard
  * @version V1.0.0
  * @date    2021-7-25
  * @brief   标注pid参数
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PID_CONTROLLER_H
#define __PID_CONTROLLER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f427xx.h"
/* defines --------------------------------------------------------------------*/
/** @defgroup Ramp Group
 *  @brief    斜坡函数状态，步幅管理
 */
#define PID_NORMAL 0
#define PID_RAMP 1
#define RAMP_STEP 30  //每一步增加多少
#define RAMP_TIME 0 //多少次增加一步
/* Exported types -------------------------------------------------------------*/
/** 
  * @brief   PID_Prameter structure definition  
  */
typedef struct
{
	uint8_t PID_RunState;	  // 运行模式，包括 PID_NORMAL PID_RAMP
	uint8_t ramp_time_count;  // 次数记录，与ramp_target_time比较
	uint8_t ramp_target_time; //计数多少次才更新一下
	float Kp;
	float Ki;
	float Kd;
	float err_now;
	float err_last;
	float err_lastlast; //增量式PID需要
	float err_all;
	float input_now;
	float out_now;
	float out_last;
	float target_now;
	float ramp_target;		// 注意
	float ramp_target_step; //斜坡函数步幅
	float pid_precision;	//PID最小精度
	float pid_err_max;		//PID积分限幅，防止积分饱和
	float pid_out_max;		//PID输出限幅
	float pid_out_min;
	float pid_outstep_max;	//PID微分限幅，步幅限制
} PID_PramTypeDef;

/* Functions ------------------------------------------------------------------*/
void PID_NormalDebug(void);
void PID_SetTarget(float target);
void PID_SetTargetWithNormal(PID_PramTypeDef *WhichPID, float Tartget);
void PID_RealSpeedUpdate(uint16_t wheel);
void PID_NormalLoop(void);
void PID_PidDeInit(void);
void PID_DeInit(PID_PramTypeDef *WhichPID);
float PID_GetOutput(PID_PramTypeDef *WhichPID, float nowInput);
float PID_UpdateTargetNow(PID_PramTypeDef *WhichPID);
void PID_Clear(PID_PramTypeDef *WhichPID);
void PID_SetTargetWithRamp(PID_PramTypeDef *WhichPID, float Tartget);

#endif

/*************MADE BY LIU****************END OF FILE****/
