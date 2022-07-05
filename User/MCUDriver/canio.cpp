//
// Created by LIU on 2022/3/6.
//
/*
 * CAN2: YAW ID5 0x2ff, 底盘通信
 * CAN1: Pitch, 摩擦轮，拨单轮
 *
 */
#include "canio.h"
#include "usbd_cdc_if.h"
/* CAN send and receive ID */
typedef enum
{
    CAN_CHASSIS_ALL_ID = 0x200,
    CAN_3508_M1_ID = 0x201,
    CAN_3508_M2_ID = 0x202,
    CAN_3508_M3_ID = 0x203,
    CAN_3508_M4_ID = 0x204,

    CAN_YAW_MOTOR_ID = 0x209,
    CAN_PIH_MOTOR_ID = 0x206,
    CAN_TRIGGER_MOTOR_ID = 0x207,

    CAN_YAW_SEND_ID   = 0x2FF,
    CAN_PIH_SEND_ID   = 0x1FF,
    CAN_SHOOT_SEND_ID = 0x200,
    CAN_GIMBAL_ALL_ID = 0x1FF,
    CAN_CHASSIS_SEND_ID = 0x401,
    CAN_ANGLE_SEND_ID = 0x402,

    CAN_JUDGE_REC_ID = 0x405
} CAN_Msg_e;

extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;

void CAN_FilterInit(void)
{
    CAN_FilterTypeDef can_filter_st;
    can_filter_st.FilterBank = 0;
    can_filter_st.FilterActivation = ENABLE;
//    can_filter_st.FilterBank =
    can_filter_st.FilterMode = CAN_FILTERMODE_IDMASK;
    can_filter_st.FilterScale = CAN_FILTERSCALE_32BIT;
    can_filter_st.FilterIdHigh = 0x0000;
    can_filter_st.FilterIdLow = 0x0000;
    can_filter_st.FilterMaskIdHigh = 0x0000;
    can_filter_st.FilterMaskIdLow = 0x0000;
    can_filter_st.FilterFIFOAssignment = CAN_RX_FIFO0;
    HAL_CAN_ConfigFilter(&hcan1, &can_filter_st);
    HAL_CAN_Start(&hcan1);

    can_filter_st.FilterBank = 14;
//    can_filter_st.SlaveStartFilterBank = 14;
    HAL_CAN_ConfigFilter(&hcan2, &can_filter_st);
    HAL_CAN_Start(&hcan2);
}

