/* 
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (c) 2006 Christian Walter <wolti@sil.at>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: $Id: mbrtu.c,v 1.18 2007/09/12 10:15:56 wolti Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbrtu.h"
#include "mbframe.h"
#include "mbcrc.h"
#include "mbport.h"
/* ----------------------- Defines ------------------------------------------*/
#define MB_SER_PDU_SIZE_MIN     4       /*!< Minimum size of a Modbus RTU frame. */
#define MB_SER_PDU_SIZE_MAX     256     /*!< Maximum size of a Modbus RTU frame. */
#define MB_SER_PDU_SIZE_CRC     2       /*!< Size of CRC field in PDU. */
#define MB_SER_PDU_ADDR_OFF     0       /*!< Offset of slave address in Ser-PDU. */
#define MB_SER_PDU_PDU_OFF      1       /*!< Offset of Modbus-PDU in Ser-PDU. */

#pragma	CODE_SECTION(xMBRTUTransmitFSM ,"ram2func")
#pragma	CODE_SECTION(xMBRTUReceiveFSM ,"ram2func")
extern Uint16 SendStateEventWifi;
/* ----------------------- Start implementation -----------------------------*/
eMBErrorCode
eMBRTUInit( struct ModbusSlaveStru *pStu,UCHAR ucSlaveAddress, UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    ULONG           usTimerT35_50us;

    ( void )ucSlaveAddress;
    ENTER_CRITICAL_SECTION();

    /* Modbus RTU uses 8 Databits. */
    if( xMBPortSerialInit( pStu, ulBaudRate, 8, eParity ) != TRUE )
    {
        eStatus = MB_EPORTERR;
    }
    else
    {
        /* If baudrate > 19200 then we should use the fixed timer values
         * t35 = 1750us. Otherwise t35 must be 3.5 times the character time.
         */
        if( ulBaudRate > 19200 )
        {
            usTimerT35_50us = 35;       /* 1800us. */
        }
        else
        {
            /* The timer reload value for a character is given by:
             *
             * ChTimeValue = Ticks_per_1s / ( Baudrate / 11 )
             *             = 11 * Ticks_per_1s / Baudrate
             *             = 220000 / Baudrate
             * The reload for t3.5 is 1.5 times this value and similary
             * for t3.5.
             */
            usTimerT35_50us = ( 7UL * 220000UL ) / ( 2UL * ulBaudRate );
        }
        if( xMBPortTimersInit( ( USHORT ) usTimerT35_50us ) != TRUE )
        {
            eStatus = MB_EPORTERR;
        }
    }
    EXIT_CRITICAL_SECTION();
    return eStatus;
}
/*函数功能
*1:设置接收状态机eRcvState为STATE_RX_INIT；
*2:使能串口接收,禁止串口发送,作为从机,等待主机传送的数据;
*3:开启定时器，3.5T时间后定时器发生第一次中断,此时eRcvState为STATE_RX_INIT,上报初始化完成事件,然后设置eRcvState为空闲STATE_RX_IDLE;
*4:每次进入3.5T定时器中断,定时器被禁止，等待串口有字节接收后，才使能定时器;
*/
void
eMBRTUStart( struct ModbusSlaveStru *pStu )
{
    ENTER_CRITICAL_SECTION(  );
    /* Initially the receiver is in the state STATE_RX_INIT. we start
     * the timer and if no character is received within t3.5 we change
     * to STATE_RX_IDLE. This makes sure that we delay startup of the
     * modbus protocol stack until the bus is free.
     * 最初，接收方处于STATE_RX_INIT状态。
     * 我们启动计时器，如果在t3.5内没有接收到字符，我们将更改为STATE_RX_IDLE。
     * 这确保我们延迟modbus协议栈的启动，直到总线空闲。
     */
    pStu->eMBRcvState = STATE_RX_INIT;
    vMBPortSerialEnable(pStu, TRUE, FALSE );
    vMBPortTimersEnable( pStu );
    EXIT_CRITICAL_SECTION(  );
}

