
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
Uint16 pucSndBuffer1[]={65,84,43,67,87,77,79,68,69,61,51,13,10};   //ʹ��ATָ���WIFIģ���·�����
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
 * һ��Modbus��������������Modbus����������ʵ�ֺ�������������
 */
static xMBFunctionHandler xFuncHandlers[MB_FUNC_HANDLERS_MAX] = {
#if MB_FUNC_OTHER_REP_SLAVEID_ENABLED > 0
    {MB_FUNC_OTHER_REPORT_SLAVEID, eMBFuncReportSlaveID},
#endif
#if MB_FUNC_READ_INPUT_ENABLED > 0
    {MB_FUNC_READ_INPUT_REGISTER, eMBFuncReadInputRegister},     //������04  ��ȡ����Ĵ���(һ�����߶��)
#endif
#if MB_FUNC_READ_HOLDING_ENABLED > 0
    {MB_FUNC_READ_HOLDING_REGISTER, eMBFuncReadHoldingRegister}, //������03  ��ȡ���ּĴ���(һ�����߶��)
#endif
#if MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0
    {MB_FUNC_WRITE_MULTIPLE_REGISTERS, eMBFuncWriteMultipleHoldingRegister},  //������16  д������ּĴ���
#endif
#if MB_FUNC_WRITE_HOLDING_ENABLED > 0
    {MB_FUNC_WRITE_REGISTER, eMBFuncWriteHoldingRegister},         //������06   д�������ּĴ���
#endif
#if MB_FUNC_READWRITE_HOLDING_ENABLED > 0
    {MB_FUNC_READWRITE_MULTIPLE_REGISTERS, eMBFuncReadWriteMultipleHoldingRegister}, //������23 ��/д������ּĴ���
#endif
#if MB_FUNC_READ_COILS_ENABLED > 0
    {MB_FUNC_READ_COILS, eMBFuncReadCoils},
#endif
#if MB_FUNC_WRITE_COIL_ENABLED > 0                              //������05     д������Ȧ
    {MB_FUNC_WRITE_SINGLE_COIL, eMBFuncWriteCoil},
#endif
#if MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0
    {MB_FUNC_WRITE_MULTIPLE_COILS, eMBFuncWriteMultipleCoils},
#endif
#if MB_FUNC_READ_DISCRETE_INPUTS_ENABLED > 0                    //������02    ����ɢ����
    {MB_FUNC_READ_DISCRETE_INPUTS, eMBFuncReadDiscreteInputs},
#endif
	{MB_FUNC_APF_SCENEREAD, APFWaveReadFunction},//0x68
	{MB_FUNC_APF_SOEDEFINED, APFOffWaveReadFunction},//0x69
	{MB_FUNC_APF_SOEDATAREAD, APFSoeReadFunction},//0x6A
	{MB_FUNC_APF_USERDEFINEREAD, APFUserDefinedReadFunction},//0x66
	{MB_FUNC_APF_USERDEFINEWRITE, APFUserDefinedWriteFunction},//0x67
};
/* ----------------------- Start implementation -----------------------------*/
/*�������ܣ�
*1:ʵ��RTUģʽ��ASCALLģʽ��Э��ջ��ʼ��;
*2:���Э��ջ���ĺ���ָ��ĸ�ֵ������ModbusЭ��ջ��ʹ�ܺͽ�ֹ�����ĵĽ��պ���Ӧ��3.5T��ʱ���жϻص����������ڷ��ͺͽ����жϻص�����;
*3:eMBRTUInit���RTUģʽ�´��ں�3.5T��ʱ���ĳ�ʼ�������û��Լ���ֲ;
*4:����ModbusЭ��ջ��ģʽeMBCurrentModeΪMB_RTU������ModbusЭ��ջ״̬eMBStateΪSTATE_DISABLED;
*/
eMBErrorCode
eMBInit( struct ModbusSlaveStru *pStu,eMBMode eMode, UCHAR ucSlaveAddress, UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity )
{
    eMBErrorCode    eStatus = MB_ENOERR;    //����״̬��ʼֵ

    /* check preconditions */  //��֤�ӻ���ַ
    if( ( ucSlaveAddress == MB_ADDRESS_BROADCAST ) ||
        ( ucSlaveAddress < MB_ADDRESS_MIN ) || ( ucSlaveAddress > MB_ADDRESS_MAX ) )
    {
        eStatus = MB_EINVAL;  //�Ƿ�����
    }
    else
    {
    	pStu->ucMBAddress = ucSlaveAddress;  /*�ӻ���ַ�ĸ�ֵ*/

        switch ( eMode )   //ѡ��RTUģʽ����ASCIIģʽ
        {
#if MB_SLAVE_RTU_ENABLED > 0
        case MB_RTU:
                peMBFrameSendCur = eMBRTUSend;//modbus�ӻ���Ӧ����
                peMBFrameReceiveCur = eMBRTUReceive; //modbus���Ľ��պ���
                pxMBFrameCBByteReceived = xMBRTUReceiveFSM;  //����״̬��/*���ڽ����ж����յ��ô˺�����������*/
                pxMBFrameCBTransmitterEmpty = xMBRTUTransmitFSM;//����״̬��/*���ڷ����ж����յ��ô˺�����������*/
				pxMBPortCBTimerExpired = xMBRTUTimerT35Expired;//���ĵ��������/*��ʱ���жϺ������յ��ô˺�����ɶ�ʱ���ж�*/
				pvMBFrameStartCur = eMBRTUStart;  /*ʹ��modbusЭ��ջ*/
//				pvMBFrameStopCur = eMBRTUStop;    /*����modbusЭ��ջ*/
//				pvMBFrameCloseCur = MB_PORT_HAS_CLOSE ? vMBPortClose : NULL;
				eStatus = eMBRTUInit( pStu,ucMBAddress, ucPort, ulBaudRate, eParity );//��ʼ��RTU
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
                 * �˿�����¼�ģ���ʼ��ʧ��
                 */
                eStatus = MB_EPORTERR;   //��ֲ�����
            }
            else
            {
//                eMBCurrentMode = eMode;  //�趨RTUģʽ
            	pStu->eMBState = STATE_DISABLED;  //modbusЭ��ջ��ʼ��״̬,�ڴ˳�ʼ��Ϊ��ֹ
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
                    xFuncHandlers[i].ucFunctionCode = ucFunctionCode; /*���ݱ����еĹ����룬������*/
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
                if( xFuncHandlers[i].ucFunctionCode == ucFunctionCode ) /*���ݱ����еĹ����룬������*/
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
         * ����Э��ջ.
         */
        pvMBFrameStartCur( pStu );   /*pvMBFrameStartCur = eMBRTUStart;����eMBRTUStart����*/
        pStu->eMBState = STATE_ENABLED;
    }
    else
    {
        eStatus = MB_EILLSTATE;   //Э��ջ���ڷǷ�״̬.
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
/*��������:
*1:���Э��ջ״̬�Ƿ�ʹ�ܣ�eMBState��ֵΪSTATE_NOT_INITIALIZED����eMBInit()�����б���ֵΪSTATE_DISABLED,��eMBEnable�����б���ֵΪSTATE_ENABLE;
*2:��ѯEV_FRAME_RECEIVED�¼���������EV_FRAME_RECEIVED�¼�����������һ֡�������ݣ��ϱ�EV_EXECUTE�¼�������һ֡���ģ���Ӧ(����)һ֡���ݸ�����;
*/
eMBErrorCode
eMBPoll( struct ModbusSlaveStru *pStu )
{
	UCHAR   	   *ucMBFrame;   //���պͷ��ͱ������ݻ�����
    USHORT   		usLength;    //���ĳ���
    UCHAR    		ucFunctionCode;//������
    eEventstate     eEvent;      //�¼���־ö��
    eMBException 	eException;  //��������Ӧö��
    eMBErrorCode    eStatus = MB_ENOERR;     //modbusЭ��ջ������
    int i;

    /* Check if the protocol stack is ready. */
    if( pStu->eMBState != STATE_ENABLED )  //���Э��ջ�Ƿ�ʹ��
    {
        return MB_EILLSTATE;      //Э��ջδʹ�ܣ�����Э��ջ��Ч������
    }
	if(pStu->pReg->SCIRXST.all & 0x00bc){   //������ϸ�λ
		pStu->pReg->SCICTL1.bit.SWRESET = 0;
		asm(" RPT	 #10 || NOP ");
		pStu->pReg->SCICTL1.bit.SWRESET = 1;
	}
    /* Check if there is a event available. If not return control to caller.
     * Otherwise we will handle the event.
     * ����Ƿ��п��õ��¼������û�У��򽫿���Ȩ���ظ������ߡ��������ǽ������¼���*/
    if( xMBPortEventGet(pStu, &eEvent ) == TRUE )   //��ѯ�ĸ��¼�����
    {
        switch ( eEvent )
        {
        case EV_READY:
            break;

        case EV_FRAME_RECEIVED:  /*���յ�һ֡���ݣ����¼�����*/
        	eStatus = peMBFrameReceiveCur(pStu, &(pStu->ucRcvAddress), &ucMBFrame, &usLength );
            if( eStatus == MB_ENOERR )   //���ĳ��Ⱥ�CRCУ����ȷ
            {
                /* Check if the frame is for us. If not ignore the frame.
                 * �жϽ��յ��ı����Ƿ����, �����,����������*/
                if( ( pStu->ucRcvAddress == pStu->ucMBAddress ) || ( pStu->ucRcvAddress == MB_ADDRESS_BROADCAST ) )
                {
                    ( void )xMBPortEventPost( pStu,EV_EXECUTE );  //�޸��¼���־ΪEV_EXECUTEִ���¼�
                    pStu->ComuState = 1;
                }
            }
            break;

        case EV_EXECUTE:                                        //�Խ��յ��ı��Ľ��д����¼�
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

