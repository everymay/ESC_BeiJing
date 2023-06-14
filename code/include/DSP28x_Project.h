
// TI File $Revision: /main/1 $
// Checkin $Date: April 22, 2008   14:35:56 $
//###########################################################################
//
// FILE:   DSP28x_Project.h
//
// TITLE:  DSP28x Project Headerfile and Examples Include File
//
//###########################################################################
// $TI Release: DSP2833x/DSP2823x C/C++ Header Files V1.31 $
// $Release Date: August 4, 2009 $
//###########################################################################

#ifndef DSP28x_PROJECT_H
#define DSP28x_PROJECT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "F2837xS_device.h"     // Headerfile Include File
#include "F2837xS_Examples.h"   // Examples Include File
#include "VariblesRef.h"
#include "MarcoDefine.h"
#include "../FreeModbus/AuthorFiles/main.h"
#include "Spi_Frame.h"

// 故障字管理，共用体
typedef union
{
    Uint16 all;

    struct
    {
    	Uint16 ESCFastHardwareOverCurFlagA   : 1;   // 0位，ESC快速硬件过流故障A
    	Uint16 ESCFastHardwareOverCurFlagB   : 1;   // 1位，ESC快速硬件过流故障B
    	Uint16 ESCFastHardwareOverCurFlagC   : 1;   // 2位，ESC快速硬件过流故障C
    	Uint16 ESCInsOverCurFlagA            : 1;   // 3位，ESC瞬时值过流故障A
    	Uint16 ESCInsOverCurFlagB            : 1;   // 4位，ESC瞬时值过流故障B
    	Uint16 ESCInsOverCurFlagC            : 1;   // 5位，ESC瞬时值过流故障C
    	Uint16 ESCDcCapVoltOverVoltFlagA     : 1;   // 6位, ESC直流电容电压过压故障A
    	Uint16 ESCDcCapVoltOverVoltFlagB     : 1;   // 7位, ESC直流电容电压过压故障B
    	Uint16 ESCDcCapVoltOverVoltFlagC     : 1;   // 8位, ESC直流电容电压过压故障C
    	Uint16 ESCGridRMSOverVoltFlagA       : 1;   // 9位，ESC电网电压有效值过压故障A
    	Uint16 ESCGridRMSOverVoltFlagB       : 1;   //10位，ESC电网电压有效值过压故障B
    	Uint16 ESCGridRMSOverVoltFlagC       : 1;   //11位，ESC电网电压有效值过压故障C
    	Uint16 ESCPowerFailDetectFlag        : 1;   //12位，ESC掉电检测故障
    	Uint16 ESCGridRMSUnderVoltFlagA      : 1;   //13位，ESC电网电压有效值欠压故障A
    	Uint16 ESCGridRMSUnderVoltFlagB      : 1;   //14位，ESC电网电压有效值欠压故障B
        Uint16 ESCGridRMSUnderVoltFlagC      : 1;   //15位，ESC电网电压有效值欠压故障C
    }B;
}SOFT_FAULT_BIT1;

typedef union
{
	Uint16 all;
	struct
	{
		Uint16 ESCOverLoadFlagA :1;// 0位，ESC输出过载故障A
		Uint16 ESCOverLoadFlagB :1;// 1位，ESC输出过载故障B
		Uint16 ESCOverLoadFlagC :1;// 2位，ESC输出过载故障C
		Uint16 ESCRmsOverCurrentFlagA :1;// 3位，ESC有效值输出过流故障A
		Uint16 ESCRmsOverCurrentFlagB :1;// 4位，ESC有效值输出过流故障B
		Uint16 ESCRmsOverCurrentFlagC :1;// 5位，ESC有效值输出过流故障C
		Uint16 ESCOverTemperatureFlagA :1;// 6位，ESC散热片温度过温故障A
		Uint16 ESCOverTemperatureFlagB :1;// 7位，ESC散热片温度过温故障B
		Uint16 ESCOverTemperatureFlagC :1;// 8位，ESC散热片温度过温故障C
		Uint16 ESCFangLeiFaultFlag :1;// 9位，ESC防雷器故障
		Uint16 ESCBypassContNoCloseFlagA :1; //A相旁路磁保持继电器闭合故障标志位。0，正常；1，故障。WY
		Uint16 ESCBypassContNoCloseFlagB :1; //B相旁路磁保持继电器闭合故障标志位。0，正常；1，故障。WY
		Uint16 ESCBypassContNoCloseFlagC :1; //C相旁路磁保持继电器闭合故障标志位。0，正常；1，故障。WY
		Uint16 ESCChassisOverTempFlag :1;//13位，ESC单元过温(机壳)
		Uint16 rsvd1 :1;//14位，保留
		Uint16 rsvd2 :1;//15位，保留
	} B;
} SOFT_FAULT_BIT2;

