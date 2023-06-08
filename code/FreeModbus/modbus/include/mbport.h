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
 * File: $Id: mbport.h,v 1.17 2006/12/07 22:10:34 wolti Exp $
 *            mbport.h,v 1.60 2013/08/17 11:42:56 Armink Add Master Functions  $
 */
#include "Modbus.h"
#ifndef _MB_PORT_H
#define _MB_PORT_H

#ifdef __cplusplus
PR_BEGIN_EXTERN_C
#endif


/* ----------------------- Type definitions ---------------------------------*/

//typedef enum
//{
//    EV_READY,                   /*!< Startup finished. */
//    EV_FRAME_RECEIVED,          /*!< Frame received. */
//    EV_EXECUTE,                 /*!< Execute function. */
//    EV_FRAME_SENT               /*!< Frame sent. */
//} eEventstate;

//typedef enum
//{
//    EV_MASTER_READY                    = 1<<0,  /*!< Startup finished. */
//    EV_MASTER_FRAME_RECEIVED           = 1<<1,  /*!< Frame received. */
//    EV_MASTER_EXECUTE                  = 1<<2,  /*!< Execute function. */
//    EV_MASTER_FRAME_SENT               = 1<<3,  /*!< Frame sent. */
//    EV_MASTER_ERROR_PROCESS            = 1<<4,  /*!< Frame error process. */
//    EV_MASTER_PROCESS_SUCESS           = 1<<5,  /*!< Request process success. */
//    EV_MASTER_ERROR_RESPOND_TIMEOUT    = 1<<6,  /*!< Request respond timeout. */
//    EV_MASTER_ERROR_RECEIVE_DATA       = 1<<7,  /*!< Request receive data error. */
//    EV_MASTER_ERROR_EXECUTE_FUNCTION   = 1<<8,  /*!< Request execute function error. */
//} eMBMasterEventType;

/*! \ingroup modbus
 * \brief Parity used for characters in serial mode.
 *
 * The parity which should be applied to the characters sent over the serial
 * link. Please note that this values are actually passed to the porting
 * layer and therefore not all parity modes might be available.
 */
typedef enum
{
    MB_PAR_NONE,                /*!< No parity. */    //无奇偶校验
    MB_PAR_ODD,                 /*!< Odd parity. */   //奇校验
    MB_PAR_EVEN                 /*!< Even parity. */  //偶校验
} eMBParity;




typedef enum
{
    EV_ERROR_RESPOND_TIMEOUT,         /*!< Slave respond timeout. */
    EV_ERROR_RECEIVE_DATA,            /*!< Receive frame data erroe. */
    EV_ERROR_EXECUTE_FUNCTION,        /*!< Execute function error. */
} eMBMasterErrorEventType;

/*******************************Main  Control********************************************/
void InitEnableModbus_Slaver(struct ModbusSlaveStru *pStu,UCHAR SlaveID,Uint32 Baund);
void eMBModbus_Slaver_poll(struct ModbusSlaveStru *pStu);

void InitModbusSCI(void);
void InitEnableModbus_Master(struct ModbusSlaveStru *pStu,Uint32 Baund);
void eMBModbus_Master_poll(struct ModbusSlaveStru *pStu);

/****************************************************************************************/

/* ----------------------- Supporting functions -----------------------------*/
BOOL            xMBPortEventInit( struct ModbusSlaveStru *pStu );

BOOL            xMBPortEventPost( struct ModbusSlaveStru *pStu, eEventstate eEvent );

BOOL            xMBPortEventGet(  struct ModbusSlaveStru *pStu, eEventstate* eEvent );

BOOL            xMBMasterPortEventInit( struct ModbusSlaveStru *pStu );

BOOL            xMBMasterPortEventPost( struct ModbusSlaveStru *pStu, eEventstate eEvent );

BOOL            xMBMasterPortEventGet(  struct ModbusSlaveStru *pStu, eEventstate* eEvent );

void            vMBMasterOsResInit( void );

BOOL            xMBMasterRunResTake( long time );

void            vMBMasterRunResRelease( void );

/* ----------------------- Serial port functions ----------------------------*/

