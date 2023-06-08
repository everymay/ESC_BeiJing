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
/*��������
*1:���ý���״̬��eRcvStateΪSTATE_RX_INIT��
*2:ʹ�ܴ��ڽ���,��ֹ���ڷ���,��Ϊ�ӻ�,�ȴ��������͵�����;
*3:������ʱ����3.5Tʱ���ʱ��������һ���ж�,��ʱeRcvStateΪSTATE_RX_INIT,�ϱ���ʼ������¼�,Ȼ������eRcvStateΪ����STATE_RX_IDLE;
*4:ÿ�ν���3.5T��ʱ���ж�,��ʱ������ֹ���ȴ��������ֽڽ��պ󣬲�ʹ�ܶ�ʱ��;
*/
void
eMBMasterRTUStart( struct ModbusSlaveStru *pStu )
{
    ENTER_CRITICAL_SECTION(  );
    /* Initially the receiver is in the state STATE_M_RX_INIT. we start
     * the timer and if no character is received within t3.5 we change
     * to STATE_M_RX_IDLE. This makes sure that we delay startup of the
     * modbus protocol stack until the bus is free.
     * ��������շ�����STATE_M_RX_INIT״̬������������ʱ���������t3.5��û�н��յ��ַ���
     * ���ǽ�����ΪSTATE_M_RX_IDLE����ȷ�������ӳ�modbusЭ��ջ��������ֱ�����߿��С�
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
/*eMBPoll������ѯ��EV_FRAME_RECEIVED�¼�ʱ,����peMBFrameReceiveCur()���˺������û�Ϊ����ָ��peMBFrameReceiveCur()�ĸ�ֵ
*�˺�����ɵĹ��ܣ���һ֡���ݱ����У�ȡ��modbus�ӻ���ַ��pucRcvAddress��PDU���ĵĳ��ȸ�pusLength��PDU���ĵ��׵�ַ��pucFrame������
*�β�ȫ��Ϊ��ַ����*/
eMBErrorCode
eMBMasterRTUReceive( struct ModbusSlaveStru *pStu, UCHAR * pucRcvAddress, UCHAR ** pucFrame, USHORT * pusLength )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    ENTER_CRITICAL_SECTION(  );
//    assert( usMasterRcvBufferPos < MB_SER_PDU_SIZE_MAX );   /*���Ժ꣬�жϽ��յ����ֽ���<256�����>256����ֹ����*/

    /* Length and CRC check */
    if( ( pStu->usRcvBufferPos >= MB_SER_PDU_SIZE_MIN )
        && ( usMBCRC16( (Uint16*)pStu->ModbusRcvSedBuff, pStu->usRcvBufferPos ) == 0 ) )
    {
        *pucRcvAddress = pStu->ModbusRcvSedBuff[MB_SER_PDU_ADDR_OFF]; //ȡ���յ��ĵ�һ���ֽڣ�modbus�ӻ���ַ
    }
    else
    {
        eStatus = MB_EIO;
    }

    EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

/*��������
*1:����Ӧ����PDUǰ����ϴӻ���ַ;
*2:����Ӧ����PDU�����CRCУ;
*3:ʹ�ܷ��ͣ���������;
*/
eMBErrorCode
eMBMasterRTUSend( struct ModbusSlaveStru *pStu, UCHAR ucSlaveAddress, const UCHAR * pucFrame, USHORT usLength )     //RTUԶ���ն��豸
{
    eMBErrorCode    eStatus = MB_ENOERR;
    USHORT          usCRC16;

//    if ( ucSlaveAddress > MB_MASTER_TOTAL_SLAVE_NUM ) 	return MB_EINVAL;

    ENTER_CRITICAL_SECTION(  );

    /* Check if the receiver is still in idle state. If not we where to
     * slow with processing the received frame and the master sent another
     * frame on the network. We have to abort sending the frame.
     * �����ջ��Ƿ��Դ��ڿ���״̬.
     * ������ǣ������������������յ���֡�����˷�����һ��֡��������.
     * ���Ǳ�����ֹ���ͻ���.
     */
    if( pStu->eMBRcvState == STATE_M_RX_IDLE )
    {
        /* First byte before the Modbus-PDU is the slave address. */
        /*��Э�����ݵ�Ԫǰ�Ӵӻ���ַ*/
    	pStu->pucSndBufferCur = ( UCHAR * ) pucFrame - 1;               //ָ��ָ��ǰ��������(��һ�������ǹ�����)�ĵ�ַ
        pStu->usSndBufferCount = 1;
        /* Now copy the Modbus-PDU into the Modbus-Serial-Line-PDU. */  //PDUЭ�����ݵ�Ԫ
        /* ���ڽ�Modbus-PDU���Ƶ�Modbus-Serial-Line-PDU��.*/
        pStu->pucSndBufferCur[MB_SER_PDU_ADDR_OFF] = ucSlaveAddress;    //���ӻ��ĵ�Ԫ��ַ������ǰ���ݻ�����
        pStu->ModbusMasterBuff[MB_SER_PDU_ADDR_OFF] = ucSlaveAddress;   //��¼�ӻ��ĵ�Ԫ��ַ
        pStu->usSndBufferCount += usLength;
        /* Calculate CRC16 checksum for Modbus-Serial-Line-PDU. */
        usCRC16 = usMBCRC16( ( UCHAR * ) (pStu->pucSndBufferCur), pStu->usSndBufferCount );
        pStu->ModbusRcvSedBuff[pStu->usSndBufferCount++] = ( UCHAR )( usCRC16 & 0xFF );
        pStu->ModbusRcvSedBuff[pStu->usSndBufferCount++] = ( UCHAR )( usCRC16 >> 8 );

        pStu->eMBSndState = STATE_M_TX_XMIT;//����״̬�л�,���ж��в��Ϸ��� /*����һ���ֽڵ����ݣ����뷢���жϺ�������������*/

        vMBMasterPortSerialEnable(pStu, FALSE, TRUE );//ʹ�ܷ���,��ֹ����
        pStu->pReg->SCIFFTX.bit.TXFIFORESET = 1;					//��λ���ͻ�����
        pStu->pReg->SCIFFTX.bit.TXFFINTCLR = 1;					    //��λ�����ж�
        pStu->pReg->SCIFFTX.bit.TXFFIENA = 1;						//���ж�
    }
    else
    {
        eStatus = MB_EIO;
    }
    EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

/*��������
*1:�����յ������ݴ���ucRTUBuf[]��;
*2:usRcvBufferPosΪȫ�ֱ�������ʾ�������ݵĸ���;
*3:ÿ���յ�һ���ֽڵ����ݣ�3.5T��ʱ����0
*/
BOOL
xMBMasterRTUReceiveFSM( struct ModbusSlaveStru *pStu)
{
    BOOL            xTaskNeedSwitch = FALSE;
    UCHAR           ucByte;
    if(pStu->portaddr == 2)		RecdNone = 0;//SCIC���Ƶ���
//    assert((  pStu->eMBSndState == STATE_M_TX_IDLE ) || (  pStu->eMBSndState == STATE_M_TX_XFWR )); /*ȷ��û�������ڷ���*/

    /* Always read the character. */
    /*�Ӵ������ݼĴ�����ȡһ���ֽ�����*/
    ( void )xMBMasterPortSerialGetByte(pStu, ( UCHAR * ) & ucByte );
    //���ݲ�ͬ��״̬ת��
    switch ( pStu->eMBRcvState )
    {
        /* If we have received a character in the init state we have to
         * wait until the frame is finished.
         * ���������init״̬�յ�һ���ַ������Ǳ���ȴ���ֱ��֡��ɡ�
         */
    case STATE_M_RX_INIT:
    	vMBMasterPortTimersEnable(pStu );     /*����3.5T��ʱ��*/
        break;

        /* In the error state we wait until all characters in the
         * damaged frame are transmitted.
         * �ڴ���״̬�£����ǵȴ���֡�е������ַ���������.
         */
    case STATE_M_RX_ERROR:        /*����֡���𻵣�������ʱ���������洮�ڽ��յ�����*/
    	vMBMasterPortTimersEnable(pStu );
        break;

        /* In the idle state we wait for a new character. If a character
         * is received the t1.5 and t3.5 timers are started and the
         * receiver is in the state STATE_RX_RECEIVCE and disable early
         * the timer of respond timeout .
         * �ڿ���״̬�£����ǵȴ�һ���µ��ַ���������յ��ַ���������t1.5��t3.5��ʱ����
         * ���ҽ���������STATE_RX_RECEIVCE״̬������ǰ������Ӧ��ʱ��ʱ��.
         */
    case STATE_M_RX_IDLE:   /*���������У���ʼ���գ�����STATE_RX_RCV״̬*/
    	/* In time of respond timeout,the receiver receive a frame.
    	 * Disable timer of respond timeout and change the transmiter state to idle.
    	 * ����Ӧ��ʱʱ�䣬���ն˽���һ��֡���ر���Ӧ��ʱ��ʱ�����ı䷢����״̬Ϊidle��
    	 */
    	vMBMasterPortTimersDisable(pStu );
    	pStu->eMBSndState = STATE_M_TX_IDLE;
    	pStu->usRcvBufferPos = 0;
    	pStu->ModbusRcvSedBuff[pStu->usRcvBufferPos++] = ucByte;  /*��������*/
        pStu->eMBRcvState = STATE_M_RX_RCV;   //���ڽ���֡

        /* Enable t3.5 timers. */
        vMBMasterPortTimersEnable(pStu );
        break;

        /* We are currently receiving a frame. Reset the timer after
         * every character received. If more than the maximum possible
         * number of bytes in a modbus frame is received the frame is
         * ignored.
         * �������ڽ���֡����ÿ���ַ��յ������ü�ʱ����
         * ������յ���modbus֡���������ܵ��ֽ�������֡�������ԡ�
         */
    case STATE_M_RX_RCV:              //���ڽ���֡
        if( pStu->usRcvBufferPos < MB_SER_PDU_SIZE_MAX )
        {
        	pStu->ModbusRcvSedBuff[pStu->usRcvBufferPos++] = ucByte;  /*��������*/
        }
        else
        {
        	pStu->eMBRcvState = STATE_M_RX_ERROR;  /*һ֡���ĵ��ֽ����������PDU���ȣ����Գ���������*/
        }
        vMBMasterPortTimersEnable(pStu);     /*ÿ�յ�һ���ֽڣ�������3.5T��ʱ��*/
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
         * ������������ڿ���״̬�����ǲ�Ӧ�õõ������¼���
         */
    case STATE_M_TX_IDLE:
        /* enable receiver/disable transmitter. */
        /*���������ڿ���״̬��ʹ�ܽ��գ���ֹ����*/
        vMBMasterPortSerialEnable(pStu, TRUE, FALSE );
        break;

    case STATE_M_TX_XMIT:   /*���������ڷ���״̬,�ڴӻ����ͺ���eMBRTUSend�и�ֵSTATE_TX_XMIT*/
        while((pStu->usSndBufferCount) && (pStu->pReg->SCIFFTX.bit.TXFFST < 15)){	//���뷢�ͻ�����
        		pStu->pReg->SCITXBUF.all = *(pStu->pucSndBufferCur)++;
        		pStu->usSndBufferCount--;
        	}
         if((pStu->usSndBufferCount == 0)&&(ScicRegs.SCIFFTX.bit.TXFFST == 0)/*&&(ScicRegs.SCICTL2.bit.TXEMPTY == 1)*/) //�������񣬷������
         {
        	pStu->SendTrsformDelayTim = 2;
         	ScicRegs.SCIFFTX.bit.TXFIFORESET = 0;
//         	ScicRegs.SCIFFTX.bit.SCIRST = 0;
         	ScicRegs.SCIFFTX.bit.TXFFIENA = 0;					//���ж�
         	ScicRegs.SCIFFTX.bit.TXFIFORESET = 1;
//         	ScicRegs.SCIFFTX.bit.SCIRST = 1;
//			vMBMasterPortSerialEnable(pStu, TRUE, FALSE );
			pStu->eMBSndState = STATE_M_TX_XFWR;   //���������ڴ���������ȴ�����״̬
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
/*��������
*1:�ӻ��������һ֡���ݺ󣬽���״̬��eRcvStateΪSTATE_RX_RCV��
*2:�ϱ������յ����ġ��¼�(EV_FRAME_RECEIVED)
*3:��ֹ3.5T��ʱ�������ý���״̬��eRcvState״̬ΪSTATE_RX_IDLE����;
*/
BOOL
xMBMasterRTUTimerExpired( struct ModbusSlaveStru *pStu)
{
	BOOL xNeedPoll = FALSE;

	switch (pStu->eMBRcvState)
	{
		/* Timer t35 expired. Startup phase is finished.
		 * ��ʱ��t35��ʱ.�����׶ν���.
		 */
	/*�ϱ�modbusЭ��ջ���¼�״̬��poll����,EV_READY:��ʼ������¼�*/
	case STATE_M_RX_INIT:
		xNeedPoll = xMBMasterPortEventPost(pStu,EV_MASTER_READY);/*�ϱ�Э��ջ�¼�,��ʼ������¼�*/
		break;

		/* A frame was received and t35 expired. Notify the listener that
		 * a new frame was received.
		 * �յ�һ֡��t35���ڡ�֪ͨ�������յ�����֡.
		 */
	case STATE_M_RX_RCV:            /*һ֡���ݽ������*/
		xNeedPoll = xMBMasterPortEventPost(pStu,EV_MASTER_FRAME_RECEIVED);/*�ϱ�Э��ջ�¼�,���յ�һ֡����������*/
		break;

		/* An error occured while receiving the frame. */
	case STATE_M_RX_ERROR:
		vMBMasterSetErrorType(pStu,EV_ERROR_RECEIVE_DATA);
		xNeedPoll = xMBMasterPortEventPost( pStu,EV_MASTER_ERROR_PROCESS );/*�ϱ�Э��ջ�¼�,֡�������*/
		break;

		/* Function called in an illegal state.
		 * �����ڷǷ�״̬�µ���
		 */
	default:
//		assert(
//				( pStu->eMBRcvState == STATE_M_RX_INIT ) || ( pStu->eMBRcvState == STATE_M_RX_RCV ) ||
//				( pStu->eMBRcvState == STATE_M_RX_ERROR ) || ( pStu->eMBRcvState == STATE_M_RX_IDLE ));
		break;
	}
	/*�����յ�һ֡���ݺ󣬽�ֹ3.5T��ʱ����ֻ��������һ֡���ݿ�ʼ����ʼ��ʱ*/
	pStu->eMBRcvState = STATE_M_RX_IDLE;    /*������һ֡���ݣ�������״̬Ϊ����*/

	switch (pStu->eMBSndState)
	{
		/* A frame was send finish and convert delay or respond timeout expired.
		 * If the frame is broadcast,The master will idle,and if the frame is not
		 * broadcast.Notify the listener process error.
		 * ֡������ɺ�ת���ӳٻ���Ӧ��ʱ���ڡ�
		 * ���֡�ǹ㲥�ģ����������У����֡���ǹ㲥��.֪ͨ���������̴���
		 */
	case STATE_M_TX_XFWR:     //��������ڴ���������ȴ�����״̬
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
	 * �����ʱ��ģʽ��ת���ӳ٣���ô����״̬�����EV_MASTER_EXECUTE״̬.
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

