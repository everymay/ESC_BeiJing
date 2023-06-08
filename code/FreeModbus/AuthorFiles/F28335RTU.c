/*
 * F28335RTU.c
 *
 *  Created on: 2018年1月30日
 *      Author: Xu-yulin
 */
/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"
/* ----------------------- Platform includes --------------------------------*/
#include "port.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mb_m.h"
#include "mbrtu.h"
#include "mbframe.h"
#include "mbcrc.h"
#include "mbport.h"
#include "MarcoDefine.h"

#if 0
#pragma	CODE_SECTION(xMBRTU335TransmitFSM ,"ram2func")
#pragma	CODE_SECTION(xMBRTU335ReceiveFSM ,"ram2func")
//#pragma	CODE_SECTION(xMBMasterRTU335ReceiveFSM ,"ram2func")
//#pragma	CODE_SECTION(xMBRTU335ReceiveFSM ,"ram2func")
/****************************************************************************************************/
#define MB_SER_PDU_SIZE_MIN     4       /*!< Minimum size of a Modbus RTU frame. */
#define MB_SER_PDU_SIZE_MAX     256     /*!< Maximum size of a Modbus RTU frame. */
#define MB_SER_PDU_SIZE_CRC     2       /*!< Size of CRC field in PDU. */
#define MB_SER_PDU_ADDR_OFF     0       /*!< Offset of slave address in Ser-PDU. */
#define MB_SER_PDU_PDU_OFF      1       /*!< Offset of Modbus-PDU in Ser-PDU. */
static volatile BOOL   xFrameIsBroadcast = FALSE;
static volatile USHORT usMasterSendPDULength;
static volatile eMBMasterTimerMode eMasterCurTimerMode;
/****************************************从机代码接口函数*********************************************/
#define TX_HEAD_PACKAGE55AA()		{pStu->pucSndBufferCur[0] = 0x55;\
			pStu->pucSndBufferCur[1] = 0xAA;\
			pStu->pucSndBufferCur[2] = 0xFF;\
			pStu->pucSndBufferCur[3] = 0x00;}

#define TX_HEAD_PACKAGEAA55()		{pStu->pucSndBufferCur[0] = 0xAA;\
			pStu->pucSndBufferCur[1] = 0x55;\
			pStu->pucSndBufferCur[2] = 0x00;\
			pStu->pucSndBufferCur[3] = 0xFF;}

eMBErrorCode
eMBRTU335Receive(struct ModbusSlaveStru *pStu, Uint16 * pucRcvAddress, UCHAR ** pucFrame, USHORT * pusLength )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    ENTER_CRITICAL_SECTION();
//    assert( usRcvBufferPos < MB_SER_PDU_SIZE_MAX );
    /* Length and CRC check */
    if( ( pStu->usRcvBufferPos >= MB_SER_PDU_SIZE_MIN )
        && ( usMBCRC16( (Uint16*)pStu->ModbusRcvSedBuff, pStu->usRcvBufferPos ) == 0 ) )
    {
          *pucRcvAddress = pStu->ModbusRcvSedBuff[MB_SER_PDU_ADDR_OFF];
    }
    else
    {
        eStatus = MB_EIO;
    }
    EXIT_CRITICAL_SECTION( );
    return eStatus;
}

eMBErrorCode
APFeMBRTU335Receive(struct ModbusSlaveStru *pStu, Uint16 * pucRcvAddress, UCHAR ** pucFrame, USHORT * pusLength )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    ENTER_CRITICAL_SECTION();
//    assert( usRcvBufferPos < MB_SER_PDU_SIZE_MAX );
    /* Length and CRC check */
    if( ( pStu->usRcvBufferPos >= MB_SER_PDU_SIZE_MIN )
        && ( usMBCRC16( (Uint16*)pStu->ModbusRcvSedBuff, pStu->usRcvBufferPos ) == 0 ) )
    {
          *pucRcvAddress = pStu->ModbusRcvSedBuff[4];

    }
    else
    {
        eStatus = MB_EIO;
    }
    EXIT_CRITICAL_SECTION( );
    return eStatus;
}