BOOL            xMBPortSerialInit( struct ModbusSlaveStru *pStu, ULONG ulBaudRate,
                                   UCHAR ucDataBits, eMBParity eParity );

BOOL xMBMasterPortSerialInit( struct ModbusSlaveStru *pStu, ULONG ulBaudRate,
        UCHAR ucDataBits, eMBParity eParity );

void            vMBPortClose( void );

void            xMBPortSerialClose( void );

void            vMBPortSerialEnable(struct ModbusSlaveStru *pStu, BOOL xRxEnable, BOOL xTxEnable );

inline BOOL     xMBPortSerialGetByte(struct ModbusSlaveStru *pStu, CHAR * pucByte );
BOOL            xMBMasterPortSerialGetByte(struct ModbusSlaveStru *pStu, UCHAR * pucByte );

inline BOOL     xMBPortSerialPutByte( CHAR ucByte );
BOOL            xMBMasterPortSerialPutByte( UCHAR ucByte );
void            vMBMasterPortSerialEnable(struct ModbusSlaveStru *pStu, BOOL xRxEnable, BOOL xTxEnable );

/* ----------------------- Timers functions ---------------------------------*/
BOOL            xMBPortTimersInit( USHORT usTimeOut50us );
BOOL            xMBMasterPortTimersInit( USHORT usTimeOut50us );

void            xMBPortTimersClose( void );

void            vMBPortTimersEnable( struct ModbusSlaveStru *pStu );
void     vMBMasterPortTimersEnable( struct ModbusSlaveStru *pStu );
void            vMBMasterPortClose( void );
void     vMBMasterPortTimersDisable( struct ModbusSlaveStru *pStu );
void            vMBPortTimersDisable( struct ModbusSlaveStru *pStu );

void            vMBPortTimersDelay( USHORT usTimeOutMS );

 void     vMBMasterPortTimersConvertDelayEnable( void );

 void     vMBMasterPortTimersRespondTimeoutEnable( void );
 /* ----------------------- APF ---------------------------------*/
 void ModBusSlaveTimeCal(struct ModbusSlaveStru *pStu);
 void ModBusMasterTimeCal(struct ModbusSlaveStru *pStu);
 void ModBusSlaveTimEnd(struct ModbusSlaveStru *pStu);
 void ModBusMasterTimEnd(struct ModbusSlaveStru *pStu);


/* ----------------------- Callback for the protocol stack ------------------*/

/*!
 * \brief Callback function for the porting layer when a new byte is
 *   available.
 *
 * Depending upon the mode this callback function is used by the RTU or
 * ASCII transmission layers. In any case a call to xMBPortSerialGetByte()
 * must immediately return a new character.
 *
 * \return <code>TRUE</code> if a event was posted to the queue because
 *   a new byte was received. The port implementation should wake up the
 *   tasks which are currently blocked on the eventqueue.
 */
extern          BOOL( *pxMBFrameCBByteReceived ) ( struct ModbusSlaveStru *pStu );

extern          BOOL( *pxMBFrameCBTransmitterEmpty ) ( struct ModbusSlaveStru *pStu );

extern          BOOL( *pxMBPortCBTimerExpired ) ( struct ModbusSlaveStru *pStu );

extern          BOOL( *pxMBMasterFrameCBByteReceived ) ( struct ModbusSlaveStru *pStu );

extern          BOOL( *pxMBMasterFrameCBTransmitterEmpty ) ( struct ModbusSlaveStru *pStu );

extern          BOOL( *pxMBMasterPortCBTimerExpired ) ( struct ModbusSlaveStru *pStu );
/* ----------------------- TCP port functions -------------------------------*/
BOOL            xMBTCPPortInit( USHORT usTCPPort );

void            vMBTCPPortClose( void );

void            vMBTCPPortDisable( void );

BOOL            xMBTCPPortGetRequest( UCHAR **ppucMBTCPFrame, USHORT * usTCPLength );

BOOL            xMBTCPPortSendResponse( const UCHAR *pucMBTCPFrame, USHORT usTCPLength );

#ifdef __cplusplus
PR_END_EXTERN_C
#endif
#endif
