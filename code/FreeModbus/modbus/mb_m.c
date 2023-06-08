/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/

#include "mb.h"
#include "mb_m.h"
#include "mbconfig.h"
#include "mbframe.h"
#include "mbproto.h"
#include "mbfunc.h"
#include "mbport.h"
#include "VariblesRef.h"

#if MB_MASTER_RTU_ENABLED == 1
#include "mbrtu.h"
#endif
#if MB_MASTER_ASCII_ENABLED == 1
#include "mbascii.h"
#endif
#if MB_MASTER_TCP_ENABLED == 1
#include "mbtcp.h"
#endif

#ifndef MB_PORT_HAS_CLOSE
#define MB_PORT_HAS_CLOSE 0
#endif

/* ----------------------- Static variables ---------------------------------*/

//static UCHAR    ucMBMasterDestAddress;
static BOOL     xMBRunInMasterMode = FALSE;
static eMBMasterErrorEventType eMBMasterCurErrorType;

//static enum
//{
//    STATE_ENABLED,
//    STATE_DISABLED,
//    STATE_NOT_INITIALIZED
//} eMBState = STATE_NOT_INITIALIZED;

/* Functions pointer which are initialized in eMBInit( ). Depending on the
 * mode (RTU or ASCII) the are set to the correct implementations.
 * Using for Modbus Master,Add by Armink 20130813
 */
static peMBFrameSend peMBMasterFrameSendCur;
static pvMBFrameStart pvMBMasterFrameStartCur;
//static pvMBFrameStop pvMBMasterFrameStopCur;
static peMBFrameReceive peMBMasterFrameReceiveCur;
//static pvMBFrameClose pvMBMasterFrameCloseCur;

/* Callback functions required by the porting layer. They are called when
 * an external event has happend which includes a timeout or the reception
 * or transmission of a character.
 * Using for Modbus Master,Add by Armink 20130813
 */
BOOL( *pxMBMasterFrameCBByteReceived ) ( struct ModbusSlaveStru *pStu );
BOOL( *pxMBMasterFrameCBTransmitterEmpty ) ( struct ModbusSlaveStru *pStu );
BOOL( *pxMBMasterPortCBTimerExpired ) ( struct ModbusSlaveStru *pStu );

BOOL( *pxMBMasterFrameCBReceiveFSMCur ) ( void );
BOOL( *pxMBMasterFrameCBTransmitFSMCur ) ( void );

/* An array of Modbus functions handlers which associates Modbus function
 * codes with implementing functions.
 * 一组Modbus函数处理器，将Modbus函数代码与实现函数关联起来.
 */
