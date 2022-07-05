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
  * @brief 	����Ŀ��ֵ������б��ģʽ�������ٶȻ�Ŀ���ٶȣ�������
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
		if (WhichPID->target_now < WhichPID->ramp_target) //б�º�����û�����꣬������ramp_target+ 4��ramp_target_step ����target_now���� ��������
		{
			WhichPID->target_now += WhichPID->ramp_target_step; //�ٶȻ�����б�º������ϸ���Ŀ��ֵ
			if (WhichPID->target_now >= WhichPID->ramp_target)
			{
				WhichPID->target_now = WhichPID->ramp_target;
				WhichPID->PID_RunState = PID_NORMAL;
			}//��ѵʱ�ҵ�bug 8/20
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
		else //б�º��������Ѿ���ɣ��˳�б�º���ģʽ
		{
			WhichPID->PID_RunState = PID_NORMAL;
		}
	}
	return WhichPID->target_now;
}
/* Functions ------------------------------------------------------------------*/
/**
  * @brief 	PID���Ĳ��֣�������
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
		WhichPID->err_now = 0; //���趨�ľ��ȷ�Χ�ڣ���Ϊ�Ѿ��ȶ�
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
		WhichPID->err_all = -WhichPID->pid_err_max; // ��ֹ���Ȼ��ֱ���
	}
	WhichPID->out_now = WhichPID->Kp * WhichPID->err_now + WhichPID->Kd * (WhichPID->err_now - WhichPID->err_last) + (WhichPID->err_all * WhichPID->Ki);
	if (WhichPID->out_now - WhichPID->out_last >= WhichPID->pid_outstep_max)
	{
		WhichPID->out_now = WhichPID->out_last + WhichPID->pid_outstep_max;
	}
	if (WhichPID->out_now - WhichPID->out_last <= -WhichPID->pid_outstep_max)
	{
		WhichPID->out_now = WhichPID->out_last + -WhichPID->pid_outstep_max; // ΢���޷�
	}
	if (WhichPID->out_now >= WhichPID->pid_out_max)
	{
		WhichPID->out_now = WhichPID->pid_out_max;
	}
	if (WhichPID->out_now <= -WhichPID->pid_out_max)
	{
		WhichPID->out_now = -WhichPID->pid_out_max; // ����޷�
	}
	WhichPID->out_last = WhichPID->out_now;
	return WhichPID->out_now;
}

/**
  * @brief 	ʹ��б��ģʽ�趨Ŀ��ֵ
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
  * @brief 	ʹ����ͨģʽ�趨Ŀ��ֵ
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
  * @brief  ���PID�ṹ�������ֵ
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
  * @brief ��ʼ��PIDΪĬ�ϲ���
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
  * @brief  ��ͨPID����
  * @param  None
  * @retval None
  */
void PID_NormalDebug(void)
{
	int16_t total_time = 0;
	PID_SetTarget(30);
	while (1)
	{
		if (pid_speeds_t.speed_update_times >= 1) //1�δ�ӡһ��
		{
			total_time += 5;
			pid_speeds_t.speed_update_times = 0;
			//����������ӵ�Ŀ��ֵ����ǰ�ٶ�
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
			//����������ӵ�Ŀ��ֵ����ǰ�ٶ�
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
  * @brief  ��ͨpidֵ�ĸ���
  * @param  None
  * @retval None
  */
void PID_RealSpeedUpdate(uint16_t wheel)
{
	pid_speeds_t.real_speed1 = wheel;

	++pid_speeds_t.speed_update_times; //������
}

#endif
