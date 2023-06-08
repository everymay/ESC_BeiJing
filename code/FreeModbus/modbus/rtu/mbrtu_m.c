/* 
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (c) 2013 China Beijing Armink <armink.ztl@gmail.com>
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
 * File: $Id: mbrtu_m.c,v 1.60 2013/08/17 11:42:56 Armink Add Master Functions $
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

#if MB_MASTER_RTU_ENABLED > 0
/* ----------------------- Defines ------------------------------------------*/
#define MB_SER_PDU_SIZE_MIN     4       /*!< Minimum size of a Modbus RTU frame. */
#define MB_SER_PDU_SIZE_MAX     256     /*!< Maximum size of a Modbus RTU frame. */
#define MB_SER_PDU_SIZE_CRC     2       /*!< Size of CRC field in PDU. */
#define MB_SER_PDU_ADDR_OFF     0       /*!< Offset of slave address in Ser-PDU. */
#define MB_SER_PDU_PDU_OFF      1       /*!< Offset of Modbus-PDU in Ser-PDU. */

/* ----------------------- Type definitions ---------------------------------*/
//typedef enum
//{
//    STATE_M_RX_INIT,              /*!< Receiver is in initial state. */
//    STATE_M_RX_IDLE,              /*!< Receiver is in idle state. */
//    STATE_M_RX_RCV,               /*!< Frame is beeing received. */
//    STATE_M_RX_ERROR,              /*!< If the frame is invalid. */
//} eMBMasterRcvState;

//typedef enum
//{
//    STATE_M_TX_IDLE,              /*!< Transmitter is in idle state. */
//    STATE_M_TX_XMIT,              /*!< Transmitter is in transfer state. */
//    STATE_M_TX_XFWR,              /*!< Transmitter is in transfer finish and wait receive state. */
//} eMBMasterSndState;

/* ----------------------- Static variables ---------------------------------*/
//static volatile eMBMasterSndState eSndState;
//static volatile eMBMasterRcvState eRcvState;
//static volatile UCHAR  ucMasterRTUSndBuf[MB_PDU_SIZE_MAX];
//volatile unsigned int ucMasterRTURcvBuf[MB_SER_PDU_SIZE_MAX];
//static volatile USHORT usMasterSndBufferCount;
//UCHAR usMasterRcvBufferPos = 0;
static volatile BOOL   xFrameIsBroadcast = FALSE;
static volatile USHORT usMasterSendPDULength;
static volatile eMBMasterTimerMode eMasterCurTimerMode;

/* ----------------------- Start implementation -----------------------------*/
eMBErrorCode
eMBMasterRTUInit( struct ModbusSlaveStru *pStu ,ULONG ulBaudRate, eMBParity eParity )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    ULONG           usTimerT35_50us;

    ENTER_CRITICAL_SECTION(  );

    /* Modbus RTU uses 8 Databits. */
    if( xMBMasterPortSerialInit( pStu, ulBaudRate, 8, eParity ) != TRUE )
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
        if( xMBMasterPortTimersInit( ( USHORT ) usTimerT35_50us ) != TRUE )
        {
            eStatus = MB_EPORTERR;
        }
    }
    EXIT_CRITICAL_SECTION(  );

    return eStatus;
}
/*函数功能
*1:设置接收状态机eRcvState为STATE_RX_INIT；
*2:使能串口接收,禁止串口发送,作为从机,等待主机传送的数据;
*3:开启定时器，3.5T时间后定时器发生第一次中断,此时eRcvState为STATE_RX_INIT,上报初始化完成事件,然后设置eRcvState为空闲STATE_RX_IDLE;
*4:每次进入3.5T定时器中断,定时器被禁止，等待串口有字节接收后，才使能定时器;
*/
void
eMBMasterRTUStart( struct ModbusSlaveStru *pStu )
{
    ENTER_CRITICAL_SECTION(  );
    /* Initially the receiver is in the state STATE_M_RX_INIT. we start
     * the timer and if no character is received within t3.5 we change
     * to STATE_M_RX_IDLE. This makes sure that we delay startup of the
     * modbus protocol stack until the bus is free.
     * 最初，接收方处于STATE_M_RX_INIT状态。我们启动计时器，如果在t3.5内没有接收到字符，
     * 我们将更改为STATE_M_RX_IDLE。这确保我们延迟modbus协议栈的启动，直到总线空闲。
     */
    pStu->eMBRcvState = STATE_M_RX_INIT;
    vMBMasterPortSerialEnable(pStu, TRUE, FALSE );
    vMBMasterPortTimersEnable( pStu );
    EXIT_CRITICAL_SECTION(  );
}

