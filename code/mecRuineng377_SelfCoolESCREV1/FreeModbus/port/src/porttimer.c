//��ʱ�����ò�����غ���,��Ҫ����20KHz��ʱ���ź�
/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
//int ModBusMasterSlaveTimFlag = 0;
//int ModBusMasterSlaveTimCount = 0;
USHORT temp;
/* ----------------------- static functions ---------------------------------*/
void prvvTIMERExpiredISR( struct ModbusSlaveStru *pStu );

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBPortTimersInit( USHORT usTim1Timerout50us )
{
	( void )usTim1Timerout50us;
	return TRUE;
}
void vMBPortTimersEnable(struct ModbusSlaveStru *pStu)
{
	pStu->ModBusMSTimFlag = 1;
	pStu->ModBusMSTimCount = 0;
}

/********************************************APF Timer Cal**********************************************
	ModBusTimeCal()ÿ��һ��ʱ���Լ1ms   ���ʱ��ΪModBusMaster_SlaveTimcount
******************************************************************************************************/
void ModBusSlaveTimeCal(struct ModbusSlaveStru *pStu)
{
	if(pStu->ModBusMSTimFlag == 1)
	{
		if(pStu->ModBusMSTimCount < pStu->ModBusMSTimOut)
			pStu->ModBusMSTimCount++;
		else
			ModBusSlaveTimEnd(pStu);
	}
}
void ModBusSlaveTimEnd(struct ModbusSlaveStru *pStu)
{
	prvvTIMERExpiredISR(pStu);
}
/******************************************************************************************************/
void vMBPortTimersDisable(struct ModbusSlaveStru *pStu)
{
	pStu->ModBusMSTimFlag = 0;
	pStu->ModBusMSTimCount = 0;
}

void prvvTIMERExpiredISR( struct ModbusSlaveStru *pStu )
{
    ( void )pxMBPortCBTimerExpired( pStu );
}