eMBErrorCode
eMBRTU335Send( struct ModbusSlaveStru *pStu,UCHAR ucSlaveAddress, const UCHAR * pucFrame, USHORT usLength )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    USHORT          usCRC16;
    ENTER_CRITICAL_SECTION();

    /* Check if the receiver is still in idle state. If not we where to
     * slow with processing the received frame and the master sent another
     * frame on the network. We have to abort sending the frame.
     */
    if( pStu->eMBRcvState == STATE_RX_IDLE )
    {
        /* First byte before the Modbus-PDU is the slave address. */
    	pStu->pucSndBufferCur = ( UCHAR * ) pucFrame - 1;
        pStu->usSndBufferCount = 1;
        /* Now copy the Modbus-PDU into the Modbus-Serial-Line-PDU. */
        pStu->pucSndBufferCur[MB_SER_PDU_ADDR_OFF] = ucSlaveAddress;
        pStu->usSndBufferCount += usLength;
        /* Calculate CRC16 checksum for Modbus-Serial-Line-PDU. */
        usCRC16 = usMBCRC16( ( UCHAR * ) (pStu->pucSndBufferCur), pStu->usSndBufferCount );
        pStu->ModbusRcvSedBuff[pStu->usSndBufferCount++] = ( UCHAR )( usCRC16 & 0xFF );
        pStu->ModbusRcvSedBuff[pStu->usSndBufferCount++] = ( UCHAR )( usCRC16 >> 8 );
        pStu->eMBSndState = STATE_TX_XMIT;
        vMBPortSerialEnable(pStu, FALSE, TRUE );
        pStu->pReg->SCIFFTX.bit.TXFIFOXRESET = 1;					//复位发送缓冲区
        pStu->pReg->SCIFFTX.bit.TXFFINTCLR = 1;					//复位发送中断
        pStu->pReg->SCIFFTX.bit.TXFFIENA = 1;						//开中断
    }
    else
    {
        eStatus = MB_EIO;
    }
    EXIT_CRITICAL_SECTION();
    return eStatus;
}

eMBErrorCode
APFeMBRTU335Send( struct ModbusSlaveStru *pStu,UCHAR ucSlaveAddress, const UCHAR * pucFrame, USHORT usLength )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    USHORT          usCRC16;
    ENTER_CRITICAL_SECTION();

    /* Check if the receiver is still in idle state. If not we where to
     * slow with processing the received frame and the master sent another
     * frame on the network. We have to abort sending the frame.
     */
    if( pStu->eMBRcvState == STATE_RX_IDLE )
    {
    	pStu->pucSndBufferCur = ( UCHAR * ) pucFrame - 5;
		pStu->usSndBufferCount = 5;
		if(pucFrame[0]>=0x64)		TX_HEAD_PACKAGE55AA()
		else						TX_HEAD_PACKAGEAA55()
		pStu->pucSndBufferCur[4] = ucSlaveAddress;
        pStu->usSndBufferCount += usLength;
        /* Calculate CRC16 checksum for Modbus-Serial-Line-PDU. */
        usCRC16 = usMBCRC16( ( UCHAR * ) (pStu->pucSndBufferCur), pStu->usSndBufferCount );
        pStu->ModbusRcvSedBuff[pStu->usSndBufferCount++] = ( UCHAR )( usCRC16 & 0xFF );
        pStu->ModbusRcvSedBuff[pStu->usSndBufferCount++] = ( UCHAR )( usCRC16 >> 8 );
        pStu->eMBSndState = STATE_TX_XMIT;
        vMBPortSerialEnable(pStu, FALSE, TRUE );
        pStu->pReg->SCIFFTX.bit.TXFIFOXRESET = 1;					//复位发送缓冲区
        pStu->pReg->SCIFFTX.bit.TXFFINTCLR = 1;					//复位发送中断
        pStu->pReg->SCIFFTX.bit.TXFFIENA = 1;						//开中断
    }
    else
    {
        eStatus = MB_EIO;
    }
    EXIT_CRITICAL_SECTION();
    return eStatus;
}