void CAN_Init(CAN_HandleTypeDef* hcan)
{
    assert_param(hcan != NULL);
    HAL_CAN_Start(hcan);
    __HAL_CAN_ENABLE_IT(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
    //注意使能中断
    HAL_CAN_ActivateNotification(hcan,CAN_IT_RX_FIFO0_MSG_PENDING);
}

/**
  * @brief  设置YAW电机，CAN2 ID5
  *         ID1234是底盘的四个电机
  * @param  current: 目标电流
  * @param
  * @retval None
  */
void CAN_YAWSendCurrent(int16_t current)
{
    CAN_TxHeaderTypeDef tx_msg;
    uint32_t send_mail_box = 0;
    uint8_t send_data[8];

//    if( HAL_CAN_GetTxMailboxesFreeLevel( &hcan2 )){
        tx_msg.StdId = CAN_YAW_SEND_ID;
        tx_msg.IDE = CAN_ID_STD;
        tx_msg.RTR = CAN_RTR_DATA;
        tx_msg.DLC = 8;
        send_data[0] = (int8_t )(current >> 8);
        send_data[1] = (int8_t )current & 0xff;
        HAL_CAN_AddTxMessage(&hcan2, &tx_msg, send_data, &send_mail_box);
}

/**
  * @brief  设置PITCH电机，CAN1 ID2
  * @param  current: 目标电流
  * @param
  * @retval None
  */
void CAN_PIHSendCurrent(int16_t current)
{
    CAN_TxHeaderTypeDef tx_msg;
    uint32_t send_mail_box = 0;
    uint8_t send_data[8];
    tx_msg.StdId = CAN_PIH_SEND_ID;
    tx_msg.IDE = CAN_ID_STD;
    tx_msg.RTR = CAN_RTR_DATA;
    tx_msg.DLC = 0x04;
    send_data[0] = 0;
    send_data[1] = 0;
    send_data[2] = (current >> 8);
    send_data[3] = current & 0xff;
    HAL_CAN_AddTxMessage(&hcan1, &tx_msg, send_data, &send_mail_box);
}

void CAN_ChasssicSendVel(float vx_100, float vy_100, float vz_10, int16_t mode)
{
    int16_t send_vx, send_vy, send_vz, send_mode = 0;
//    static int16_t last_send_vx, last_send_vy, last_send_vz, last_send_mode = 0;
    CAN_TxHeaderTypeDef tx_msg;
    uint32_t send_mail_box;
    uint8_t send_data[8];
    send_vx = vx_100 * 100;
    send_vy = vy_100 * 100;
    send_mode = mode;
    send_vz = vz_10 * 10;

//    if (send_vx != last_send_vx || send_vy != last_send_vy || last_send_vz != last_send_vz || last_send_mode != send_mode)
    {
        tx_msg.StdId = CAN_CHASSIS_SEND_ID;
        tx_msg.IDE = CAN_ID_STD;
        tx_msg.RTR = CAN_RTR_DATA;
        tx_msg.DLC = 0x08;
        send_data[0] = (send_vx >> 8);
        send_data[1] = send_vx & 0xff;
        send_data[2] = (send_vy >> 8);
        send_data[3] = send_vy & 0xff;
        send_data[4] = (send_vz >> 8);
        send_data[5] = send_vz & 0xff;
        send_data[6] = (send_mode >> 8);
        send_data[7] = send_mode & 0xff;
        HAL_CAN_AddTxMessage(&hcan2, &tx_msg, send_data, &send_mail_box);
    }
}

void CAN_ChasssicSendAngle(float m_angle_5, float imu_angle_5)
{
    int16_t send_mangle, send_imuangle;
    static int16_t last_send_mangle, last_send_imuangle;
    CAN_TxHeaderTypeDef tx_msg;
    uint32_t send_mail_box;
    uint8_t send_data[8];
    send_mangle = m_angle_5;
    send_imuangle= imu_angle_5;
//    if (send_mangle != last_send_mangle || send_imuangle != last_send_imuangle)
    {
        tx_msg.StdId = CAN_ANGLE_SEND_ID;
        tx_msg.IDE = CAN_ID_STD;
        tx_msg.RTR = CAN_RTR_DATA;
        tx_msg.DLC = 0x04;
        send_data[0] = (send_mangle >> 8);
        send_data[1] = send_mangle & 0xff;
        send_data[2] = (send_imuangle >> 8);
        send_data[3] = send_imuangle & 0xff;
        //TODO:不确定是不是can2
        HAL_CAN_AddTxMessage(&hcan2, &tx_msg, send_data, &send_mail_box);
    }
    last_send_mangle= send_mangle;
    last_send_imuangle = send_imuangle;
}

void portCANSendStatus(float m_yaw, float m_pih, uint8_t isRedrawing, uint8_t servo_status, uint8_t is_zimiao, uint8_t status___)
{
	int16_t send_yaw, send_pih, send_imuangle;
	static int16_t last_send_mangle, last_send_imuangle;
	CAN_TxHeaderTypeDef tx_msg;
	uint32_t send_mail_box;
	uint8_t send_data[8];
	send_yaw = (int16_t )m_yaw;
	send_pih = (int16_t )m_pih;
	tx_msg.StdId = CAN_ANGLE_SEND_ID;
	tx_msg.IDE = CAN_ID_STD;
	tx_msg.RTR = CAN_RTR_DATA;
	tx_msg.DLC = 0x08;
	send_data[0] = (send_yaw >> 8);
	send_data[1] = send_yaw & 0xff;
	send_data[2] = (send_pih >> 8);
	send_data[3] = send_pih & 0xff;
	send_data[4] = isRedrawing;
	send_data[5] = servo_status;
	send_data[6] = is_zimiao;
	send_data[7] = status___;
	HAL_CAN_AddTxMessage(&hcan2, &tx_msg, send_data, &send_mail_box);
}


/**
  * @brief  设置摩擦轮，拨单轮，CAN1 ID1
  * @param  current: 目标电流
  * @param
  * @retval None
  */
void CAN_ShootSendCurrent(int16_t friLc, int16_t friRc, int16_t ramc)
{
    CAN_TxHeaderTypeDef tx_msg;
    uint32_t send_mail_box;
    uint8_t send_data[8];
    tx_msg.StdId = CAN_SHOOT_SEND_ID;
    tx_msg.IDE = CAN_ID_STD;
    tx_msg.RTR = CAN_RTR_DATA;
    tx_msg.DLC = 0x06;
    send_data[0] = (friLc >> 8);
    send_data[1] = friLc;
    send_data[2] = (friRc >> 8);
    send_data[3] = friRc;
    send_data[4] = (ramc >> 8);
    send_data[5] = ramc;
    HAL_CAN_AddTxMessage(&hcan1, &tx_msg, send_data, &send_mail_box);
}


/*HAL库FIFO0中断*/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    /*!< CAN receive buffer */
    CAN_RxHeaderTypeDef rx_header;
    uint8_t rx_data[8];
    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_data);
    int16_t ID = rx_header.StdId;
    int16_t pos, vel, current, tmp;
    uint16_t cool_rate, cool_limit, shoot_spd_max;
    uint8_t color, smallOrBig;
    /* Switch to user call back function. */
    if(hcan== &hcan1)
    {
        
    }

    else if(hcan == &hcan2)
    {
        
    }
}

void CAN_DriverInit()
{
    CAN_Init(&hcan1);
    CAN_Init(&hcan2);
    CAN_FilterInit();
//    setYawCurrent = CAN_YAWSendCurrent;
//    setPihCurrent = CAN_PIHSendCurrent;
//    setShootCurrent = CAN_ShootSendCurrent;
//    sendChassisVel = CAN_ChasssicSendVel;
//    sendChassisYaw = CAN_ChasssicSendAngle;
//	sendUpdateCmd = portCANSendStatus;
}


