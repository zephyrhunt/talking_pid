/**MADE BY ME
  ****************************************************************************** 	
  * @author  LIU_Standard
  * @version V1.0.0
  * @date    2021-7-25
  * @brief   ��עpid����
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
 *  @brief    б�º���״̬����������
 */
#define PID_NORMAL 0
#define PID_RAMP 1
#define RAMP_STEP 30  //ÿһ�����Ӷ���
#define RAMP_TIME 0 //���ٴ�����һ��
/* Exported types -------------------------------------------------------------*/
/** 
  * @brief   PID_Prameter structure definition  
  */
typedef struct
{
	uint8_t PID_RunState;	  // ����ģʽ������ PID_NORMAL PID_RAMP
	uint8_t ramp_time_count;  // ������¼����ramp_target_time�Ƚ�
	uint8_t ramp_target_time; //�������ٴβŸ���һ��
	float Kp;
	float Ki;
	float Kd;
	float err_now;
	float err_last;
	float err_lastlast; //����ʽPID��Ҫ
	float err_all;
	float input_now;
	float out_now;
	float out_last;
	float target_now;
	float ramp_target;		// ע��
	float ramp_target_step; //б�º�������
	float pid_precision;	//PID��С����
	float pid_err_max;		//PID�����޷�����ֹ���ֱ���
	float pid_out_max;		//PID����޷�
	float pid_out_min;
	float pid_outstep_max;	//PID΢���޷�����������
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