//void
//eMBMasterRTUStop( void )
//{
//    ENTER_CRITICAL_SECTION(  );
//    vMBMasterPortSerialEnable( FALSE, FALSE );
//    vMBMasterPortTimersDisable(  );
//    EXIT_CRITICAL_SECTION(  );
//}
/*eMBPoll函数轮询到EV_FRAME_RECEIVED事件时,调用peMBFrameReceiveCur()，此函数是用户为函数指针peMBFrameReceiveCur()的赋值
*此函数完成的功能：从一帧数据报文中，取得modbus从机地址给pucRcvAddress，PDU报文的长度给pusLength，PDU报文的首地址给pucFrame，函数
*形参全部为地址传递*/
eMBErrorCode
eMBMasterRTUReceive( struct ModbusSlaveStru *pStu, UCHAR * pucRcvAddress, UCHAR ** pucFrame, USHORT * pusLength )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    ENTER_CRITICAL_SECTION(  );
//    assert( usMasterRcvBufferPos < MB_SER_PDU_SIZE_MAX );   /*断言宏，判断接收到的字节数<256，如果>256，终止程序*/

    /* Length and CRC check */
    if( ( pStu->usRcvBufferPos >= MB_SER_PDU_SIZE_MIN )
        && ( usMBCRC16( (Uint16*)pStu->ModbusRcvSedBuff, pStu->usRcvBufferPos ) == 0 ) )
    {
        *pucRcvAddress = pStu->ModbusRcvSedBuff[MB_SER_PDU_ADDR_OFF]; //取接收到的第一个字节，modbus从机地址
    }
    else
    {
        eStatus = MB_EIO;
    }

    EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