BOOL
xMBRTU335ReceiveFSM( struct ModbusSlaveStru *pStu )
{
    BOOL            xTaskNeedSwitch = FALSE;
    UCHAR           ucByte;
//    assert( pStu->eMBSndState == STATE_TX_IDLE );
    /* Always read the character. */
    ( void )xMBPortSerialGetByte( pStu,( CHAR * ) & ucByte );
    switch ( pStu->eMBRcvState )
    {
        /* If we have received a character in the init state we have to
         * wait until the frame is finished.
         */
    case STATE_RX_INIT:
        vMBPortTimersEnable( pStu );
        break;

        /* In the error state we wait until all characters in the
         * damaged frame are transmitted.
         */
    case STATE_RX_ERROR:
        vMBPortTimersEnable( pStu);
        break;

        /* In the idle state we wait for a new character. If a character
         * is received the t1.5 and t3.5 timers are started and the
         * receiver is in the state STATE_RX_RECEIVCE.
         */
    case STATE_RX_IDLE:
    	pStu->usRcvBufferPos = 0;
    	pStu->ModbusRcvSedBuff[pStu->usRcvBufferPos++] = ucByte;
    	pStu->eMBRcvState = STATE_RX_RCV;
        /* Enable t3.5 timers. */
        vMBPortTimersEnable(pStu);
        break;
        /* We are currently receiving a frame. Reset the timer after
         * every character received. If more than the maximum possible
         * number of bytes in a modbus frame is received the frame is
         * ignored.
         */
    case STATE_RX_RCV:
        if( pStu->usRcvBufferPos < MB_SER_PDU_SIZE_MAX )
        {
        		pStu->ModbusRcvSedBuff[pStu->usRcvBufferPos++] = ucByte;
        }
        else
        {
        	pStu->eMBRcvState = STATE_RX_ERROR;
        }
        vMBPortTimersEnable(pStu);
        break;
    }
    return xTaskNeedSwitch;
}

BOOL
xMBRTU335TransmitFSM( struct ModbusSlaveStru *pStu )
{
    BOOL            xNeedPoll = FALSE;

//    assert( pStu->eMBRcvState == STATE_RX_IDLE );

    switch ( pStu->eMBSndState )
    {
        /* We should not get a transmitter event if the transmitter is in
         * idle state.  */
    case STATE_TX_IDLE:
        /* enable receiver/disable transmitter. */
        vMBPortSerialEnable(pStu, TRUE, FALSE );
        break;
    case STATE_TX_XMIT:
        while((pStu->usSndBufferCount) && (pStu->pReg->SCIFFTX.bit.TXFFST < 15)){	//填入发送缓冲区
        		pStu->pReg->SCITXBUF = *(pStu->pucSndBufferCur)++;
        		pStu->usSndBufferCount--;
        }
        if((pStu->usSndBufferCount == 0)&&(pStu->pReg->SCIFFTX.bit.TXFFST == 0)/*&&(pStu->pReg->SCICTL2.bit.TXEMPTY == 1)*/)
        {
        	pStu->SendTrsformDelayTim = pStu->SendTrsformDelayTimCont;
        	pStu->pReg->SCIFFTX.bit.TXFIFOXRESET = 0;
//        	pStu->pReg->SCIFFTX.bit.SCIRST = 0;
        	pStu->pReg->SCIFFTX.bit.TXFFIENA = 0;					//关中断
        	pStu->pReg->SCIFFTX.bit.TXFIFOXRESET = 1;
//        	pStu->pReg->SCIFFTX.bit.SCIRST = 1;
//            xNeedPoll = xMBPortEventPost(pStu, EV_FRAME_SENT );
//        	vMBPortSerialEnable(pStu, TRUE, FALSE );
            pStu->eMBSndState = STATE_TX_IDLE;
        }
        break;
    }
    return xNeedPoll;
}