static xMBMasterFunctionHandler xMasterFuncHandlers[MB_FUNC_HANDLERS_MAX] = {
#if MB_FUNC_OTHER_REP_SLAVEID_ENABLED > 0
	//TODO Add Master function define
    {MB_FUNC_OTHER_REPORT_SLAVEID, eMBFuncReportSlaveID},
#endif
#if MB_FUNC_READ_INPUT_ENABLED > 0
    {MB_FUNC_READ_INPUT_REGISTER, eMBMasterFuncReadInputRegister}, //功能码04,读取输入寄存器
#endif
#if MB_FUNC_READ_HOLDING_ENABLED > 0
    {MB_FUNC_READ_HOLDING_REGISTER, eMBMasterFuncReadHoldingRegister},//功能码03,读取保持寄存器
#endif
#if MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0
    {MB_FUNC_WRITE_MULTIPLE_REGISTERS, eMBMasterFuncWriteMultipleHoldingRegister},//功能码16,写多个保持寄存器
#endif
#if MB_FUNC_WRITE_HOLDING_ENABLED > 0
    {MB_FUNC_WRITE_REGISTER, eMBMasterFuncWriteHoldingRegister},//功能码06,写单个保持寄存器
#endif
#if MB_FUNC_READWRITE_HOLDING_ENABLED > 0
    {MB_FUNC_READWRITE_MULTIPLE_REGISTERS, eMBMasterFuncReadWriteMultipleHoldingRegister},//功能码23,读写多个保持
#endif
#if MB_FUNC_READ_COILS_ENABLED > 0
    {MB_FUNC_READ_COILS, eMBMasterFuncReadCoils},
#endif
#if MB_FUNC_WRITE_COIL_ENABLED > 0
    {MB_FUNC_WRITE_SINGLE_COIL, eMBMasterFuncWriteCoil},
#endif
#if MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0
    {MB_FUNC_WRITE_MULTIPLE_COILS, eMBMasterFuncWriteMultipleCoils},
#endif
#if MB_FUNC_READ_DISCRETE_INPUTS_ENABLED > 0
    {MB_FUNC_READ_DISCRETE_INPUTS, eMBMasterFuncReadDiscreteInputs},
#endif
};
/* ----------------------- Start implementation -----------------------------*/
/*函数功能：
*1:实现RTU模式和ASCALL模式的协议栈初始化;
*2:完成协议栈核心函数指针的赋值，包括Modbus协议栈的使能和禁止、报文的接收和响应、3.5T定时器中断回调函数、串口发送和接收中断回调函数;
*3:eMBRTUInit完成RTU模式下串口和3.5T定时器的初始化，需用户自己移植;
*4:设置Modbus协议栈的模式eMBCurrentMode为MB_RTU，设置Modbus协议栈状态eMBState为STATE_DISABLED;
*/
eMBErrorCode
eMBMasterInit(struct ModbusSlaveStru *pStu, eMBMode eMode, UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity )
{
    eMBErrorCode    eStatus = MB_ENOERR; //错误状态初始值

	switch (eMode)
	{
#if MB_MASTER_RTU_ENABLED > 0
	case MB_RTU:
		peMBMasterFrameSendCur = eMBMasterRTUSend;  //modbus主机响应函数
		peMBMasterFrameReceiveCur = eMBMasterRTUReceive;  //modbus报文接收函数
		pxMBMasterFrameCBByteReceived = xMBMasterRTUReceiveFSM;  //串口接收中断最终调用此函数接收数据(接收状态机)  //5
		pxMBMasterFrameCBTransmitterEmpty = xMBMasterRTUTransmitFSM;  //串口发送中断最终调用此函数发送数据(发送状态机)
		pxMBMasterPortCBTimerExpired = xMBMasterRTUTimerExpired;  //定时器中断函数最终调用次函数完成定时器中断(报文到达间隔检查)   //6
		pvMBMasterFrameStartCur = eMBMasterRTUStart;  //使能主机modbus协议栈  //3
//		pvMBMasterFrameStopCur = eMBMasterRTUStop;    //禁用modbus协议栈
//		pvMBMasterFrameCloseCur = MB_PORT_HAS_CLOSE ? vMBMasterPortClose : NULL;  //接收状态机
		eStatus = eMBMasterRTUInit(pStu, ulBaudRate, eParity);  //初始化RTU 1
		break;
#endif
#if MB_MASTER_ASCII_ENABLED > 0
		case MB_ASCII:
		pvMBMasterFrameStartCur = eMBMasterASCIIStart;
		pvMBMasterFrameStopCur = eMBMasterASCIIStop;
		peMBMasterFrameSendCur = eMBMasterASCIISend;
		peMBMasterFrameReceiveCur = eMBMasterASCIIReceive;
		pvMBMasterFrameCloseCur = MB_PORT_HAS_CLOSE ? vMBMasterPortClose : NULL;
		pxMBMasterFrameCBByteReceived = xMBMasterASCIIReceiveFSM;
		pxMBMasterFrameCBTransmitterEmpty = xMBMasterASCIITransmitFSM;
		pxMBMasterPortCBTimerExpired = xMBMasterASCIITimerT1SExpired;

		eStatus = eMBMasterASCIIInit(ucPort, ulBaudRate, eParity );
		break;
#endif
	default:
		eStatus = MB_EINVAL;    //非法参数
		break;
	}

	if (eStatus == MB_ENOERR)
	{
		if (!xMBMasterPortEventInit(pStu))
		{
			/* port dependent event module initalization failed.
			 * 端口相关事件模块初始化失败
			 */
			eStatus = MB_EPORTERR;  //移植层错误
		}
		else
		{   //设定当前状态
			pStu->eMBState = STATE_DISABLED;  //modbus协议栈初始化状态,在此初始化为禁止
		}
		/* initialize the OS resource for modbus master. */
//		vMBMasterOsResInit();
	}
	return eStatus;
}

