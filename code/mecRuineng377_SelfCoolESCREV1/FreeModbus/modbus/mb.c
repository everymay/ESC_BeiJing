
/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbconfig.h"
#include "mbframe.h"
#include "mbproto.h"
#include "mbfunc.h"
#include "mbport.h"
#include "Modbus.h"
#if MB_SLAVE_RTU_ENABLED == 1
#include "mbrtu.h"
#endif
#if MB_SLAVE_ASCII_ENABLED == 1
#include "mbascii.h"
#endif
#if MB_SLAVE_TCP_ENABLED == 1
#include "mbtcp.h"
#endif

#ifndef MB_PORT_HAS_CLOSE
#define MB_PORT_HAS_CLOSE 0
#endif
Uint16 SendStateEventWifi = 0;
Uint16 pucSndBuffer0[]={65,84,43,67,73,80,83,69,78,68,61,48,44,56,13,10};
Uint16 pucSndBuffer1[]={65,84,43,67,87,77,79,68,69,61,51,13,10};   //使用AT指令对WIFI模块下发数据
Uint16 pucSndBuffer2[]={65,84,43,67,73,80,77,85,88,61,49,13,10};
Uint16 pucSndBuffer3[]={65,84,43,67,73,80,83,69,82,86,69,82,61,49,13,10};
/* ----------------------- Static variables ---------------------------------*/

static UCHAR    ucMBAddress;
//static eMBMode  eMBCurrentMode;

/* Functions pointer which are initialized in eMBInit( ). Depending on the
 * mode (RTU or ASCII) the are set to the correct implementations.
 */
static peMBFrameSend peMBFrameSendCur;
static pvMBFrameStart pvMBFrameStartCur;
//static pvMBFrameStop pvMBFrameStopCur;

static peMBFrameReceive peMBFrameReceiveCur;
//static pvMBFrameClose pvMBFrameCloseCur;

/* Callback functions required by the porting layer. They are called when
 * an external event has happend which includes a timeout or the reception
 * or transmission of a character.
 */
BOOL( *pxMBFrameCBByteReceived ) ( struct ModbusSlaveStru *pStu );
BOOL( *pxMBFrameCBTransmitterEmpty ) ( struct ModbusSlaveStru *pStu );
BOOL( *pxMBPortCBTimerExpired ) ( struct ModbusSlaveStru *pStu );

BOOL( *pxMBFrameCBReceiveFSMCur ) ( void );
BOOL( *pxMBFrameCBTransmitFSMCur ) ( void );

/* An array of Modbus functions handlers which associates Modbus function
 * codes with implementing functions.
 * 一组Modbus函数处理器，将Modbus函数代码与实现函数关联起来。
 */