/*函数功能
*1:对响应报文PDU前面加上从机地址;
*2:对响应报文PDU后加上CRC校;
*3:使能发送，启动传输;
*/
eMBErrorCode
eMBMasterRTUSend( struct ModbusSlaveStru *pStu, UCHAR ucSlaveAddress, const UCHAR * pucFrame, USHORT usLength )     //RTU远程终端设备
{
    eMBErrorCode    eStatus = MB_ENOERR;
    USHORT          usCRC16;

//    if ( ucSlaveAddress > MB_MASTER_TOTAL_SLAVE_NUM ) 	return MB_EINVAL;

    ENTER_CRITICAL_SECTION(  );

    /* Check if the receiver is still in idle state. If not we where to
     * slow with processing the received frame and the master sent another
     * frame on the network. We have to abort sending the frame.
     * 检查接收机是否仍处于空闲状态.
     * 如果不是，我们在哪里慢处理收到的帧和主人发送另一个帧在网络上.
     * 我们必须中止传送画面.
     */
    if( pStu->eMBRcvState == STATE_M_RX_IDLE )
    {
        /* First byte before the Modbus-PDU is the slave address. */
        /*在协议数据单元前加从机地址*/
    	pStu->pucSndBufferCur = ( UCHAR * ) pucFrame - 1;               //指针指向当前发送数据(第一个数据是功能码)的地址
        pStu->usSndBufferCount = 1;
        /* Now copy the Modbus-PDU into the Modbus-Serial-Line-PDU. */  //PDU协议数据单元
        /* 现在将Modbus-PDU复制到Modbus-Serial-Line-PDU中.*/
        pStu->pucSndBufferCur[MB_SER_PDU_ADDR_OFF] = ucSlaveAddress;    //将从机的单元地址给到当前数据缓冲区
        pStu->ModbusMasterBuff[MB_SER_PDU_ADDR_OFF] = ucSlaveAddress;   //记录从机的单元地址
        pStu->usSndBufferCount += usLength;
        /* Calculate CRC16 checksum for Modbus-Serial-Line-PDU. */
        usCRC16 = usMBCRC16( ( UCHAR * ) (pStu->pucSndBufferCur), pStu->usSndBufferCount );
        pStu->ModbusRcvSedBuff[pStu->usSndBufferCount++] = ( UCHAR )( usCRC16 & 0xFF );
        pStu->ModbusRcvSedBuff[pStu->usSndBufferCount++] = ( UCHAR )( usCRC16 >> 8 );

        pStu->eMBSndState = STATE_M_TX_XMIT;//发送状态切换,在中断中不断发送 /*发送一个字节的数据，进入发送中断函数，启动传输*/

        vMBMasterPortSerialEnable(pStu, FALSE, TRUE );//使能发送,禁止接收
        pStu->pReg->SCIFFTX.bit.TXFIFORESET = 1;					//复位发送缓冲区
        pStu->pReg->SCIFFTX.bit.TXFFINTCLR = 1;					    //复位发送中断
        pStu->pReg->SCIFFTX.bit.TXFFIENA = 1;						//开中断
    }
    else
    {
        eStatus = MB_EIO;
    }
    EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

/*函数功能
*1:将接收到的数据存入ucRTUBuf[]中;
*2:usRcvBufferPos为全局变量，表示接收数据的个数;
*3:每接收到一个字节的数据，3.5T定时器清0
*/
BOOL
xMBMasterRTUReceiveFSM( struct ModbusSlaveStru *pStu)
{
    BOOL            xTaskNeedSwitch = FALSE;
    UCHAR           ucByte;
    if(pStu->portaddr == 2)		RecdNone = 0;//SCIC控制电容
//    assert((  pStu->eMBSndState == STATE_M_TX_IDLE ) || (  pStu->eMBSndState == STATE_M_TX_XFWR )); /*确保没有数据在发送*/

    /* Always read the character. */
    /*从串口数据寄存器读取一个字节数据*/
    ( void )xMBMasterPortSerialGetByte(pStu, ( UCHAR * ) & ucByte );
    //根据不同的状态转移
    switch ( pStu->eMBRcvState )
    {
        /* If we have received a character in the init state we have to
         * wait until the frame is finished.
         * 如果我们在init状态收到一个字符，我们必须等待，直到帧完成。
         */
    case STATE_M_RX_INIT:
    	vMBMasterPortTimersEnable(pStu );     /*开启3.5T定时器*/
        break;

        /* In the error state we wait until all characters in the
         * damaged frame are transmitted.
         * 在错误状态下，我们等待损坏帧中的所有字符都传输完.
         */
    case STATE_M_RX_ERROR:        /*数据帧被损坏，重启定时器，不保存串口接收的数据*/
    	vMBMasterPortTimersEnable(pStu );
        break;

        /* In the idle state we wait for a new character. If a character
         * is received the t1.5 and t3.5 timers are started and the
         * receiver is in the state STATE_RX_RECEIVCE and disable early
         * the timer of respond timeout .
         * 在空闲状态下，我们等待一个新的字符。如果接收到字符，则启动t1.5和t3.5计时器，
         * 并且接收器处于STATE_RX_RECEIVCE状态，并提前禁用响应超时计时器.
         */
    case STATE_M_RX_IDLE:   /*接收器空闲，开始接收，进入STATE_RX_RCV状态*/
    	/* In time of respond timeout,the receiver receive a frame.
    	 * Disable timer of respond timeout and change the transmiter state to idle.
    	 * 在响应超时时间，接收端接收一个帧。关闭响应超时定时器，改变发送器状态为idle。
    	 */
    	vMBMasterPortTimersDisable(pStu );
    	pStu->eMBSndState = STATE_M_TX_IDLE;
    	pStu->usRcvBufferPos = 0;
    	pStu->ModbusRcvSedBuff[pStu->usRcvBufferPos++] = ucByte;  /*保存数据*/
        pStu->eMBRcvState = STATE_M_RX_RCV;   //正在接收帧

        /* Enable t3.5 timers. */
        vMBMasterPortTimersEnable(pStu );
        break;

        /* We are currently receiving a frame. Reset the timer after
         * every character received. If more than the maximum possible
         * number of bytes in a modbus frame is received the frame is
         * ignored.
         * 我们正在接收帧。在每个字符收到后重置计时器。
         * 如果接收到的modbus帧超过最大可能的字节数，该帧将被忽略。
         */
    case STATE_M_RX_RCV:              //正在接收帧
        if( pStu->usRcvBufferPos < MB_SER_PDU_SIZE_MAX )
        {
        	pStu->ModbusRcvSedBuff[pStu->usRcvBufferPos++] = ucByte;  /*接收数据*/
        }
        else
        {
        	pStu->eMBRcvState = STATE_M_RX_ERROR;  /*一帧报文的字节数大于最大PDU长度，忽略超出的数据*/
        }
        vMBMasterPortTimersEnable(pStu);     /*每收到一个字节，都重启3.5T定时器*/
        break;
    }
    return xTaskNeedSwitch;
}

BOOL
xMBMasterRTUTransmitFSM( struct ModbusSlaveStru *pStu)
{
    BOOL            xNeedPoll = FALSE;

//    assert( pStu->eMBSndState == STATE_M_RX_IDLE );
    switch ( pStu->eMBSndState  )
    {
        /* We should not get a transmitter event if the transmitter is in
         * idle state.
         * 如果发送器处于空闲状态，我们不应该得到发送事件。
         */
    case STATE_M_TX_IDLE:
        /* enable receiver/disable transmitter. */
        /*发送器处于空闲状态，使能接收，禁止发送*/
        vMBMasterPortSerialEnable(pStu, TRUE, FALSE );
        break;

    case STATE_M_TX_XMIT:   /*发送器处于发送状态,在从机发送函数eMBRTUSend中赋值STATE_TX_XMIT*/
        while((pStu->usSndBufferCount) && (pStu->pReg->SCIFFTX.bit.TXFFST < 15)){	//填入发送缓冲区
        		pStu->pReg->SCITXBUF.all = *(pStu->pucSndBufferCur)++;
        		pStu->usSndBufferCount--;
        	}
         if((pStu->usSndBufferCount == 0)&&(ScicRegs.SCIFFTX.bit.TXFFST == 0)/*&&(ScicRegs.SCICTL2.bit.TXEMPTY == 1)*/) //传递任务，发送完成
         {
        	pStu->SendTrsformDelayTim = 2;
         	ScicRegs.SCIFFTX.bit.TXFIFORESET = 0;
//         	ScicRegs.SCIFFTX.bit.SCIRST = 0;
         	ScicRegs.SCIFFTX.bit.TXFFIENA = 0;					//关中断
         	ScicRegs.SCIFFTX.bit.TXFIFORESET = 1;
//         	ScicRegs.SCIFFTX.bit.SCIRST = 1;
//			vMBMasterPortSerialEnable(pStu, TRUE, FALSE );
			pStu->eMBSndState = STATE_M_TX_XFWR;   //发送器处于传输结束、等待接收状态
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
/*函数功能
*1:从机接受完成一帧数据后，接收状态机eRcvState为STATE_RX_RCV；
*2:上报“接收到报文”事件(EV_FRAME_RECEIVED)
*3:禁止3.5T定时器，设置接收状态机eRcvState状态为STATE_RX_IDLE空闲;
*/
BOOL
xMBMasterRTUTimerExpired( struct ModbusSlaveStru *pStu)
{
	BOOL xNeedPoll = FALSE;

	switch (pStu->eMBRcvState)
	{
		/* Timer t35 expired. Startup phase is finished.
		 * 计时器t35超时.启动阶段结束.
		 */
	/*上报modbus协议栈的事件状态给poll函数,EV_READY:初始化完成事件*/
	case STATE_M_RX_INIT:
		xNeedPoll = xMBMasterPortEventPost(pStu,EV_MASTER_READY);/*上报协议栈事件,初始化完成事件*/
		break;

		/* A frame was received and t35 expired. Notify the listener that
		 * a new frame was received.
		 * 收到一帧，t35过期。通知侦听器收到了新帧.
		 */
	case STATE_M_RX_RCV:            /*一帧数据接收完成*/
		xNeedPoll = xMBMasterPortEventPost(pStu,EV_MASTER_FRAME_RECEIVED);/*上报协议栈事件,接收到一帧完整的数据*/
		break;

		/* An error occured while receiving the frame. */
	case STATE_M_RX_ERROR:
		vMBMasterSetErrorType(pStu,EV_ERROR_RECEIVE_DATA);
		xNeedPoll = xMBMasterPortEventPost( pStu,EV_MASTER_ERROR_PROCESS );/*上报协议栈事件,帧错误过程*/
		break;

		/* Function called in an illegal state.
		 * 函数在非法状态下调用
		 */
	default:
//		assert(
//				( pStu->eMBRcvState == STATE_M_RX_INIT ) || ( pStu->eMBRcvState == STATE_M_RX_RCV ) ||
//				( pStu->eMBRcvState == STATE_M_RX_ERROR ) || ( pStu->eMBRcvState == STATE_M_RX_IDLE ));
		break;
	}
	/*当接收到一帧数据后，禁止3.5T定时器，只到接受下一帧数据开始，开始计时*/
	pStu->eMBRcvState = STATE_M_RX_IDLE;    /*处理完一帧数据，接收器状态为空闲*/

	switch (pStu->eMBSndState)
	{
		/* A frame was send finish and convert delay or respond timeout expired.
		 * If the frame is broadcast,The master will idle,and if the frame is not
		 * broadcast.Notify the listener process error.
		 * 帧发送完成和转换延迟或响应超时过期。
		 * 如果帧是广播的，主机将空闲，如果帧不是广播的.通知侦听器进程错误
		 */
	case STATE_M_TX_XFWR:     //发射机处于传输结束、等待接收状态
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
	/* If timer mode is convert delay, the master event then turns EV_MASTER_EXECUTE status.
	 * 如果定时器模式是转换延迟，那么主机状态将变成EV_MASTER_EXECUTE状态.
	 */
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
void vMBMasterGetPDUSndBuf( struct ModbusSlaveStru *pStu, UCHAR ** pucFrame, UCHAR ** pucSndFrame)
{
	*pucFrame = ( UCHAR * ) &pStu->ModbusRcvSedBuff[MB_SER_PDU_PDU_OFF];
	*pucSndFrame = ( UCHAR * ) &pStu->ModbusMasterBuff[MB_SER_PDU_PDU_OFF];
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