//void
//eMBRTUStop( void )
//{
//    ENTER_CRITICAL_SECTION(  );
//    vMBPortSerialEnable( FALSE, FALSE );
//    vMBPortTimersDisable(  );
//    EXIT_CRITICAL_SECTION(  );
//}
eMBErrorCode
eMBRTUReceive(struct ModbusSlaveStru *pStu, Uint16 * pucRcvAddress, UCHAR ** pucFrame, USHORT * pusLength )
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
eMBRTUSend( struct ModbusSlaveStru *pStu,UCHAR ucSlaveAddress, const UCHAR * pucFrame, USHORT usLength )
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
        if(SendStateEventWifi == 0) //正常状态机发送
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
        }
        else            //Wifi 配置发送
        {
            pStu->pucSndBufferCur = ( UCHAR * ) pucFrame;
            pStu->usSndBufferCount = usLength;
        }
        pStu->eMBSndState = STATE_TX_XMIT;
        vMBPortSerialEnable(pStu, FALSE, TRUE );
        pStu->pReg->SCIFFTX.bit.TXFIFORESET = 1;				//复位发送缓冲区
        pStu->pReg->SCIFFTX.bit.TXFFINTCLR = 1;					//复位发送中断
        pStu->pReg->SCIFFTX.bit.TXFFIENA = 1;					//开中断
    }
    else
    {
        eStatus = MB_EIO;
    }
    EXIT_CRITICAL_SECTION();
    return eStatus;
}


inline BOOL xMBPortSerialGetByte( struct ModbusSlaveStru *pStu,CHAR * pucByte )
{
	*pucByte= pStu->pReg->SCIRXBUF.all;
	return TRUE;
}
BOOL
xMBRTUReceiveFSM( struct ModbusSlaveStru *pStu )
{
    BOOL            xTaskNeedSwitch = FALSE;
    UCHAR           ucByte;
//    assert( pStu->eMBSndState == STATE_TX_IDLE );
    /* Always read the character. */
    ( void )xMBPortSerialGetByte( pStu,( CHAR * ) & ucByte );
    switch ( pStu->eMBRcvState )
    {
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
    case STATE_RX_IDLE:
    	pStu->usRcvBufferPos = 0;
    	pStu->ModbusRcvSedBuff[pStu->usRcvBufferPos++] = ucByte;
    	pStu->eMBRcvState = STATE_RX_RCV;
        /* Enable t3.5 timers. */
        vMBPortTimersEnable(pStu);
        break;

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

    }
    return xTaskNeedSwitch;
}

BOOL
xMBRTUTransmitFSM( struct ModbusSlaveStru *pStu )
{
    BOOL            xNeedPoll = FALSE;
    Uint16 fifoCnt,buffCnt,cnt;
    volatile Uint16 *dst,*src;
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
    	fifoCnt = 15-pStu->pReg->SCIFFTX.bit.TXFFST;
    	buffCnt = (pStu->usSndBufferCount);
    	cnt = buffCnt<fifoCnt ? buffCnt :fifoCnt;	//取最小量作为循环次数
		pStu->usSndBufferCount -= cnt;
      	dst=&pStu->pReg->SCITXBUF.all;
        src=pStu->pucSndBufferCur;
 		while(cnt--){
//		   	   *dst=*src++;
		   	pStu->pReg->SCITXBUF.bit.TXDT = *(pStu->pucSndBufferCur)++;
    	}
//        pStu->pucSndBufferCur=src;

 /*       while((pStu->usSndBufferCount) && (pStu->pReg->SCIFFTX.bit.TXFFST < 15)){	//填入发送缓冲区
        		pStu->pReg->SCITXBUF = *(pStu->pucSndBufferCur)++;
        		pStu->usSndBufferCount--;
        }
*/
        if((pStu->usSndBufferCount == 0)&&(pStu->pReg->SCIFFTX.bit.TXFFST == 0)/*&&(pStu->pReg->SCICTL2.bit.TXEMPTY == 1)*/)
        {
        	pStu->SendTrsformDelayTim = pStu->SendTrsformDelayTimCont;
        	pStu->pReg->SCIFFTX.bit.TXFIFORESET = 0;
//        	pStu->pReg->SCIFFTX.bit.SCIRST = 0;
        	pStu->pReg->SCIFFTX.bit.TXFFIENA = 0;					//关中断
        	pStu->pReg->SCIFFTX.bit.TXFIFORESET = 1;
//        	pStu->pReg->SCIFFTX.bit.SCIRST = 1;
//          xNeedPoll = xMBPortEventPost(pStu, EV_FRAME_SENT );
//        	vMBPortSerialEnable(pStu, TRUE, FALSE );
            pStu->eMBSndState = STATE_TX_IDLE;
        }
        break;
    }
    return xNeedPoll;
}

BOOL
xMBRTUTimerT35Expired( struct ModbusSlaveStru *pStu )
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
