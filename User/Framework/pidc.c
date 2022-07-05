/**MADE BY ME
  ******************************************************************************
  * @author  LIU_Standard
  * @version V1.0.0
  * @date    04-August-2021
  * @brief  
  * 
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

/* Private Includes ----------------------------------------------------------*/
#include "pidc.h"
#include "usart.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private functions --------------------------------------------------------*/
/**
  * @brief 	更新目标值，用于斜坡模式，设置速度环目标速度，逐步增加
  * @param  PID_PramTypeDef* WhichPID, float NowInput
  * @retval None
  */
float PID_UpdateTargetNow(PID_PramTypeDef *WhichPID)
{
	if (WhichPID->ramp_time_count < WhichPID->ramp_target_time)
	{
		++WhichPID->ramp_time_count;
	}
	if (WhichPID->PID_RunState == PID_RAMP)
	{
		WhichPID->ramp_time_count = 0;
		if (WhichPID->target_now < WhichPID->ramp_target) //斜坡函数还没计数完，理论上ramp_target+ 4次ramp_target_step 就是target_now测试 留下余量
		{
			WhichPID->target_now += WhichPID->ramp_target_step; //速度环采用斜坡函数不断更改目标值
			if (WhichPID->target_now >= WhichPID->ramp_target)
			{
				WhichPID->target_now = WhichPID->ramp_target;
				WhichPID->PID_RunState = PID_NORMAL;
			}//培训时找到bug 8/20
		}
		else if (WhichPID->target_now > WhichPID->ramp_target)
		{
			WhichPID->target_now -= WhichPID->ramp_target_step;
				if (WhichPID->target_now <= WhichPID->ramp_target)
				{
					WhichPID->target_now = WhichPID->ramp_target;
					WhichPID->PID_RunState = PID_NORMAL;
				}
		}
		else //斜坡函数计数已经完成，退出斜坡函数模式
		{
			WhichPID->PID_RunState = PID_NORMAL;
		}
	}
	return WhichPID->target_now;
}
/* Functions ------------------------------------------------------------------*/
/**
  * @brief 	PID核心部分，获得输出
  * @param  PID_PramTypeDef* WhichPID, float nowInput
  * @retval float  WhichPID->out_now
  */
float PID_GetOutput(PID_PramTypeDef *WhichPID, float nowInput)
{
	PID_UpdateTargetNow(WhichPID);
	WhichPID->input_now = nowInput;
	WhichPID->err_lastlast = WhichPID->err_last;
	WhichPID->err_last = WhichPID->err_now;
	WhichPID->err_now = WhichPID->target_now - nowInput;
	if (WhichPID->err_now < WhichPID->pid_precision && WhichPID->err_now > -WhichPID->pid_precision)
	{
		WhichPID->err_now = 0; //在设定的精度范围内，认为已经稳定
	}
	if (WhichPID->err_all <= WhichPID->pid_err_max && WhichPID->err_all >= -WhichPID->pid_err_max)
	{
		WhichPID->err_all += WhichPID->err_now;
	}
	else if (WhichPID->err_all > WhichPID->pid_err_max)
	{
		WhichPID->err_all = WhichPID->pid_err_max;
	}
	else if (WhichPID->err_all < -WhichPID->pid_err_max)
	{
		WhichPID->err_all = -WhichPID->pid_err_max; // 防止过度积分饱和
	}
	WhichPID->out_now = WhichPID->Kp * WhichPID->err_now + WhichPID->Kd * (WhichPID->err_now - WhichPID->err_last) + (WhichPID->err_all * WhichPID->Ki);
	if (WhichPID->out_now - WhichPID->out_last >= WhichPID->pid_outstep_max)
	{
		WhichPID->out_now = WhichPID->out_last + WhichPID->pid_outstep_max;
	}
	if (WhichPID->out_now - WhichPID->out_last <= -WhichPID->pid_outstep_max)
	{
		WhichPID->out_now = WhichPID->out_last + -WhichPID->pid_outstep_max; // 微分限幅
	}
	if (WhichPID->out_now >= WhichPID->pid_out_max)
	{
		WhichPID->out_now = WhichPID->pid_out_max;
	}
	if (WhichPID->out_now <= -WhichPID->pid_out_max)
	{
		WhichPID->out_now = -WhichPID->pid_out_max; // 输出限幅
	}
	WhichPID->out_last = WhichPID->out_now;
	return WhichPID->out_now;
}

/**
  * @brief 	使用斜坡模式设定目标值
  * @param  PID_PramTypeDef* WhichPID, float Tartget
  * @retval None
  */