BOOL
xMBRTUTimerT35335Expired( struct ModbusSlaveStru *pStu )
{
    BOOL            xNeedPoll = FALSE;

    switch ( pStu->eMBRcvState )
    {
        /* Timer t35 expired. Startup phase is finished. */
    case STATE_RX_INIT:
        xNeedPoll = xMBPortEventPost( pStu,EV_READY );
        break;

        /* A frame was received and t35 expired. Notify the listener that
         * a new frame was received. */
    case STATE_RX_RCV:
        xNeedPoll = xMBPortEventPost( pStu,EV_FRAME_RECEIVED );
        break;

        /* An error occured while receiving the frame. */
    case STATE_RX_ERROR:
        break;

        /* Function called in an illegal state. */
    default:break;
//        assert( ( pStu->eMBRcvState == STATE_RX_INIT ) ||
//                ( pStu->eMBRcvState == STATE_RX_RCV ) || ( pStu->eMBRcvState == STATE_RX_ERROR ) );
    }
    vMBPortTimersDisable( pStu );
    pStu->eMBRcvState = STATE_RX_IDLE;
    return xNeedPoll;
}

/****************************************主机代码接口函数*********************************************/
eMBErrorCode
eMBMasterRTU335Receive( struct ModbusSlaveStru *pStu, UCHAR * pucRcvAddress, UCHAR ** pucFrame, USHORT * pusLength )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    ENTER_CRITICAL_SECTION(  );
//    assert( usMasterRcvBufferPos < MB_SER_PDU_SIZE_MAX );
    /* Length and CRC check */
    if( ( pStu->usRcvBufferPos >= MB_SER_PDU_SIZE_MIN )
        && ( usMBCRC16( (Uint16*)pStu->ModbusRcvSedBuff, pStu->usRcvBufferPos ) == 0 ) )
    {
        *pucRcvAddress = pStu->ModbusRcvSedBuff[MB_SER_PDU_ADDR_OFF];
    }
    else
    {
        eStatus = MB_EIO;
        if(pStu->portaddr == 2)		WrongData=1;//SCIC控制电容
    }

    EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

eMBErrorCode
eMBMasterRTU335Send( struct ModbusSlaveStru *pStu, UCHAR ucSlaveAddress, const UCHAR * pucFrame, USHORT usLength )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    USHORT          usCRC16;

//    if ( ucSlaveAddress > MB_MASTER_TOTAL_SLAVE_NUM ) 	return MB_EINVAL;

    ENTER_CRITICAL_SECTION(  );

    /* Check if the receiver is still in idle state. If not we where to
     * slow with processing the received frame and the master sent another
     * frame on the network. We have to abort sending the frame.
     */
    if( pStu->eMBRcvState == STATE_M_RX_IDLE )
    {
        /* First byte before the Modbus-PDU is the slave address. */
    	pStu->pucSndBufferCur = ( UCHAR * ) pucFrame - 1;
        pStu->usSndBufferCount = 1;
        /* Now copy the Modbus-PDU into the Modbus-Serial-Line-PDU. */
        pStu->pucSndBufferCur[MB_SER_PDU_ADDR_OFF] = ucSlaveAddress;
        pStu->usSndBufferCount += usLength;
        /* Calculate CRC16 checksum for Modbus-Serial-Line-PDU. */
        usCRC16 = usMBCRC16( ( UCHAR * ) (pStu->pucSndBufferCur), pStu->usSndBufferCount );
        pStu->ModbusRcvSedBuff[pStu->usSndBufferCount++] = ( UCHAR )( usCRC16 & 0xFF );
        pStu->ModbusRcvSedBuff[pStu->usSndBufferCount++] = ( UCHAR )( usCRC16 >> 8 );
        pStu->eMBSndState = STATE_M_TX_XMIT;
        vMBMasterPortSerialEnable(pStu, FALSE, TRUE );
        pStu->pReg->SCIFFTX.bit.TXFIFOXRESET = 1;					//复位发送缓冲区
        pStu->pReg->SCIFFTX.bit.TXFFINTCLR = 1;					//复位发送中断
        pStu->pReg->SCIFFTX.bit.TXFFIENA = 1;						//开中断
    }
    else
    {
        eStatus = MB_EIO;
    }
    EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