static xMBFunctionHandler xFuncHandlers[MB_FUNC_HANDLERS_MAX] = {
#if MB_FUNC_OTHER_REP_SLAVEID_ENABLED > 0
    {MB_FUNC_OTHER_REPORT_SLAVEID, eMBFuncReportSlaveID},
#endif
#if MB_FUNC_READ_INPUT_ENABLED > 0
    {MB_FUNC_READ_INPUT_REGISTER, eMBFuncReadInputRegister},     //功能码04  读取输入寄存器(一个或者多个)
#endif
#if MB_FUNC_READ_HOLDING_ENABLED > 0
    {MB_FUNC_READ_HOLDING_REGISTER, eMBFuncReadHoldingRegister}, //功能码03  读取保持寄存器(一个或者多个)
#endif
#if MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0
    {MB_FUNC_WRITE_MULTIPLE_REGISTERS, eMBFuncWriteMultipleHoldingRegister},  //功能码16  写多个保持寄存器
#endif
#if MB_FUNC_WRITE_HOLDING_ENABLED > 0
    {MB_FUNC_WRITE_REGISTER, eMBFuncWriteHoldingRegister},         //功能码06   写单个保持寄存器
#endif
#if MB_FUNC_READWRITE_HOLDING_ENABLED > 0
    {MB_FUNC_READWRITE_MULTIPLE_REGISTERS, eMBFuncReadWriteMultipleHoldingRegister}, //功能码23 读/写多个保持寄存器
#endif
#if MB_FUNC_READ_COILS_ENABLED > 0
    {MB_FUNC_READ_COILS, eMBFuncReadCoils},
#endif
#if MB_FUNC_WRITE_COIL_ENABLED > 0                              //功能码05     写单个线圈
    {MB_FUNC_WRITE_SINGLE_COIL, eMBFuncWriteCoil},
#endif
#if MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0
    {MB_FUNC_WRITE_MULTIPLE_COILS, eMBFuncWriteMultipleCoils},
#endif
#if MB_FUNC_READ_DISCRETE_INPUTS_ENABLED > 0                    //功能码02    读离散输入
    {MB_FUNC_READ_DISCRETE_INPUTS, eMBFuncReadDiscreteInputs},
#endif
	{MB_FUNC_APF_SCENEREAD, APFWaveReadFunction},//0x68
	{MB_FUNC_APF_SOEDEFINED, APFOffWaveReadFunction},//0x69
	{MB_FUNC_APF_SOEDATAREAD, APFSoeReadFunction},//0x6A
	{MB_FUNC_APF_USERDEFINEREAD, APFUserDefinedReadFunction},//0x66
	{MB_FUNC_APF_USERDEFINEWRITE, APFUserDefinedWriteFunction},//0x67
};
/* ----------------------- Start implementation -----------------------------*/
/*函数功能：
*1:实现RTU模式和ASCALL模式的协议栈初始化;
*2:完成协议栈核心函数指针的赋值，包括Modbus协议栈的使能和禁止、报文的接收和响应、3.5T定时器中断回调函数、串口发送和接收中断回调函数;
*3:eMBRTUInit完成RTU模式下串口和3.5T定时器的初始化，需用户自己移植;
*4:设置Modbus协议栈的模式eMBCurrentMode为MB_RTU，设置Modbus协议栈状态eMBState为STATE_DISABLED;
*/
eMBErrorCode
eMBInit( struct ModbusSlaveStru *pStu,eMBMode eMode, UCHAR ucSlaveAddress, UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity )
{
    eMBErrorCode    eStatus = MB_ENOERR;    //错误状态初始值

    /* check preconditions */  //验证从机地址
    if( ( ucSlaveAddress == MB_ADDRESS_BROADCAST ) ||
        ( ucSlaveAddress < MB_ADDRESS_MIN ) || ( ucSlaveAddress > MB_ADDRESS_MAX ) )
    {
        eStatus = MB_EINVAL;  //非法参数
    }
    else
    {
    	pStu->ucMBAddress = ucSlaveAddress;  /*从机地址的赋值*/

        switch ( eMode )   //选择RTU模式还是ASCII模式
        {
#if MB_SLAVE_RTU_ENABLED > 0
        case MB_RTU:
                peMBFrameSendCur = eMBRTUSend;//modbus从机响应函数
                peMBFrameReceiveCur = eMBRTUReceive; //modbus报文接收函数
                pxMBFrameCBByteReceived = xMBRTUReceiveFSM;  //接收状态机/*串口接收中断最终调用此函数接收数据*/
                pxMBFrameCBTransmitterEmpty = xMBRTUTransmitFSM;//发送状态机/*串口发送中断最终调用此函数发送数据*/
				pxMBPortCBTimerExpired = xMBRTUTimerT35Expired;//报文到达间隔检查/*定时器中断函数最终调用此函数完成定时器中断*/
				pvMBFrameStartCur = eMBRTUStart;  /*使能modbus协议栈*/
//				pvMBFrameStopCur = eMBRTUStop;    /*禁用modbus协议栈*/
//				pvMBFrameCloseCur = MB_PORT_HAS_CLOSE ? vMBPortClose : NULL;
				eStatus = eMBRTUInit( pStu,ucMBAddress, ucPort, ulBaudRate, eParity );//初始化RTU
            break;
#endif
#if MB_SLAVE_ASCII_ENABLED > 0
        case MB_ASCII:
            pvMBFrameStartCur = eMBASCIIStart;
            pvMBFrameStopCur = eMBASCIIStop;
            peMBFrameSendCur = eMBASCIISend;
            peMBFrameReceiveCur = eMBASCIIReceive;
            pvMBFrameCloseCur = MB_PORT_HAS_CLOSE ? vMBPortClose : NULL;
            pxMBFrameCBByteReceived = xMBASCIIReceiveFSM;
            pxMBFrameCBTransmitterEmpty = xMBASCIITransmitFSM;
            pxMBPortCBTimerExpired = xMBASCIITimerT1SExpired;

            eStatus = eMBASCIIInit( ucMBAddress, ucPort, ulBaudRate, eParity );
            break;
#endif
        default:
            eStatus = MB_EINVAL;
            break;
        }

        if( eStatus == MB_ENOERR )
        {
            if( !xMBPortEventInit( pStu ) )
            {
                /* port dependent event module initalization failed.
                 * 端口相关事件模块初始化失败
                 */
                eStatus = MB_EPORTERR;   //移植层错误
            }
            else
            {
//                eMBCurrentMode = eMode;  //设定RTU模式
            	pStu->eMBState = STATE_DISABLED;  //modbus协议栈初始化状态,在此初始化为禁止
            }
        }
    }
    return eStatus;
}

