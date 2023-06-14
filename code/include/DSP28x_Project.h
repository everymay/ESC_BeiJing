
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

// �����ֹ���������
typedef union
{
    Uint16 all;

    struct
    {
    	Uint16 ESCFastHardwareOverCurFlagA   : 1;   // 0λ��ESC����Ӳ����������A
    	Uint16 ESCFastHardwareOverCurFlagB   : 1;   // 1λ��ESC����Ӳ����������B
    	Uint16 ESCFastHardwareOverCurFlagC   : 1;   // 2λ��ESC����Ӳ����������C
    	Uint16 ESCInsOverCurFlagA            : 1;   // 3λ��ESC˲ʱֵ��������A
    	Uint16 ESCInsOverCurFlagB            : 1;   // 4λ��ESC˲ʱֵ��������B
    	Uint16 ESCInsOverCurFlagC            : 1;   // 5λ��ESC˲ʱֵ��������C
    	Uint16 ESCDcCapVoltOverVoltFlagA     : 1;   // 6λ, ESCֱ�����ݵ�ѹ��ѹ����A
    	Uint16 ESCDcCapVoltOverVoltFlagB     : 1;   // 7λ, ESCֱ�����ݵ�ѹ��ѹ����B
    	Uint16 ESCDcCapVoltOverVoltFlagC     : 1;   // 8λ, ESCֱ�����ݵ�ѹ��ѹ����C
    	Uint16 ESCGridRMSOverVoltFlagA       : 1;   // 9λ��ESC������ѹ��Чֵ��ѹ����A
    	Uint16 ESCGridRMSOverVoltFlagB       : 1;   //10λ��ESC������ѹ��Чֵ��ѹ����B
    	Uint16 ESCGridRMSOverVoltFlagC       : 1;   //11λ��ESC������ѹ��Чֵ��ѹ����C
    	Uint16 ESCPowerFailDetectFlag        : 1;   //12λ��ESC���������
    	Uint16 ESCGridRMSUnderVoltFlagA      : 1;   //13λ��ESC������ѹ��ЧֵǷѹ����A
    	Uint16 ESCGridRMSUnderVoltFlagB      : 1;   //14λ��ESC������ѹ��ЧֵǷѹ����B
        Uint16 ESCGridRMSUnderVoltFlagC      : 1;   //15λ��ESC������ѹ��ЧֵǷѹ����C
    }B;
}SOFT_FAULT_BIT1;

typedef union
{
	Uint16 all;
	struct
	{
		Uint16 ESCOverLoadFlagA :1;// 0λ��ESC������ع���A
		Uint16 ESCOverLoadFlagB :1;// 1λ��ESC������ع���B
		Uint16 ESCOverLoadFlagC :1;// 2λ��ESC������ع���C
		Uint16 ESCRmsOverCurrentFlagA :1;// 3λ��ESC��Чֵ�����������A
		Uint16 ESCRmsOverCurrentFlagB :1;// 4λ��ESC��Чֵ�����������B
		Uint16 ESCRmsOverCurrentFlagC :1;// 5λ��ESC��Чֵ�����������C
		Uint16 ESCOverTemperatureFlagA :1;// 6λ��ESCɢ��Ƭ�¶ȹ��¹���A
		Uint16 ESCOverTemperatureFlagB :1;// 7λ��ESCɢ��Ƭ�¶ȹ��¹���B
		Uint16 ESCOverTemperatureFlagC :1;// 8λ��ESCɢ��Ƭ�¶ȹ��¹���C
		Uint16 ESCFangLeiFaultFlag :1;// 9λ��ESC����������
		Uint16 ESCBypassContNoCloseFlagA :1; //A����·�ű��̵ּ����պϹ��ϱ�־λ��0��������1�����ϡ�WY
		Uint16 ESCBypassContNoCloseFlagB :1; //B����·�ű��̵ּ����պϹ��ϱ�־λ��0��������1�����ϡ�WY
		Uint16 ESCBypassContNoCloseFlagC :1; //C����·�ű��̵ּ����պϹ��ϱ�־λ��0��������1�����ϡ�WY
		Uint16 ESCChassisOverTempFlag :1;//13λ��ESC��Ԫ����(����)
		Uint16 rsvd1 :1;//14λ������
		Uint16 rsvd2 :1;//15λ������
	} B;
} SOFT_FAULT_BIT2;

//---BGMODLE_3P_CURR_SOURCEģʽ��,װ������״̬{1.	��������ʾ���ݶ�(0x30,01)->��1-5 ������Դ(����6)->������3}
typedef union
{
    Uint16                       all;
    struct
    {
    	Uint16 ESCCalibrarionFailureFlag   		: 1;   	// 0λ��ESC��ƫУ׼�������
    	Uint16 ESCResonanceFlautFlag         	: 1;   	// 1λ��ESCг�����
    	Uint16 ESCGridOverFreqenceFlag	        : 1;   	// 2λ��ESC����Ƶ�ʹ�Ƶ����
    	Uint16 ESCGridUnderFreqenceFlag         : 1;	// 3λ��ESC����Ƶ��ǷƵ����
    	Uint16 ESCTieDianReadErrFlag	        : 1;   	// 4λ��ESC�������
    	Uint16 ESCAutoStartMuchFaultFlag 	    : 1;   	// 5λ��ESC������Ƶ������
    	Uint16 ESCSicFaultFlagA 	            : 1;   	// 6λ��ESC̼���������A
    	Uint16 ESCSicFaultFlagB     	       	: 1;  	// 7λ, ESC̼���������B
    	Uint16 ESCSicFaultFlagC 		        : 1;  	// 8λ��ESC̼���������C
    	Uint16 ESCHighLowRelayA   		        : 1;	// 9λ��ESC�ߵ�ѹ�ű���δ��բA
    	Uint16 ESCHighLowRelayB   		        : 1;	// 10λ��ESC�ߵ�ѹ�ű���δ��բB
    	Uint16 ESCHighLowRelayC   		        : 1;	// 11λ��ESC�ߵ�ѹ�ű���δ��բC
        Uint16 ESCBYRelayFaultA                 : 1;    // 12λ, ESC��·�ű��̵ּ���δ��բA
    	Uint16 ESCBYRelayFaultB     		    : 1;   	// 13λ, ESC��·�ű��̵ּ���δ��բB
    	Uint16 ESCBYRelayFaultC                 : 1;  	// 14λ, ESC��·�ű��̵ּ���δ��բC
    	Uint16 rsvd10       		            : 1;   	// 15λ������
     }B;
}SOFT_FAULT_BIT3;

typedef union
{
    Uint16                       all;
    struct
    {
        Uint16 rsvd             : 16;
//        Uint16 overTemperatureFlag       : 1;   // 0λ��ESC-���� ģ�����
//        Uint16 boardovertemp             : 1;   // 1λ, ESC-���� ��·�����
//        Uint16 apfOverLoadFlag           : 1;   // 2λ��ESC-���� APF�������
//        Uint16 gridRMSUnderVoltageFlag   : 1;   // 3λ��ESC-���� ������ѹ��ЧֵǷѹ����λ
//        Uint16 gridRMSOverVoltageFlag    : 1;   // 4λ��ESC-���� ������ѹ��Чֵ��ѹ����λ
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