BOOL
xMBMasterRTU335ReceiveFSM( struct ModbusSlaveStru *pStu)
{
    BOOL            xTaskNeedSwitch = FALSE;
    UCHAR           ucByte;
    if(pStu->portaddr == 2)		RecdNone = 0;//SCIC控制电容
//    assert((  pStu->eMBSndState == STATE_M_TX_IDLE ) || (  pStu->eMBSndState == STATE_M_TX_XFWR ));

    /* Always read the character. */
    ( void )xMBMasterPortSerialGetByte(pStu, ( CHAR * ) & ucByte );

    switch ( pStu->eMBRcvState )
    {
        /* If we have received a character in the init state we have to
         * wait until the frame is finished.
         */
    case STATE_M_RX_INIT:
    	vMBMasterPortTimersEnable(pStu );
        break;

        /* In the error state we wait until all characters in the
         * damaged frame are transmitted.
         */
    case STATE_M_RX_ERROR:
    	vMBMasterPortTimersEnable(pStu );
        break;

        /* In the idle state we wait for a new character. If a character
         * is received the t1.5 and t3.5 timers are started and the
         * receiver is in the state STATE_RX_RECEIVCE and disable early
         * the timer of respond timeout .
         */
    case STATE_M_RX_IDLE:
    	/* In time of respond timeout,the receiver receive a frame.
    	 * Disable timer of respond timeout and change the transmiter state to idle.
    	 */
    	vMBMasterPortTimersDisable(pStu );
    	pStu->eMBSndState = STATE_M_TX_IDLE;
    	pStu->usRcvBufferPos = 0;
    	pStu->ModbusRcvSedBuff[pStu->usRcvBufferPos++] = ucByte;
        pStu->eMBRcvState = STATE_M_RX_RCV;

        /* Enable t3.5 timers. */
        vMBMasterPortTimersEnable(pStu );
        break;

        /* We are currently receiving a frame. Reset the timer after
         * every character received. If more than the maximum possible
         * number of bytes in a modbus frame is received the frame is
         * ignored.
         */
    case STATE_M_RX_RCV:
        if( pStu->usRcvBufferPos < MB_SER_PDU_SIZE_MAX )
        {
        	pStu->ModbusRcvSedBuff[pStu->usRcvBufferPos++] = ucByte;
        }
        else
        {
        	pStu->eMBRcvState = STATE_M_RX_ERROR;
        }
        vMBMasterPortTimersEnable(pStu);
        break;
    }
    return xTaskNeedSwitch;
}

BOOL
xMBMasterRTU335TransmitFSM( struct ModbusSlaveStru *pStu)
{
    BOOL            xNeedPoll = FALSE;

//    assert( pStu->eMBSndState == STATE_M_RX_IDLE );
    switch ( pStu->eMBSndState  )
    {
        /* We should not get a transmitter event if the transmitter is in
         * idle state.  */
    case STATE_M_TX_IDLE:
        /* enable receiver/disable transmitter. */
        vMBMasterPortSerialEnable(pStu, TRUE, FALSE );
        break;

    case STATE_M_TX_XMIT:
        while((pStu->usSndBufferCount) && (pStu->pReg->SCIFFTX.bit.TXFFST < 15)){	//填入发送缓冲区
        		pStu->pReg->SCITXBUF = *(pStu->pucSndBufferCur)++;
        		pStu->usSndBufferCount--;
        	}
         if((pStu->usSndBufferCount == 0)&&(ScicRegs.SCIFFTX.bit.TXFFST == 0)/*&&(ScicRegs.SCICTL2.bit.TXEMPTY == 1)*/)
         {
        	pStu->SendTrsformDelayTim = 2;
         	ScicRegs.SCIFFTX.bit.TXFIFOXRESET = 0;
//         	ScicRegs.SCIFFTX.bit.SCIRST = 0;
         	ScicRegs.SCIFFTX.bit.TXFFIENA = 0;					//关中断
         	ScicRegs.SCIFFTX.bit.TXFIFOXRESET = 1;
//         	ScicRegs.SCIFFTX.bit.SCIRST = 1;
//			vMBMasterPortSerialEnable(pStu, TRUE, FALSE );
			pStu->eMBSndState = STATE_M_TX_XFWR;
//			if ( xFrameIsBroadcast == TRUE )
//			{
//				vMBMasterPortTimersConvertDelayEnable( );
//			}
//			else
//			{
//				vMBMasterPortTimersRespondTimeoutEnable( );
//			}
         }
        break;
    }

    return xNeedPoll;
}