#if MB_TCP_ENABLED > 0
eMBErrorCode
eMBTCPInit( USHORT ucTCPPort )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    if( ( eStatus = eMBTCPDoInit( ucTCPPort ) ) != MB_ENOERR )
    {
        eMBState = STATE_DISABLED;
    }
    else if( !xMBPortEventInit(  ) )
    {
        /* Port dependent event module initalization failed. */
        eStatus = MB_EPORTERR;
    }
    else
    {
        pvMBFrameStartCur = eMBTCPStart;
        pvMBFrameStopCur = eMBTCPStop;
        peMBFrameReceiveCur = eMBTCPReceive;
        peMBFrameSendCur = eMBTCPSend;
        pvMBFrameCloseCur = MB_PORT_HAS_CLOSE ? vMBTCPPortClose : NULL;
        ucMBAddress = MB_TCP_PSEUDO_ADDRESS;
        eMBCurrentMode = MB_TCP;
        eMBState = STATE_DISABLED;
    }
    return eStatus;
}
#endif

eMBErrorCode eMBRegisterCB( UCHAR ucFunctionCode, pxMBFunctionHandler pxHandler )
{
    int             i;
    eMBErrorCode    eStatus;

    if( ( 0 < ucFunctionCode ) && ( ucFunctionCode <= 127 ) )
    {
        ENTER_CRITICAL_SECTION(  );
        if( pxHandler != NULL )
        {
            for( i = 0; i < MB_FUNC_HANDLERS_MAX; i++ )
            {
                if( ( xFuncHandlers[i].pxHandler == NULL ) ||
                    ( xFuncHandlers[i].pxHandler == pxHandler ) )
                {
                    xFuncHandlers[i].ucFunctionCode = ucFunctionCode; /*根据报文中的功能码，处理报文*/
                    xFuncHandlers[i].pxHandler = pxHandler;
                    break;
                }
            }
            eStatus = ( i != MB_FUNC_HANDLERS_MAX ) ? MB_ENOERR : MB_ENORES;
        }
        else
        {
            for( i = 0; i < MB_FUNC_HANDLERS_MAX; i++ )
            {
                if( xFuncHandlers[i].ucFunctionCode == ucFunctionCode ) /*根据报文中的功能码，处理报文*/
                {
                    xFuncHandlers[i].ucFunctionCode = 0;
                    xFuncHandlers[i].pxHandler = NULL;
                    break;
                }
            }
            /* Remove can't fail. */
            eStatus = MB_ENOERR;
        }
        EXIT_CRITICAL_SECTION(  );
    }
    else
    {
        eStatus = MB_EINVAL;
    }
    return eStatus;
}
//eMBErrorCode
//eMBClose( void )
//{
//    eMBErrorCode    eStatus = MB_ENOERR;
//
//    if( eMBState == STATE_DISABLED )
//    {
//        if( pvMBFrameCloseCur != NULL )
//        {
//            pvMBFrameCloseCur(  );
//        }
//    }
//    else
//    {
//        eStatus = MB_EILLSTATE;
//    }
//    return eStatus;
//}

eMBErrorCode
eMBEnable( struct ModbusSlaveStru *pStu )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    if( pStu->eMBState == STATE_DISABLED )
    {
        /* Activate the protocol stack.
         * 激活协议栈.
         */
        pvMBFrameStartCur( pStu );   /*pvMBFrameStartCur = eMBRTUStart;调用eMBRTUStart函数*/
        pStu->eMBState = STATE_ENABLED;
    }
    else
    {
        eStatus = MB_EILLSTATE;   //协议栈处于非法状态.
    }
    return eStatus;
}

