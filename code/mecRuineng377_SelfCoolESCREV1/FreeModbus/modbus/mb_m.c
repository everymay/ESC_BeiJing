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
 * һ��Modbus��������������Modbus����������ʵ�ֺ�����������.
 */
static xMBMasterFunctionHandler xMasterFuncHandlers[MB_FUNC_HANDLERS_MAX] = {
#if MB_FUNC_OTHER_REP_SLAVEID_ENABLED > 0
	//TODO Add Master function define
    {MB_FUNC_OTHER_REPORT_SLAVEID, eMBFuncReportSlaveID},
#endif
#if MB_FUNC_READ_INPUT_ENABLED > 0
    {MB_FUNC_READ_INPUT_REGISTER, eMBMasterFuncReadInputRegister}, //������04,��ȡ����Ĵ���
#endif
#if MB_FUNC_READ_HOLDING_ENABLED > 0
    {MB_FUNC_READ_HOLDING_REGISTER, eMBMasterFuncReadHoldingRegister},//������03,��ȡ���ּĴ���
#endif
#if MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0
    {MB_FUNC_WRITE_MULTIPLE_REGISTERS, eMBMasterFuncWriteMultipleHoldingRegister},//������16,д������ּĴ���
#endif
#if MB_FUNC_WRITE_HOLDING_ENABLED > 0
    {MB_FUNC_WRITE_REGISTER, eMBMasterFuncWriteHoldingRegister},//������06,д�������ּĴ���
#endif
#if MB_FUNC_READWRITE_HOLDING_ENABLED > 0
    {MB_FUNC_READWRITE_MULTIPLE_REGISTERS, eMBMasterFuncReadWriteMultipleHoldingRegister},//������23,��д�������
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
/*�������ܣ�
*1:ʵ��RTUģʽ��ASCALLģʽ��Э��ջ��ʼ��;
*2:���Э��ջ���ĺ���ָ��ĸ�ֵ������ModbusЭ��ջ��ʹ�ܺͽ�ֹ�����ĵĽ��պ���Ӧ��3.5T��ʱ���жϻص����������ڷ��ͺͽ����жϻص�����;
*3:eMBRTUInit���RTUģʽ�´��ں�3.5T��ʱ���ĳ�ʼ�������û��Լ���ֲ;
*4:����ModbusЭ��ջ��ģʽeMBCurrentModeΪMB_RTU������ModbusЭ��ջ״̬eMBStateΪSTATE_DISABLED;
*/
eMBErrorCode
eMBMasterInit(struct ModbusSlaveStru *pStu, eMBMode eMode, UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity )
{
    eMBErrorCode    eStatus = MB_ENOERR; //����״̬��ʼֵ

	switch (eMode)
	{
#if MB_MASTER_RTU_ENABLED > 0
	case MB_RTU:
		peMBMasterFrameSendCur = eMBMasterRTUSend;  //modbus������Ӧ����
		peMBMasterFrameReceiveCur = eMBMasterRTUReceive;  //modbus���Ľ��պ���
		pxMBMasterFrameCBByteReceived = xMBMasterRTUReceiveFSM;  //���ڽ����ж����յ��ô˺�����������(����״̬��)  //5
		pxMBMasterFrameCBTransmitterEmpty = xMBMasterRTUTransmitFSM;  //���ڷ����ж����յ��ô˺�����������(����״̬��)
		pxMBMasterPortCBTimerExpired = xMBMasterRTUTimerExpired;  //��ʱ���жϺ������յ��ôκ�����ɶ�ʱ���ж�(���ĵ��������)   //6
		pvMBMasterFrameStartCur = eMBMasterRTUStart;  //ʹ������modbusЭ��ջ  //3
//		pvMBMasterFrameStopCur = eMBMasterRTUStop;    //����modbusЭ��ջ
//		pvMBMasterFrameCloseCur = MB_PORT_HAS_CLOSE ? vMBMasterPortClose : NULL;  //����״̬��
		eStatus = eMBMasterRTUInit(pStu, ulBaudRate, eParity);  //��ʼ��RTU 1
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
		eStatus = MB_EINVAL;    //�Ƿ�����
		break;
	}

	if (eStatus == MB_ENOERR)
	{
		if (!xMBMasterPortEventInit(pStu))
		{
			/* port dependent event module initalization failed.
			 * �˿�����¼�ģ���ʼ��ʧ��
			 */
			eStatus = MB_EPORTERR;  //��ֲ�����
		}
		else
		{   //�趨��ǰ״̬
			pStu->eMBState = STATE_DISABLED;  //modbusЭ��ջ��ʼ��״̬,�ڴ˳�ʼ��Ϊ��ֹ
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
/*��������
*1:����ModbusЭ��ջ����״̬eMBStateΪSTATE_ENABLED;
*2:����pvMBFrameStartCur()��������Э��ջ
*/
eMBErrorCode
eMBMasterEnable( struct ModbusSlaveStru *pStu )     //2
{
    eMBErrorCode    eStatus = MB_ENOERR;

    if( pStu->eMBState == STATE_DISABLED )
    {
        /* Activate the protocol stack.
         * ����Э��ջ
         */
        pvMBMasterFrameStartCur(pStu);  //����eMBMasterRTUStart����
        pStu->eMBState = STATE_ENABLED;  //ʹ��
    }
    else
    {
        eStatus = MB_EILLSTATE;//Э��ջ���ڷǷ�״̬
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
/*��������:
*1:���Э��ջ״̬�Ƿ�ʹ�ܣ�eMBState��ֵΪSTATE_NOT_INITIALIZED����eMBInit()�����б���ֵΪSTATE_DISABLED,��eMBEnable�����б���ֵΪSTATE_ENABLE;
*2:��ѯEV_FRAME_RECEIVED�¼���������EV_FRAME_RECEIVED�¼�����������һ֡�������ݣ��ϱ�EV_EXECUTE�¼�������һ֡���ģ���Ӧ(����)һ֡���ݸ��ӻ�;
*/
eMBErrorCode
eMBMasterPoll( struct ModbusSlaveStru *pStu )    // 4
{
	UCHAR   *ucMBFrame;           //���պͷ��ͱ������ݻ�����
	UCHAR   *ucSndFrame;          //����֡
	UCHAR 	ucFunctionCode;       //������
    USHORT   usLength;            //���ĳ���
    UCHAR	i,j;
    eMBException 	eException;   //��������Ӧö��
    eMBErrorCode    eStatus = MB_ENOERR;  //modbusЭ��ջ������
    eEventstate    eEvent;        //�¼���־ö��
//    eMBMasterErrorEventType errorType;

    /* Check if the protocol stack is ready.
     * ���Э��ջ�Ƿ�׼������
     */
    if( pStu->eMBState != STATE_ENABLED )      //���Э��ջ�Ƿ�ʹ��
    {
        return MB_EILLSTATE;            //Э��ջδʹ��,����Э��ջ��Ч������
    }
	if(pStu->pReg->SCIRXST.all & 0x00bc){   //������ϸ�λ
		pStu->pReg->SCICTL1.bit.SWRESET = 0;
		asm(" RPT	 #10 || NOP ");
		pStu->pReg->SCICTL1.bit.SWRESET = 1;
	}
    /* Check if there is a event available. If not return control to caller.
     * Otherwise we will handle the event.
     * ����Ƿ��п��õ��¼�.���û�У��򽫿���Ȩ���ظ������ߡ��������ǽ������¼���
     */
    if( xMBMasterPortEventGet(pStu, &eEvent ) == TRUE )  //��ѯ�ĸ��¼�����
    {
        switch ( eEvent )
        {
        case EV_MASTER_READY:
            break;

        case EV_MASTER_FRAME_RECEIVED:       //���յ�һ֡����,���¼�����
			eStatus = peMBMasterFrameReceiveCur(pStu, &(pStu->ucRcvAddress), &ucMBFrame, &usLength );
			/* Check if the frame is for us. If not ,send an error process event.
			 * �жϽ��յ��ı��������Ƿ�ɽ��ܣ�����ǣ�����������;�������,����һ���������¼�
			 */
			if ( ( eStatus == MB_ENOERR ) && ( pStu->ucRcvAddress == ucMBMasterGetDestAddress(pStu) ) )  //���ĳ�����ȷ��CRCУ����ȷ,���Ҵӻ���ַ��ȷ
			{
				( void ) xMBMasterPortEventPost( pStu,EV_MASTER_EXECUTE );  //�޸��¼���־ΪEV_MASTER_EXECUTEִ���¼�
			}
			break;

        case EV_MASTER_EXECUTE:                                    //�Խ��յ��ı��Ľ����¼�����
        	usLength = pStu->usRcvBufferPos-3;                     //��ǰ���ձ��ĳ��ȼ�ȥ��������(�����������CRCУ��,ʣ��������Ϊ��Ԫ��ַλ�����ݸ���λ)
        	ucMBFrame = (Uint16 *)(pStu->ModbusRcvSedBuff + 1);    //+1��Ϊ�˴����һλ��ַλ.
            ucFunctionCode = ucMBFrame[MB_PDU_FUNC_OFF];           //��ȡPDU�е�һ���ֽ�,Ϊ������
             eException = MB_EX_ILLEGAL_FUNCTION;                  //���������ֵΪ��Ч�Ĺ�����
             /* If receive frame has exception .The receive function code highest bit is 1.
              * �������֡���쳣�����չ��������λΪ1.
              */
             if(ucFunctionCode >> 7) {
             	eException = (eMBException)ucMBFrame[MB_PDU_DATA_OFF];//��Ӧ������PDU�е�ƫ����
             }
 			else
 			{
 				for (i = 0; i < MB_FUNC_HANDLERS_MAX; i++)
 				{
 					/* No more function handlers registered. Abort.
 					 * ����ע�ắ���������.��ֹ.
 					 */
 					if (xMasterFuncHandlers[i].ucFunctionCode == 0){
 						break;
 					}
 					else if (xMasterFuncHandlers[i].ucFunctionCode == ucFunctionCode) {   /*���ݱ����еĹ����룬������*/
 						vMBMasterSetCBRunInMasterMode(TRUE);
 						/* If master request is broadcast,
 						 * the master need execute function for all slave.
 						 * ��������󱻹㲥������Ҫִ�����дӵĹ���.
 						 */
 						if ( xMBMasterRequestIsBroadcast() ) {
 							usLength = usMBMasterGetPDUSndLength(pStu);  //��ȡModbus Master����PDU�Ļ���������
 							for(j = 1; j <= MB_MASTER_TOTAL_SLAVE_NUM; j++){
 								vMBMasterSetDestAddress(pStu ,j);  //����Modbus Master����Ŀ�ĵ�ַ
 								eException = xMasterFuncHandlers[i].pxHandler(pStu,ucMBFrame, &usLength); /*�Խ��յ��ı��Ľ��н���*/
 							}
 						}
 						else {
 							eException = xMasterFuncHandlers[i].pxHandler(pStu,ucMBFrame, &usLength); /*�Խ��յ��ı��Ľ��н���*/
 						}
 						vMBMasterSetCBRunInMasterMode(FALSE);  //����Modbus Master�Ƿ���Masterģʽ����
 						break;
 					}
 				}
 			}
             /* If master has exception ,Master will send error process.Otherwise the Master is idle.
              * ����������쳣�����������ʹ�����̡������������ڿ���״̬.
              */
             if (eException != MB_EX_NONE) {         /*���յ��ı����д���*/
             	vMBMasterSetErrorType(pStu ,EV_ERROR_EXECUTE_FUNCTION);  //����Modbus Master��ǰ�����¼�����.
             	( void ) xMBMasterPortEventPost(pStu, EV_MASTER_ERROR_PROCESS );   //֡�������.
             }
      //       else {
  //           	vMBMasterCBRequestScuuess( );
 //            	vMBMasterRunResRelease( );
    //         }
            break;

        case EV_MASTER_FRAME_SENT:
        	/* Master is busy now. */
        	vMBMasterGetPDUSndBuf(pStu, &ucMBFrame ,&ucSndFrame);   //��ȡModbus Master����PDU�Ļ�������ַָ��
			eStatus = peMBMasterFrameSendCur( pStu,ucMBMasterGetDestAddress(pStu), ucMBFrame, usMBMasterGetPDUSndLength(pStu) );  /*modbus������Ӧ����,������Ӧ���ӻ�*/
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