//---BGMODLE_3P_CURR_SOURCE模式下,装置现在状态{1.	主界面显示数据读(0x30,01)->表1-5 逆变电流源(场景6)->故障字3}
typedef union
{
    Uint16                       all;
    struct
    {
    	Uint16 ESCCalibrarionFailureFlag   		: 1;   	// 0位，ESC零偏校准过大故障
    	Uint16 ESCResonanceFlautFlag         	: 1;   	// 1位，ESC谐振故障
    	Uint16 ESCGridOverFreqenceFlag	        : 1;   	// 2位，ESC电网频率过频故障
    	Uint16 ESCGridUnderFreqenceFlag         : 1;	// 3位，ESC电网频率欠频故障
    	Uint16 ESCTieDianReadErrFlag	        : 1;   	// 4位，ESC铁电故障
    	Uint16 ESCAutoStartMuchFaultFlag 	    : 1;   	// 5位，ESC自启动频繁故障
    	Uint16 ESCSicFaultFlagA 	            : 1;   	// 6位，ESC碳化硅管子损坏A
    	Uint16 ESCSicFaultFlagB     	       	: 1;  	// 7位, ESC碳化硅管子损坏B
    	Uint16 ESCSicFaultFlagC 		        : 1;  	// 8位，ESC碳化硅管子损坏C
    	Uint16 ESCHighLowRelayA   		        : 1;	// 9位，ESC高低压磁保持未合闸A
    	Uint16 ESCHighLowRelayB   		        : 1;	// 10位，ESC高低压磁保持未合闸B
    	Uint16 ESCHighLowRelayC   		        : 1;	// 11位，ESC高低压磁保持未合闸C
        Uint16 ESCBYRelayFaultA                 : 1;    // 12位, ESC旁路磁保持继电器未合闸A
    	Uint16 ESCBYRelayFaultB     		    : 1;   	// 13位, ESC旁路磁保持继电器未合闸B
    	Uint16 ESCBYRelayFaultC                 : 1;  	// 14位, ESC旁路磁保持继电器未合闸C
    	Uint16 rsvd10       		            : 1;   	// 15位，保留
     }B;
}SOFT_FAULT_BIT3;

typedef union
{
    Uint16                       all;
    struct
    {
        Uint16 rsvd             : 16;
//        Uint16 overTemperatureFlag       : 1;   // 0位，ESC-警告 模块过温
//        Uint16 boardovertemp             : 1;   // 1位, ESC-警告 电路板过温
//        Uint16 apfOverLoadFlag           : 1;   // 2位，ESC-警告 APF输出过载
//        Uint16 gridRMSUnderVoltageFlag   : 1;   // 3位，ESC-警告 电网电压有效值欠压故障位
//        Uint16 gridRMSOverVoltageFlag    : 1;   // 4位，ESC-警告 电网电压有效值过压故障位
     }B;
}SOFT_FAULT_BIT4;

typedef union
{
    Uint16                       all;
    struct
    {
    	Uint16 rsvd   			: 16;
     }B;
}SOFT_FAULT_BIT5;

extern SOFT_FAULT_BIT1 softwareFaultWord1;
extern SOFT_FAULT_BIT2 softwareFaultWord2;
extern SOFT_FAULT_BIT3 softwareFaultWord3;
extern SOFT_FAULT_BIT4 softwareFaultWord4;
extern SOFT_FAULT_BIT5 softwareFaultWord5;
#ifdef __cplusplus
}
#endif /* extern "C" */

#endif  // end of DSP28x_PROJECT_H definition

//===========================================================================
// End of file.
//===========================================================================