//eMBErrorCode
//eMBDisable( void )
//{
//    eMBErrorCode    eStatus;
//
//    if( eMBState == STATE_ENABLED )
//    {
//        pvMBFrameStopCur(  );
//        eMBState = STATE_DISABLED;
//        eStatus = MB_ENOERR;
//    }
//    else if( eMBState == STATE_DISABLED )
//    {
//        eStatus = MB_ENOERR;
//    }
//    else
//    {
//        eStatus = MB_EILLSTATE;
//    }
//    return eStatus;
//}
/*函数功能:
*1:检查协议栈状态是否使能，eMBState初值为STATE_NOT_INITIALIZED，在eMBInit()函数中被赋值为STATE_DISABLED,在eMBEnable函数中被赋值为STATE_ENABLE;
*2:轮询EV_FRAME_RECEIVED事件发生，若EV_FRAME_RECEIVED事件发生，接收一帧报文数据，上报EV_EXECUTE事件，解析一帧报文，响应(发送)一帧数据给主机;
*/
eMBErrorCode
eMBPoll( struct ModbusSlaveStru *pStu )
{
	UCHAR   	   *ucMBFrame;   //接收和发送报文数据缓存区
    USHORT   		usLength;    //报文长度
    UCHAR    		ucFunctionCode;//功能码
    eEventstate     eEvent;      //事件标志枚举
    eMBException 	eException;  //错误码响应枚举
    eMBErrorCode    eStatus = MB_ENOERR;     //modbus协议栈错误码
    int i;

    /* Check if the protocol stack is ready. */
    if( pStu->eMBState != STATE_ENABLED )  //检查协议栈是否使能
    {
        return MB_EILLSTATE;      //协议栈未使能，返回协议栈无效错误码
    }
	if(pStu->pReg->SCIRXST.all & 0x00bc){   //软件故障复位
		pStu->pReg->SCICTL1.bit.SWRESET = 0;
		asm(" RPT	 #10 || NOP ");
		pStu->pReg->SCICTL1.bit.SWRESET = 1;
	}
    /* Check if there is a event available. If not return control to caller.
     * Otherwise we will handle the event.
     * 检查是否有可用的事件。如果没有，则将控制权返回给调用者。否则我们将处理事件。*/
    if( xMBPortEventGet(pStu, &eEvent ) == TRUE )   //查询哪个事件发生
    {
        switch ( eEvent )
        {
        case EV_READY:
            break;

        case EV_FRAME_RECEIVED:  /*接收到一帧数据，此事件发生*/
        	eStatus = peMBFrameReceiveCur(pStu, &(pStu->ucRcvAddress), &ucMBFrame, &usLength );
            if( eStatus == MB_ENOERR )   //报文长度和CRC校验正确
            {
                /* Check if the frame is for us. If not ignore the frame.
                 * 判断接收到的报文是否可用, 如果是,处理报文数据*/
                if( ( pStu->ucRcvAddress == pStu->ucMBAddress ) || ( pStu->ucRcvAddress == MB_ADDRESS_BROADCAST ) )
                {
                    ( void )xMBPortEventPost( pStu,EV_EXECUTE );  //修改事件标志为EV_EXECUTE执行事件
                    pStu->ComuState = 1;
                }
            }
            break;

        case EV_EXECUTE:                                        //对接收到的报文进行处理事件
        	usLength = pStu->usRcvBufferPos-3;
        	ucMBFrame = (Uint16 *)(pStu->ModbusRcvSedBuff + 1);
            ucFunctionCode = ucMBFrame[MB_PDU_FUNC_OFF];
            eException = MB_EX_ILLEGAL_FUNCTION;
            for( i = 0; i < MB_FUNC_HANDLERS_MAX; i++ )
            {
                /* No more function handlers registered. Abort. */
                if( xFuncHandlers[i].ucFunctionCode == 0 ){
                    break;
                }
                else if( xFuncHandlers[i].ucFunctionCode == ucFunctionCode ){
                    eException = xFuncHandlers[i].pxHandler(ucMBFrame, &usLength );
                    break;
                }
            }

            /* If the request was not sent to the broadcast address we
             * return a reply. */
            if( pStu->ucRcvAddress != MB_ADDRESS_BROADCAST )
            {
                if( eException != MB_EX_NONE )
                {
                    /* An exception occured. Build an error frame. */
                    usLength = 0;
                    ucMBFrame[usLength++] = ( UCHAR )( ucFunctionCode | MB_FUNC_ERROR );
                    ucMBFrame[usLength++] = eException;
                }
//                xMBPortEventPost( &SCIC,EV_FRAME_SENT0 );
                SendStateEventWifi = 0;
                eStatus = peMBFrameSendCur( pStu,pStu->ucMBAddress, ucMBFrame, usLength );
            }
            break;
        case EV_FRAME_SENT:
            break;
//        case EV_FRAME_SENT0:
//            SendStateEventWifi = 1;
//            usLength = 16;
//            peMBFrameSendCur( pStu,pStu->ucMBAddress, pucSndBuffer0, usLength );
//            break;
//        case EV_FRAME_SENT1:
//            SendStateEventWifi = 1;
//            usLength = 13;
//            peMBFrameSendCur( pStu,pStu->ucMBAddress, pucSndBuffer1, usLength );
//            break;
//        case EV_FRAME_SENT2:
//            SendStateEventWifi = 1;
//            usLength = 13;
//            peMBFrameSendCur( pStu,pStu->ucMBAddress, pucSndBuffer2, usLength );
//            break;
//        case EV_FRAME_SENT3:
//            SendStateEventWifi = 1;
//            usLength = 16;
//            peMBFrameSendCur( pStu,pStu->ucMBAddress, pucSndBuffer3, usLength );
//            break;
        }
    }
    return MB_ENOERR;
}