BOOL
xMBMasterRTUTimer335Expired( struct ModbusSlaveStru *pStu)
{
	BOOL xNeedPoll = FALSE;

	switch (pStu->eMBRcvState)
	{
		/* Timer t35 expired. Startup phase is finished. */
	case STATE_M_RX_INIT:
		xNeedPoll = xMBMasterPortEventPost(pStu,EV_MASTER_READY);
		break;

		/* A frame was received and t35 expired. Notify the listener that
		 * a new frame was received. */
	case STATE_M_RX_RCV:
		xNeedPoll = xMBMasterPortEventPost(pStu,EV_MASTER_FRAME_RECEIVED);
		break;

		/* An error occured while receiving the frame. */
	case STATE_M_RX_ERROR:
		vMBMasterSetErrorType(pStu,EV_ERROR_RECEIVE_DATA);
		xNeedPoll = xMBMasterPortEventPost( pStu,EV_MASTER_ERROR_PROCESS );
		break;

		/* Function called in an illegal state. */
	default:
//		assert(
//				( pStu->eMBRcvState == STATE_M_RX_INIT ) || ( pStu->eMBRcvState == STATE_M_RX_RCV ) ||
//				( pStu->eMBRcvState == STATE_M_RX_ERROR ) || ( pStu->eMBRcvState == STATE_M_RX_IDLE ));
		break;
	}
	pStu->eMBRcvState = STATE_M_RX_IDLE;

	switch (pStu->eMBSndState)
	{
		/* A frame was send finish and convert delay or respond timeout expired.
		 * If the frame is broadcast,The master will idle,and if the frame is not
		 * broadcast.Notify the listener process error.*/
	case STATE_M_TX_XFWR:
//		if ( xFrameIsBroadcast == FALSE ) {
//			vMBMasterSetErrorType(pStu,EV_ERROR_RESPOND_TIMEOUT);
//			xNeedPoll = xMBMasterPortEventPost(pStu,EV_MASTER_ERROR_PROCESS);
//		}
		break;
		/* Function called in an illegal state. */
	default:
//		assert(( pStu->eMBSndState == STATE_M_TX_XFWR ) || ( pStu->eMBSndState == STATE_M_TX_IDLE ));
		break;
	}
	pStu->eMBSndState = STATE_M_TX_IDLE;

	vMBMasterPortTimersDisable(pStu );
	/* If timer mode is convert delay, the master event then turns EV_MASTER_EXECUTE status. */
	if (eMasterCurTimerMode == MB_TMODE_CONVERT_DELAY) {
		xNeedPoll = xMBMasterPortEventPost(pStu, EV_MASTER_EXECUTE );
	}

	return xNeedPoll;
}

/* Get Modbus Master send RTU's buffer address pointer.*/
void vMBMasterGetRTUSndBuf( struct ModbusSlaveStru *pStu, UCHAR ** pucFrame )
{
	*pucFrame = ( UCHAR * ) (pStu->ModbusRcvSedBuff);
}

/* Get Modbus Master send PDU's buffer address pointer.*/
void vMBMasterGetPDUSndBuf( struct ModbusSlaveStru *pStu, UCHAR ** pucFrame )
{
	*pucFrame = ( UCHAR * ) &pStu->ModbusRcvSedBuff[MB_SER_PDU_PDU_OFF];
}

/* Set Modbus Master send PDU's buffer length.*/
void vMBMasterSetPDUSndLength(struct ModbusSlaveStru *pStu, USHORT SendPDULength )
{
	pStu->usMasterSendPDULength = SendPDULength;
}

/* Get Modbus Master send PDU's buffer length.*/
USHORT usMBMasterGetPDUSndLength( struct ModbusSlaveStru *pStu )
{
	return pStu->usMasterSendPDULength;
}

/* Set Modbus Master current timer mode.*/
void vMBMasterSetCurTimerMode( eMBMasterTimerMode eMBTimerMode )
{
	eMasterCurTimerMode = eMBTimerMode;
}

/* The master request is broadcast? */
BOOL xMBMasterRequestIsBroadcast( void ){
	return xFrameIsBroadcast;
}

#endif