void PID_SetTargetWithRamp(PID_PramTypeDef *WhichPID, float Tartget)
{
	if (WhichPID->ramp_target != Tartget)
	{
		WhichPID->ramp_target = Tartget;
		WhichPID->PID_RunState = PID_RAMP;
	}
}
/**
  * @brief 	使用普通模式设定目标值
  * @param  PID_PramTypeDef* WhichPID, float Tartget
  * @retval None
  */
void PID_SetTargetWithNormal(PID_PramTypeDef *WhichPID, float Tartget)
{
	if (WhichPID->target_now != Tartget)
	{
		WhichPID->target_now = Tartget;
		WhichPID->PID_RunState = PID_NORMAL;
	}
}


/**
  * @brief  清除PID结构体的所有值
  * @param  None
  * @retval None
  */
void PID_Clear(PID_PramTypeDef *WhichPID)
{
	WhichPID->err_now = 0.0;
	WhichPID->err_last = 0.0;
	WhichPID->err_lastlast = 0.0;
	WhichPID->err_all = 0.0;
	WhichPID->out_now = 0.0;
	WhichPID->out_last = 0.0;
	WhichPID->target_now = 0.0;
	WhichPID->input_now = 0.0;
	WhichPID->ramp_target = 0.0;
}

/**
  * @brief 初始化PID为默认参数
  * @param  PID_PramTypeDef * WhichPID
  * @retval None
  */
void PID_DeInit(PID_PramTypeDef *WhichPID)
{
	WhichPID->Kp = 0.0;
	WhichPID->Ki = 0.0;
	WhichPID->Kd = 0.0;
	WhichPID->err_now = 0.0;
	WhichPID->err_last = 0.0;
	WhichPID->err_lastlast = 0.0;
	WhichPID->err_all = 0.0;
	WhichPID->out_now = 0.0;
	WhichPID->out_last = 0.0;
	WhichPID->target_now = 0.0;
	WhichPID->input_now = 0.0;
	WhichPID->PID_RunState = PID_NORMAL;
	WhichPID->ramp_target = 0.0;
	WhichPID->ramp_target_time = RAMP_TIME;
	WhichPID->ramp_target_step = RAMP_STEP;
	WhichPID->pid_precision = 0;
	WhichPID->pid_err_max = 10000;
	WhichPID->pid_out_max = 10000;
	WhichPID->pid_outstep_max = 10000;
}
#if PID_TESTMODE
void PID_SetTarget(float target)
{
	PID_SetTargetWithNormal(&speed_pid_t, target);
}
void PID_NormalLoop(void)
{
	PID_GetOutput(&speed_pid_t, pid_speeds_t.real_speed1);
	MOTORC_SetCurrent_200(speed_pid_t.out_now, 0, 0, 0);
}


void PID_PidDeInit(void)
{
	PID_DeInit(&speed_pid_t);
	speed_pid_t.Kp = PID_DEFAULT_KP;
	speed_pid_t.Ki = PID_DEFAULT_KI;
	speed_pid_t.Kd = PID_DEFAULT_KD;
}

/**
  * @brief  普通PID测试
  * @param  None
  * @retval None
  */
void PID_NormalDebug(void)
{
	int16_t total_time = 0;
	PID_SetTarget(30);
	while (1)
	{
		if (pid_speeds_t.speed_update_times >= 1) //1次打印一次
		{
			total_time += 5;
			pid_speeds_t.speed_update_times = 0;
			//输出单个轮子的目标值、当前速度
			printf("v1,target:%d,%.2f,%.2f\n\r", pid_speeds_t.real_speed1, speed_pid_t.target_now, speed_pid_t.out_now);

			if (total_time >= 3000)
			{
				break;
			}
		}
	}
	total_time = 0;
	PID_SetTarget(0);
	while (1)
	{
		if (pid_speeds_t.speed_update_times >= 1)
		{
			total_time += 5;
			pid_speeds_t.speed_update_times = 0;
			//输出单个轮子的目标值、当前速度
			printf("v1,target:%d,%.2f,%.2f\n\r", pid_speeds_t.real_speed1, speed_pid_t.target_now, speed_pid_t.out_now);

			if (total_time >= 3000)
			{
				break;
			}
		}
	}

	while (1)
		;
}
/**
  * @brief  普通pid值的更新
  * @param  None
  * @retval None
  */
void PID_RealSpeedUpdate(uint16_t wheel)
{
	pid_speeds_t.real_speed1 = wheel;

	++pid_speeds_t.speed_update_times; //调试用
}

#endif
