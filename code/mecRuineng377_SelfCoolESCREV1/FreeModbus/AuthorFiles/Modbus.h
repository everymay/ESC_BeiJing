/*
 * Modbus.h
 *
 *  Created on: 2018年1月29日
 *      Author: Xu-yulin
 */
#ifndef MODBUS_H_
#define MODBUS_H_
#include "F2837xS_device.h"
#include "port.h"
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/BIOS.h>
#include <xdc/std.h>
#include <xdc/runtime/Log.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Mailbox.h>

typedef enum
{
    STATE_RX_INIT,              /*!< Receiver is in initial state. */
    STATE_RX_IDLE,              /*!< Receiver is in idle state. */
    STATE_RX_RCV,               /*!< Frame is beeing received. */
    STATE_RX_ERROR,              /*!< If the frame is invalid. */
    STATE_M_RX_INIT,              /*!< Receiver is in initial state. */
    STATE_M_RX_IDLE,              /*!< Receiver is in idle state. */
    STATE_M_RX_RCV,               /*!< Frame is beeing received. */
    STATE_M_RX_ERROR,              /*!< If the frame is invalid. */
} mdslavercvstate;
typedef enum
{
    STATE_TX_IDLE,              /*!< Transmitter is in idle state. */
    STATE_TX_XMIT,               /*!< Transmitter is in transfer state. */
    STATE_M_TX_IDLE,              /*!< Transmitter is in idle state. */
    STATE_M_TX_XMIT,              /*!< Transmitter is in transfer state. */
    STATE_M_TX_XFWR,              /*!< Transmitter is in transfer finish and wait receive state. */
} mdslavesedstate;
typedef enum
{
    EV_READY            			   ,         /*!< Startup finished. */
    EV_FRAME_RECEIVED   			   ,         /*!< Frame received. */
    EV_EXECUTE         				   ,         /*!< Execute function. */
    EV_FRAME_SENT       			   ,          /*!< Frame sent. */
    EV_MASTER_READY                    ,  /*!< Startup finished. */
    EV_MASTER_FRAME_RECEIVED           ,  /*!< Frame received. */
    EV_MASTER_EXECUTE                  ,  /*!< Execute function. */
    EV_MASTER_FRAME_SENT               ,  /*!< Frame sent. */
    EV_MASTER_ERROR_PROCESS            ,  /*!< Frame error process. */
    EV_MASTER_PROCESS_SUCESS           ,  /*!< Request process success. */
    EV_MASTER_ERROR_RESPOND_TIMEOUT    ,  /*!< Request respond timeout. */
    EV_MASTER_ERROR_RECEIVE_DATA       ,  /*!< Request receive data error. */
    EV_MASTER_ERROR_EXECUTE_FUNCTION   ,  /*!< Request execute function error. */
    EV_FRAME_SENT0                      ,          /*!< Frame sent. */
    EV_FRAME_SENT1                      ,          /*!< Frame sent. */
    EV_FRAME_SENT2                      ,          /*!< Frame sent. */
    EV_FRAME_SENT3                      ,          /*!< Frame sent. */
} eEventstate;
typedef enum
{
   STATE_ENABLED,
   STATE_DISABLED,
   STATE_NOT_INITIALIZED
} eMBSlaveState;
struct ModbusSlaveStru{
	volatile struct SCI_REGS *pReg;
	Uint16 *ModbusRcvSedBuff;
	Uint16 *ModbusMasterBuff;
	Uint16 ModBusMSTimFlag;
	Uint16 ModBusMSTimCount;
	Uint16 ModBusMSTimOut;
	Uint16 usSndBufferCount;
	Uint16 usRcvBufferPos;
	Uint16 ucMBAddress;
	Uint16 xEventInQueue;
	Uint16 portaddr;
	Uint16 ComuState;
	Uint16 ucRcvAddress;
	Uint16 usMasterSendPDULength;
	eMBSlaveState   eMBState;
	eEventstate     eMBEventType;
	eEventstate     eQueuedEvent;
	mdslavercvstate eMBRcvState;
	mdslavesedstate eMBSndState;
	Uint16 			SendTrsformDelayTim;
	Uint16 			SendTrsformDelayTimCont;//发送转换延迟定时器
	volatile UCHAR 	*pucSndBufferCur;
};
extern struct ModbusSlaveStru SCIC,SCIB,SCID;
extern  Uint16 Modbuff[3][256];
extern  Uint16 SndbuffM[3][8];
#define RetHoldStart  0x300
#define CapyRadio 0.01//电容器组容量变比
#define CaShRadio 0.01//投切门限变比
#define TmOnRadio 0.01//投延时变比
#define TmOfRadio 0.01//切延时变比
#define CapGroup				(24)
#define CapGroupNes				(CapGroup*2)
#define CapSwitchVarUp			(1.2*CaShRadio)
#define CapSwitchVarDown		(0.8)

extern UCHAR   WrongData;
extern UCHAR   RecdNone;
extern UCHAR   CapCommErrCount;
extern UCHAR   CapDataReceived;
/******************蓝牙**************************************************************************/
#define SCIC_DEFAULTS   {&ScicRegs,Modbuff[0],SndbuffM[0],0,0,3,0,0,0,0,0,0,0,0,STATE_NOT_INITIALIZED,EV_READY,EV_READY,STATE_RX_INIT,STATE_TX_IDLE,0,0}
#define SCIC_M_DEFAULTS {&ScicRegs,Modbuff[0],SndbuffM[0],0,0,3,0,0,0,0,0,0,0,0,STATE_NOT_INITIALIZED,EV_MASTER_READY,EV_MASTER_READY,STATE_M_RX_INIT,STATE_M_TX_IDLE,0,0}
/******************液晶屏**************************************************************************/
#define SCIB_DEFAULTS 	{&ScibRegs,Modbuff[1],SndbuffM[1],0,0,3,0,0,0,0,1,0,0,0,STATE_NOT_INITIALIZED,EV_READY,EV_READY,STATE_RX_INIT,STATE_TX_IDLE,0,0}
#define SCIB_M_DEFAULTS {&ScibRegs,Modbuff[1],SndbuffM[1],0,0,3,0,0,0,0,1,0,0,0,STATE_NOT_INITIALIZED,EV_MASTER_READY,EV_MASTER_READY,STATE_M_RX_INIT,STATE_M_TX_IDLE,0,0}
/******************后台**************************************************************************/
#define SCID_DEFAULTS 	{&ScidRegs,Modbuff[2],SndbuffM[2],0,0,3,0,0,0,0,2,0,0,0,STATE_NOT_INITIALIZED,EV_READY,EV_READY,STATE_RX_INIT,STATE_TX_IDLE,0,0}
#define SCID_M_DEFAULTS {&ScidRegs,Modbuff[2],SndbuffM[2],0,0,3,0,0,0,0,2,0,0,0,STATE_NOT_INITIALIZED,EV_MASTER_READY,EV_MASTER_READY,STATE_M_RX_INIT,STATE_M_TX_IDLE,0,0}
/********************************************************************************************/
extern const struct ModbusSlaveStru ModbusSciDefaults[3];
extern const struct ModbusSlaveStru ModbusSciDefaultsM[3];
void CapSwitchStatePoll(void);
extern Event_Handle Event_SCI_Reveived;

#endif /* MODBUS_H_ */