//eMBErrorCode
//eMBMasterClose( void )
//{
//    eMBErrorCode    eStatus = MB_ENOERR;
//
//    if( pStu->eMBState == STATE_DISABLED )
//    {
//        if( pvMBMasterFrameCloseCur != NULL )
//        {
//            pvMBMasterFrameCloseCur(  );
//        }
//    }
//    else
//    {
//        eStatus = MB_EILLSTATE;
//    }
//    return eStatus;
//}
/*函数功能
*1:设置Modbus协议栈工作状态eMBState为STATE_ENABLED;
*2:调用pvMBFrameStartCur()函数激活协议栈
*/
eMBErrorCode
eMBMasterEnable( struct ModbusSlaveStru *pStu )     //2
{
    eMBErrorCode    eStatus = MB_ENOERR;

    if( pStu->eMBState == STATE_DISABLED )
    {
        /* Activate the protocol stack.
         * 激活协议栈
         */
        pvMBMasterFrameStartCur(pStu);  //调用eMBMasterRTUStart函数
        pStu->eMBState = STATE_ENABLED;  //使能
    }
    else
    {
        eStatus = MB_EILLSTATE;//协议栈处于非法状态
    }
    return eStatus;
}

//eMBErrorCode
//eMBMasterDisable( struct ModbusSlaveStru *pStu )
//{
//    eMBErrorCode    eStatus;
//
//    if( pStu->eMBState == STATE_ENABLED )
//    {
//        pvMBMasterFrameStopCur( pStu );
//        pStu->eMBState = STATE_DISABLED;
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
*2:轮询EV_FRAME_RECEIVED事件发生，若EV_FRAME_RECEIVED事件发生，接收一帧报文数据，上报EV_EXECUTE事件，解析一帧报文，响应(发送)一帧数据给从机;
*/
eMBErrorCode
eMBMasterPoll( struct ModbusSlaveStru *pStu )    // 4
{
	UCHAR   *ucMBFrame;           //接收和发送报文数据缓存区
	UCHAR   *ucSndFrame;          //发送帧
	UCHAR 	ucFunctionCode;       //功能码
    USHORT   usLength;            //报文长度
    UCHAR	i,j;
    eMBException 	eException;   //错误码响应枚举
    eMBErrorCode    eStatus = MB_ENOERR;  //modbus协议栈错误码
    eEventstate    eEvent;        //事件标志枚举
//    eMBMasterErrorEventType errorType;

    /* Check if the protocol stack is ready.
     * 检查协议栈是否准备就绪
     */
    if( pStu->eMBState != STATE_ENABLED )      //检查协议栈是否使能
    {
        return MB_EILLSTATE;            //协议栈未使能,返回协议栈无效错误码
    }
	if(pStu->pReg->SCIRXST.all & 0x00bc){   //软件故障复位
		pStu->pReg->SCICTL1.bit.SWRESET = 0;
		asm(" RPT	 #10 || NOP ");
		pStu->pReg->SCICTL1.bit.SWRESET = 1;
	}
    /* Check if there is a event available. If not return control to caller.
     * Otherwise we will handle the event.
     * 检查是否有可用的事件.如果没有，则将控制权返回给调用者。否则我们将处理事件。
     */
    if( xMBMasterPortEventGet(pStu, &eEvent ) == TRUE )  //查询哪个事件发生
    {
        switch ( eEvent )
        {
        case EV_MASTER_READY:
            break;

        case EV_MASTER_FRAME_RECEIVED:       //接收到一帧数据,此事件发生
			eStatus = peMBMasterFrameReceiveCur(pStu, &(pStu->ucRcvAddress), &ucMBFrame, &usLength );
			/* Check if the frame is for us. If not ,send an error process event.
			 * 判断接收到的报文数据是否可接受，如果是，处理报文数据;如果不是,发送一个错误处理事件
			 */
			if ( ( eStatus == MB_ENOERR ) && ( pStu->ucRcvAddress == ucMBMasterGetDestAddress(pStu) ) )  //报文长度正确和CRC校验正确,并且从机地址正确
			{
				( void ) xMBMasterPortEventPost( pStu,EV_MASTER_EXECUTE );  //修改事件标志为EV_MASTER_EXECUTE执行事件
			}
			break;

        case EV_MASTER_EXECUTE:                                    //对接收到的报文进行事件处理
        	usLength = pStu->usRcvBufferPos-3;                     //当前接收报文长度减去三个部分(功能码和两个CRC校验,剩下两部分为单元地址位和数据个数位)
        	ucMBFrame = (Uint16 *)(pStu->ModbusRcvSedBuff + 1);    //+1是为了错过第一位地址位.
            ucFunctionCode = ucMBFrame[MB_PDU_FUNC_OFF];           //获取PDU中第一个字节,为功能码
             eException = MB_EX_ILLEGAL_FUNCTION;                  //赋错误码初值为无效的功能码
             /* If receive frame has exception .The receive function code highest bit is 1.
              * 如果接收帧有异常，接收功能码最高位为1.
              */
             if(ucFunctionCode >> 7) {
             	eException = (eMBException)ucMBFrame[MB_PDU_DATA_OFF];//响应数据在PDU中的偏移量
             }
 			else
 			{
 				for (i = 0; i < MB_FUNC_HANDLERS_MAX; i++)
 				{
 					/* No more function handlers registered. Abort.
 					 * 不再注册函数处理程序.中止.
 					 */
 					if (xMasterFuncHandlers[i].ucFunctionCode == 0){
 						break;
 					}
 					else if (xMasterFuncHandlers[i].ucFunctionCode == ucFunctionCode) {   /*根据报文中的功能码，处理报文*/
 						vMBMasterSetCBRunInMasterMode(TRUE);
 						/* If master request is broadcast,
 						 * the master need execute function for all slave.
 						 * 如果主请求被广播，主需要执行所有从的功能.
 						 */
 						if ( xMBMasterRequestIsBroadcast() ) {
 							usLength = usMBMasterGetPDUSndLength(pStu);  //获取Modbus Master发送PDU的缓冲区长度
 							for(j = 1; j <= MB_MASTER_TOTAL_SLAVE_NUM; j++){
 								vMBMasterSetDestAddress(pStu ,j);  //设置Modbus Master发送目的地址
 								eException = xMasterFuncHandlers[i].pxHandler(pStu,ucMBFrame, &usLength); /*对接收到的报文进行解析*/
 							}
 						}
 						else {
 							eException = xMasterFuncHandlers[i].pxHandler(pStu,ucMBFrame, &usLength); /*对接收到的报文进行解析*/
 						}
 						vMBMasterSetCBRunInMasterMode(FALSE);  //设置Modbus Master是否以Master模式运行
 						break;
 					}
 				}
 			}
             /* If master has exception ,Master will send error process.Otherwise the Master is idle.
              * 如果主机有异常，主机将发送错误进程。否则主机处于空闲状态.
              */
             if (eException != MB_EX_NONE) {         /*接收到的报文有错误*/
             	vMBMasterSetErrorType(pStu ,EV_ERROR_EXECUTE_FUNCTION);  //设置Modbus Master当前错误事件类型.
             	( void ) xMBMasterPortEventPost(pStu, EV_MASTER_ERROR_PROCESS );   //帧错误进程.
             }
      //       else {
  //           	vMBMasterCBRequestScuuess( );
 //            	vMBMasterRunResRelease( );
    //         }
            break;

        case EV_MASTER_FRAME_SENT:
        	/* Master is busy now. */
        	vMBMasterGetPDUSndBuf(pStu, &ucMBFrame ,&ucSndFrame);   //获取Modbus Master发送PDU的缓冲区地址指针
			eStatus = peMBMasterFrameSendCur( pStu,ucMBMasterGetDestAddress(pStu), ucMBFrame, usMBMasterGetPDUSndLength(pStu) );  /*modbus主机响应函数,发送响应给从机*/
            break;
     }
    }
    return MB_ENOERR;

}
/* Get whether the Modbus Master is run in master mode.*/
BOOL xMBMasterGetCBRunInMasterMode( void )
{
	return xMBRunInMasterMode;
}
/* Set whether the Modbus Master is run in master mode.*/
void vMBMasterSetCBRunInMasterMode( BOOL IsMasterMode )
{
	xMBRunInMasterMode = IsMasterMode;
}
/* Get Modbus Master send destination address. */
UCHAR ucMBMasterGetDestAddress( struct ModbusSlaveStru *pStu )
{
	return pStu->ucMBAddress;
}
/* Set Modbus Master send destination address. */
void vMBMasterSetDestAddress(struct ModbusSlaveStru *pStu, UCHAR Address )
{
	pStu->ucMBAddress = Address;
}
/* Get Modbus Master current error event type. */
eMBMasterErrorEventType eMBMasterGetErrorType( void )
{
	return eMBMasterCurErrorType;
}
/* Set Modbus Master current error event type. */
void vMBMasterSetErrorType(struct ModbusSlaveStru *pStu, eMBMasterErrorEventType errorType )
{
	eMBMasterCurErrorType = errorType;
}

