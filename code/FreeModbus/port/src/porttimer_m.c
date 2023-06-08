
/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mb_m.h"
#include "mbport.h"
USHORT temp1;
/* ----------------------- static functions ---------------------------------*/
static void prvvTIMERExpiredISR(struct ModbusSlaveStru *pStu);

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBMasterPortTimersInit(USHORT usTimeOut50us)  //无符号整型
{
	( void )usTimeOut50us;
	return TRUE;
}

void vMBMasterPortTimersEnable(struct ModbusSlaveStru *pStu)
{
	pStu->ModBusMSTimFlag = 1;
	pStu->ModBusMSTimCount = 0;
}

void vMBMasterPortTimersDisable(struct ModbusSlaveStru *pStu)
{
	pStu->ModBusMSTimFlag = 0;
	pStu->ModBusMSTimCount = 0;
}

/********************************************APF Timer Cal**********************************************
	ModBusTimeCal()每进一次时间大约1ms   间隔时间为ModBusMaster_SlaveTimcount
******************************************************************************************************/
void ModBusMasterTimeCal(struct ModbusSlaveStru *pStu)
{
	if(pStu->ModBusMSTimFlag == 1)
	{
		if(pStu->ModBusMSTimCount < pStu->ModBusMSTimOut)
			pStu->ModBusMSTimCount++;
		else
			ModBusMasterTimEnd(pStu);
	}
}
void ModBusMasterTimEnd(struct ModbusSlaveStru *pStu)
{
	prvvTIMERExpiredISR(pStu);
}

/******************************************************************************************************/
void prvvTIMERExpiredISR(struct ModbusSlaveStru *pStu)
{
    (void) pxMBMasterPortCBTimerExpired(pStu);
}



void vMBMasterPortTimersConvertDelayEnable()
{
}

void vMBMasterPortTimersRespondTimeoutEnable()
{
}
