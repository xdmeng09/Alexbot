#include "can_bsp.h"
#include "fdcan.h"

/**
************************************************************************
* @brief:      	can_bsp_init(void)
* @param:       void
* @retval:     	void
* @details:    	CAN ʹ��
************************************************************************
**/
void can_bsp_init(void)
{ 
	can_filter_init();
	HAL_Delay(20);
	HAL_FDCAN_Start(&hfdcan1);
  HAL_Delay(20);  
	HAL_FDCAN_Start(&hfdcan2);
  HAL_Delay(20);	
	HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE,0);
	HAL_Delay(20);
	HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE,0);
}

/**
************************************************************************
* @brief:      	can_filter_init(void)
* @param:       void
* @retval:     	void
* @details:    	CAN�˲�����ʼ��
************************************************************************
**/
void can_filter_init(void)
{
	FDCAN_FilterTypeDef fdcan_filter;
	
	fdcan_filter.IdType = FDCAN_STANDARD_ID;                       //��׼ID
	fdcan_filter.FilterIndex = 0;                                  //�˲�������                   
	fdcan_filter.FilterType = FDCAN_FILTER_RANGE;                  //�˲�������
	fdcan_filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;           //������0������FIFO0  
	fdcan_filter.FilterID1 = 0x0000;                               //32λID
	fdcan_filter.FilterID2 = 0x0000;                               //���FDCAN����Ϊ��ͳģʽ�Ļ���������32λ����
	if(HAL_FDCAN_ConfigFilter(&hfdcan1,&fdcan_filter)!=HAL_OK) 		 //�˲�����ʼ��
	{
		Error_Handler();
	}
	HAL_FDCAN_ConfigFifoWatermark(&hfdcan1, FDCAN_CFG_RX_FIFO0, 1);
}
/**
************************************************************************
* @brief:      	canx_bsp_send_data(FDCAN_HandleTypeDef *hfdcan, uint16_t id, uint8_t *data, uint32_t len)
* @param:       hcan: CAN���
* @param:       id: 	CAN�豸ID
* @param:       data: ���͵�����
* @param:       len:  ���͵����ݳ���
* @retval:     	void
* @details:    	��������
************************************************************************
**/

uint8_t canx_bsp_send_data(FDCAN_HandleTypeDef *hcan, uint16_t id, uint8_t *data, uint32_t len)
{
	FDCAN_TxHeaderTypeDef TxHeader;
	
  TxHeader.Identifier = id;
  TxHeader.IdType = FDCAN_STANDARD_ID;																// ��׼ID 
  TxHeader.TxFrameType = FDCAN_DATA_FRAME;														// ����֡ 
  TxHeader.DataLength = len << 16;																		// �������ݳ��� 
  TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;										// ���ô���״ָ̬ʾ 								
  TxHeader.BitRateSwitch = FDCAN_BRS_OFF;															// �������ɱ䲨���� 
  TxHeader.FDFormat = FDCAN_CLASSIC_CAN;															// ��ͨCAN��ʽ 
  TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;										// ���ڷ����¼�FIFO����, ���洢 
  TxHeader.MessageMarker = 0x00; 			// ���ڸ��Ƶ�TX EVENT FIFO����ϢMaker��ʶ����Ϣ״̬����Χ0��0xFF                
    
  if(HAL_FDCAN_AddMessageToTxFifoQ(hcan, &TxHeader, data)!=HAL_OK) 
		return 1;//����
	return 0;	
}
/**
************************************************************************
* @brief:      	canx_bsp_receive(CAN_HandleTypeDef *hcan, uint8_t *buf)
* @param:       hcan: CAN���
* @param[out]:  rec_id: 	���յ����ݵ�CAN�豸ID
* @param:       buf���������ݻ���
* @retval:     	���յ����ݳ���
* @details:    	��������
************************************************************************
**/
uint8_t fdcanx_receive(FDCAN_HandleTypeDef *hfdcan, uint8_t *buf)
{	
	FDCAN_RxHeaderTypeDef fdcan_RxHeader;
  if(HAL_FDCAN_GetRxMessage(hfdcan,FDCAN_RX_FIFO0, &fdcan_RxHeader, buf)!=HAL_OK)
		return 0;//��������
  return fdcan_RxHeader.DataLength>>16;	
}
/**
************************************************************************
* @brief:      	HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
* @param:       hfdcan��FDCAN���
* @param:       RxFifo0ITs���жϱ�־λ
* @retval:     	void
* @details:    	HAL���CAN�жϻص�����
************************************************************************
**/
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
  if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET)
  {
		if(hfdcan == &hfdcan1)
		{
			fdcan1_rx_callback();
		}
		if(hfdcan == &hfdcan2)
		{
			fdcan2_rx_callback();
		}
	}
}

/**
************************************************************************
* @brief:      	can1_rx_callback(void)
* @param:       void
* @retval:     	void
* @details:    	���û����õĽ���������
************************************************************************
**/
uint8_t rx_data1[8] = {0};
void fdcan1_rx_callback(void)
{
	fdcanx_receive(&hfdcan1, rx_data1);
}
/**
************************************************************************
* @brief:      	can2_rx_callback(void)
* @param:       void
* @retval:     	void
* @details:    	���û����õĽ���������
************************************************************************
**/
__weak void fdcan2_rx_callback(void)
{

}
