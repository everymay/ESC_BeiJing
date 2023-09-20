#include "DSP28x_Project.h"
#include "math.h"
#include "C28x_FPU_FastRTS.h"
#include "VariblesRef.h"
#include "DPLL.h"
#include "MarcoDefine.h"
#pragma	CODE_SECTION(FaultDetect ,"ram2func")
#pragma	CODE_SECTION(SetFaultDelayCounter ,"ram2func")
#pragma	CODE_SECTION(FaultDetectInInt ,"ram2func")
#pragma	CODE_SECTION(FaultFastDetectInInt ,"ram2func")
#pragma	CODE_SECTION(FirstFaultStop ,"ram2func")
#pragma CODE_SECTION(EcapINT1 ,"ram2func")
#pragma CODE_SECTION(EcapINT2 ,"ram2func")
#pragma CODE_SECTION(EcapINT3 ,"ram2func")
#pragma CODE_SECTION(StateFeedBackJudge ,"ram2func")


	/*
	 * ���Գ���
	   ��¼������ѹ��ЧֵVoltInA_rms�͸��ص�ѹ��ЧֵVoltOutA_rms
	 */
#define LENTH 50 //���鳤��

	unsigned int voltage_grid_A[LENTH] = {0}; //���ԡ�A�������ѹ��Чֵ��WY
	unsigned int voltage_load_A[LENTH] = {0}; //���ԡ�A�ฺ�ص�ѹ��Чֵ��WY

	unsigned int voltage_grid_B[LENTH] = {0}; //���ԡ�WY
	unsigned int voltage_load_B[LENTH] = {0}; //���ԡ�WY

	unsigned int voltage_grid_C[LENTH] = {0}; //���ԡ�WY
	unsigned int voltage_load_C[LENTH] = {0}; //���ԡ�WY

	unsigned int AD_grid_A[LENTH] = {0}; //���ԡ�A����������ֵ��WY
	unsigned int AD_load_A[LENTH] = {0}; //���ԡ�

	unsigned int AD_grid_B[LENTH] = {0}; //���ԡ�
	unsigned int AD_load_B[LENTH] = {0}; //���ԡ�

	unsigned int AD_grid_C[LENTH] = {0}; //���ԡ�
	unsigned int AD_load_C[LENTH] = {0}; //���ԡ�


	unsigned int index_A = 0; //���ԡ�A������������WY
	unsigned int index_B = 0; //���ԡ�����������WY
	unsigned int index_C = 0; //���ԡ�����������WY

/*
 * ���ܣ�����ͣ��Ԥ����WY
 */
void FirstFaultStop(Uint16 ESCPHASE)
{
	/*����A�ࡣWY*/
	if ((ESCFlagA.FAULTCONFIRFlag == 1) //A����ڹ����źš�WY
			&& (ESCPHASE == 1)) //A�ࡣWY
	{
		SET_IGBT_ENA(IGBT_FAULT); //ʧ��A��PWM
		DisablePWMA(); //����A��PWM
		SET_SCRA_ENABLE(ESC_ACTION_ENABLE); //��ͨA����·��բ�ܡ�WY
		SET_MAIN_CONTACT_ACTION_A(ESC_ACTION_DISABLE); //�Ͽ�A����·�ű��̵ּ�����WY
		SET_BYPASS_CONTACT_ACTION_A(ESC_ACTION_ENABLE); //�պ�A����·�ű��̵ּ�����WY
		ESCFlagA.FaultskipFlag = 1;
		ESCFlagA.FAULTCONFIRFlag = 0; //�����ź��ѵõ�����WY
		ESCFlagA.ESCCntMs.StartDelay = 0;
		ESCFlagA.HWPowerFAULTFlag = 1; //A��Ӳ�����ڹ��ϡ�WY
		ESCFlagA.ESCCntSec.HWPowerFaultDelay = 0;
	}

	/*����B�ࡣWY*/
	if ((ESCFlagB.FAULTCONFIRFlag == 1) && (ESCPHASE == 2))
	{
		SET_IGBT_ENB(IGBT_FAULT);
		DisablePWMB();
		SET_SCRB_ENABLE(ESC_ACTION_ENABLE);
		SET_MAIN_CONTACT_ACTION_B(ESC_ACTION_DISABLE);
		SET_BYPASS_CONTACT_ACTION_B(ESC_ACTION_ENABLE);
		ESCFlagB.FaultskipFlag = 1;
		ESCFlagB.FAULTCONFIRFlag = 0;
		ESCFlagB.ESCCntMs.StartDelay = 0;
		ESCFlagB.HWPowerFAULTFlag = 1;
		ESCFlagB.ESCCntSec.HWPowerFaultDelay = 0;
	}

	/*����C�ࡣWY*/
	if ((ESCFlagC.FAULTCONFIRFlag == 1) && (ESCPHASE == 3))
	{
		SET_IGBT_ENC(IGBT_FAULT);
		DisablePWMC();
		SET_SCRC_ENABLE(ESC_ACTION_ENABLE);
		SET_MAIN_CONTACT_ACTION_C(ESC_ACTION_DISABLE);
		SET_BYPASS_CONTACT_ACTION_C(ESC_ACTION_ENABLE);
		ESCFlagC.FaultskipFlag = 1;
		ESCFlagC.FAULTCONFIRFlag = 0;
		ESCFlagC.ESCCntMs.StartDelay = 0;
		ESCFlagC.HWPowerFAULTFlag = 1;
		ESCFlagC.ESCCntSec.HWPowerFaultDelay = 0;
	}
}

int TESTVALAUE = 500;

/*
 * ���ܣ������·����·�ű��̵ּ�����ͨ��״̬��WY
 * ˵�����ú�����ADC-D-1���ж�ISR�б����á�WY
 */
void StateFeedBackJudge(void)
{
	/*����A�ࡣWY*/
	if (GET_BYPASS_CONTACT_ACTION_A == 0) //��⵽A����·�ű��̵ּ����պ��źš�WY
	{
		if (++BypassContactCntA >= TESTVALAUE) //��μ�⡣WY
		{
			BypassContactCntA = TESTVALAUE;
			ESCFlagA.ByPassContactFlag = 1; //A����·�ű��̵ּ����պϡ�WY
		}
	}
	else //��⵽A����·�ű��̵ּ����Ͽ��źš�WY
	{
		if (--BypassContactCntA <= 0) //��μ�⡣WY
		{
			BypassContactCntA = 0;
			ESCFlagA.ByPassContactFlag = 0; //A����·�ű��̵ּ����Ͽ���WY
		}
	}

	/*����B�ࡣWY*/
	if (GET_BYPASS_CONTACT_ACTION_B == 0)
	{
		if (++BypassContactCntB >= TESTVALAUE)
		{
			BypassContactCntB = TESTVALAUE;
			ESCFlagB.ByPassContactFlag = 1;
		}
	}
	else
	{
		if (--BypassContactCntB <= 0)
		{
			BypassContactCntB = 0;
			ESCFlagB.ByPassContactFlag = 0;
		}
	}

	/*����C�ࡣWY*/
	if (GET_BYPASS_CONTACT_ACTION_C == 0)
	{
		if (++BypassContactCntC >= TESTVALAUE)
		{
			BypassContactCntC = TESTVALAUE;
			ESCFlagC.ByPassContactFlag = 1;
		}
	}
	else
	{
		if (--BypassContactCntC <= 0)
		{
			BypassContactCntC = 0;
			ESCFlagC.ByPassContactFlag = 0;
		}
	}

	/*����A��*/
	if (GET_MAIN_CONTACT_ACTION_A == 0) //��⵽A����·�ű��̵ּ����պϵ��źš�WY
	{
		if (++ContactorFeedBackCntA >= TESTVALAUE) //��μ�⡣WY
		{
			ContactorFeedBackCntA = TESTVALAUE;
			ESCFlagA.ContactorFeedBackFlag = 1;//A����·�ű��̵ּ����պϡ�WY
		}
	}
	else //��⵽A����·�ű��̵ּ����Ͽ����źš�WY
	{
		if (--ContactorFeedBackCntA <= 0)
		{
			ContactorFeedBackCntA = 0;
			ESCFlagA.ContactorFeedBackFlag = 0;
		}
	}

	/*����B��*/
	if (GET_MAIN_CONTACT_ACTION_B == 0)
	{
		if (++ContactorFeedBackCntB >= TESTVALAUE)
		{
			ContactorFeedBackCntB = TESTVALAUE;
			ESCFlagB.ContactorFeedBackFlag = 1;
		}
	}
	else
	{
		if (--ContactorFeedBackCntB <= 0)
		{
			ContactorFeedBackCntB = 0;
			ESCFlagB.ContactorFeedBackFlag = 0;
		}
	}

	/*����C��*/
	if (GET_MAIN_CONTACT_ACTION_C == 0)
	{
		if (++ContactorFeedBackCntC >= TESTVALAUE)
		{
			ContactorFeedBackCntC = TESTVALAUE;
			ESCFlagC.ContactorFeedBackFlag = 1;
		}
	}
	else
	{
		if (--ContactorFeedBackCntC <= 0)
		{
			ContactorFeedBackCntC = 0;
			ESCFlagC.ContactorFeedBackFlag = 0;
		}
	}
}

/*
 * ���ܣ������ϡ�WY
 *
 * �������SoeName������������ʾ�Ĺ�������������WY
 * �������counterName�������źż�������������
 * �������FaultDelayTime�������źŴ�������ֵ��
   �����źż����� < �����źŴ�������ֵ�����������ж���
   �����źż����� > �����źŴ�������ֵ��ִ�й����ж���
 * �������ESCPHASE�����ж��������͵��ࡣ
 */
int16 FaultDetect(Uint16 SoeName, Uint16 counterName, Uint16 FaultDelayTime, Uint16 ESCPHASE)
{
	/*
	 * �����źż����� > �����źŴ�������ֵ��ִ�й����ж���WY
	 * Ŀ�ģ���ֹ��⡣
	 */
	if (CntFaultDelay[counterName]++ >= FaultDelayTime)
	{
		DINT;

		if (CTRL24_POWERFlag == 1) //15V��Դ���ϡ�WY
		{
			EPwm1Regs.AQCSFRC.bit.CSFA = EPwmRegsAQCSFRCclear; //PWMǿ�Ƴ�������ߵ�ƽ�����ͣ����WY
			CTRL24_POWERFlag = 0; //����Ӧ��WY
		}

		if ((StateEventFlag_A == STATE_EVENT_WAIT_A) //A�ദ�ڴ���״̬��WY
				&& (StateEventFlag_B == STATE_EVENT_WAIT_B) //B�ദ�ڴ���״̬��WY
				&& (StateEventFlag_C == STATE_EVENT_WAIT_C)) //C�ദ�ڴ���״̬��WY
		{
			SET_POWER_CTRL(1); //15V��Դ�ϵ硣WY
			Delayus(TIME_WRITE_15VOLT_REDAY);
		}

		/*����A��*/
		if ((ESCFlagA.FAULTCONFIRFlag == 1) //A����ڹ����źš�WY
				&& (ESCPHASE == 1) //A�ࡣWY
				&& (ESCBYRelayCNTA != 1)) //A����·�ű��̵ּ���״̬������WY
		{
			StateEventFlag_A = STATE_EVENT_FAULT_A;//��ת״̬������A�����ͣ����WY
			ESCFlagA.faultFlag = 1; //A����ڹ��ϡ�WY
			FirstFaultStop(ESCFlagA.PHASE); //����ͣ��Ԥ����WY
			ESCFlagA.ESCCntMs.CutCurrDelay = 0;
		}

		/*����B��*/
		if ((ESCFlagB.FAULTCONFIRFlag == 1) && (ESCPHASE == 2) && (ESCBYRelayCNTB != 1))
		{
			StateEventFlag_B = STATE_EVENT_FAULT_B;
			ESCFlagB.faultFlag = 1;
			FirstFaultStop(ESCFlagB.PHASE);
			ESCFlagB.ESCCntMs.CutCurrDelay = 0;
		}

		/*����C��*/
		if ((ESCFlagC.FAULTCONFIRFlag == 1) && (ESCPHASE == 3) && (ESCBYRelayCNTC != 1))
		{
			StateEventFlag_C = STATE_EVENT_FAULT_C;
			ESCFlagC.faultFlag = 1;
			FirstFaultStop(ESCFlagC.PHASE);
			ESCFlagC.ESCCntMs.CutCurrDelay = 0;
		}

		EINT;

		SoeRecData(SoeName); //������־��WY
		WriteFlashConfig(SoeName); //��Flashд�����¼����WY
		CntFaultDelay[counterName] = 0; //��������źż�������WY

		return 1;
	}
	else
	{
		ESCFlagA.ESCCntSec.HWPowerFaultDelay = 0;
		ESCFlagA.ESCCntSec.HWPowerStopDelay = 0;
		ESCFlagB.ESCCntSec.HWPowerFaultDelay = 0;
		ESCFlagB.ESCCntSec.HWPowerStopDelay = 0;
		ESCFlagC.ESCCntSec.HWPowerFaultDelay = 0;
		ESCFlagC.ESCCntSec.HWPowerStopDelay = 0;

		return 0;
	}
}

/*
 * ���ܣ����ϼ�⡣WY
 * �������SoeName������������ʾ�Ĺ�������������WY
 * �������counterName�������źż�������������
 * �������FaultDelayTime�������źŴ�������ֵ��
   �����źż����� < �����źŴ�������ֵ�����������ж���
   �����źż����� > �����źŴ�������ֵ��ִ�й����ж���
 */
int16 THREEPHASEFaultDetect(Uint16 SoeName,Uint16 counterName,Uint16 FaultDelayTime)
{
	if (CntFaultDelay[counterName]++ >= FaultDelayTime)
	{
		DINT;

		if (CTRL24_POWERFlag == 1)
		{
			EPwm1Regs.AQCSFRC.bit.CSFA = EPwmRegsAQCSFRCclear;
			CTRL24_POWERFlag = 0;
		}

		if ((StateEventFlag_A == STATE_EVENT_WAIT_A) && (StateEventFlag_B == STATE_EVENT_WAIT_B) && (StateEventFlag_C == STATE_EVENT_WAIT_C))
		{
			SET_POWER_CTRL(1);
			Delayus(TIME_WRITE_15VOLT_REDAY);
		}

		if ((ESCFlagA.FAULTCONFIRFlag == 1) && (ESCBYRelayCNTA != 1))
		{
			StateEventFlag_A = STATE_EVENT_FAULT_A;
			ESCFlagA.faultFlag = 1;
			FirstFaultStop(ESCFlagA.PHASE);
			ESCFlagA.ESCCntMs.CutCurrDelay = 0;
		}

		if ((ESCFlagB.FAULTCONFIRFlag == 1) && (ESCBYRelayCNTB != 1))
		{
			StateEventFlag_B = STATE_EVENT_FAULT_B;
			ESCFlagB.faultFlag = 1;
			FirstFaultStop(ESCFlagB.PHASE);
			ESCFlagB.ESCCntMs.CutCurrDelay = 0;
		}

		if ((ESCFlagC.FAULTCONFIRFlag == 1) && (ESCBYRelayCNTC != 1))
		{
			StateEventFlag_C = STATE_EVENT_FAULT_C;
			ESCFlagC.faultFlag = 1;
			FirstFaultStop(ESCFlagC.PHASE);
			ESCFlagC.ESCCntMs.CutCurrDelay = 0;
		}
		EINT;
		SoeRecData(SoeName);
		WriteFlashConfig(SoeName);
		CntFaultDelay[counterName] = 0;
		return 1;
	}
	else
	{
		ESCFlagA.ESCCntSec.HWPowerFaultDelay = 0;
		ESCFlagA.ESCCntSec.HWPowerStopDelay = 0;
		ESCFlagB.ESCCntSec.HWPowerFaultDelay = 0;
		ESCFlagB.ESCCntSec.HWPowerStopDelay = 0;
		ESCFlagC.ESCCntSec.HWPowerFaultDelay = 0;
		ESCFlagC.ESCCntSec.HWPowerStopDelay = 0;

		return 0;
	}
}

/*
 * ���ܣ�15V��Դ���ϼ�⡣WY
 * �������SoeName������������ʾ�Ĺ�������������WY
 * �������counterName�������źż�������������
 * �������FaultDelayTime�������źŴ�������ֵ��
   �����źż����� < �����źŴ�������ֵ�����������ж���
   �����źż����� > �����źŴ�������ֵ��ִ�й����ж���
 */
int16 PowerFailFaultDetect(Uint16 SoeName,Uint16 counterName,Uint16 FaultDelayTime)
{
	/*
	 * �����źż����� > �����źŴ�������ֵ��ִ�й����ж���WY
	 * Ŀ�ģ���ֹ��⡣
	 */
	if (CntFaultDelay[counterName]++ >= FaultDelayTime)
	{
		DINT;
		if (CTRL24_POWERFlag == 1) //15V��Դ���ϣ��ȴ���Ӧ��WY
		{
			EPwm1Regs.AQCSFRC.bit.CSFA = EPwmRegsAQCSFRCclear; //PWMǿ�Ƴ�������ߵ�ƽ�����ͣ����WY
			CTRL24_POWERFlag = 0; //����Ӧ��WY
		}
		SET_POWER_CTRL(1); //����15V��Դ��WY

		/*����A��*/
		if ((ESCFlagA.FAULTCONFIRFlag == 1) //A����ڹ����źš�WY
				&& (ESCBYRelayCNTA != 1)) //A����·�ű��̵ּ���״̬������WY
		{
			StateEventFlag_A = STATE_EVENT_FAULT_A;//��ת״̬������A�����ͣ����WY
			ESCFlagA.faultFlag = 1; //A����ڹ��ϡ�WY
			FirstFaultStop(ESCFlagA.PHASE); //����ͣ��Ԥ����WY
			ESCFlagA.ESCCntMs.CutCurrDelay = 0;
		}

		/*����B��*/
		if ((ESCFlagB.FAULTCONFIRFlag == 1) && (ESCBYRelayCNTB != 1))
		{
			StateEventFlag_B = STATE_EVENT_FAULT_B;
			ESCFlagB.faultFlag = 1;
			FirstFaultStop(ESCFlagB.PHASE);
			ESCFlagB.ESCCntMs.CutCurrDelay = 0;
		}

		/*����C��*/
		if ((ESCFlagC.FAULTCONFIRFlag == 1) && (ESCBYRelayCNTC != 1))
		{
			StateEventFlag_C = STATE_EVENT_FAULT_C;
			ESCFlagC.faultFlag = 1;
			FirstFaultStop(ESCFlagC.PHASE);
			ESCFlagC.ESCCntMs.CutCurrDelay = 0;
		}

		EINT;

		SoeRecData(SoeName); //������־��WY
		WriteFlashConfig(SoeName); //��Flashд�����¼����WY
		CntFaultDelay[counterName] = 0; //��������źż�������WY

		return 1;
	}
	else
	{
		ESCFlagA.ESCCntSec.HWPowerFaultDelay = 0;
		ESCFlagA.ESCCntSec.HWPowerStopDelay = 0;
		ESCFlagB.ESCCntSec.HWPowerFaultDelay = 0;
		ESCFlagB.ESCCntSec.HWPowerStopDelay = 0;
		ESCFlagC.ESCCntSec.HWPowerFaultDelay = 0;
		ESCFlagC.ESCCntSec.HWPowerStopDelay = 0;

		return 0;
	}
}

/*
 * ���ܣ���·�ű��̵ּ������ϼ�⡣WY
 * �������SoeName������������ʾ�Ĺ�������������WY
 * �������counterName�������źż�������������
 * �������FaultDelayTime�������źŴ�������ֵ��
   �����źż����� < �����źŴ�������ֵ�����������ж���
   �����źż����� > �����źŴ�������ֵ��ִ�й����ж���
 * �������ESCPHASE�����ж��������͵��ࡣ
 */
int16 BYRelayFaultDetect(Uint16 SoeName, Uint16 counterName, Uint16 FaultDelayTime, Uint16 ESCPHASE)
{
	/*
	 * �����źż����� > �����źŴ�������ֵ��ִ�й����ж���WY
	 * Ŀ�ģ���ֹ��⡣
	 */
	if (CntFaultDelay[counterName]++ >= FaultDelayTime)
	{
		DINT;

		if (CTRL24_POWERFlag == 1) //15V��Դ���ϣ��ȴ�ִ����Ӧ��WY
		{
			EPwm1Regs.AQCSFRC.bit.CSFA = EPwmRegsAQCSFRCclear; //PWMǿ�Ƴ�������ߵ�ƽ�����ͣ����WY
			CTRL24_POWERFlag = 0; //����Ӧ��WY
		}

		SET_POWER_CTRL(1); //15V��Դ�ϵ硣WY
		Delayus(TIME_WRITE_15VOLT_REDAY);

		/*����A�ࣺ��·�ű��̵ּ������ϡ�WY*/
		if ((ESCFlagA.FAULTCONFIRFlag == 1) //A����ڹ����źš�WY
				&& (ESCPHASE == 1)) //A�ࡣWY
		{
			StateEventFlag_A = STATE_EVENT_FAULT_A; //�л�״̬������A�����ͣ����WY
			ESCFlagA.faultFlag = 1; //A����ڹ��ϡ�WY
			ESCFlagA.PWM_ins_index = 1; //ֱͨ��·PWM��WY
			ESCFlagA.ESC_DutyData = 1.0; //PWMռ�ձ�Ϊ1��WY
			SET_SCRA_ENABLE(ESC_ACTION_ENABLE); //��ͨA����·��բ�ܡ�WY
			SET_MAIN_CONTACT_ACTION_A(ESC_ACTION_ENABLE); //�պ���·�ű��̵ּ�����WY
			SET_BYPASS_CONTACT_ACTION_A(ESC_ACTION_ENABLE); //�պ���·�ű��̵ּ�����WY
			Delayus(TIME_WRITE_15VOLT_REDAY);
			EnablePWMA(); //�������A��PWM��WY
			Delayus(100);
			SET_IGBT_ENA(IGBT_ENABLE); //Ӳ��ʹ��A��PWM��WY

			EPwm3Regs.CMPA.bit.CMPA = T1PR;
			EPwm3Regs.CMPB.bit.CMPB = T1PR;
			EPwm4Regs.CMPA.bit.CMPA = T1PR;
			EPwm4Regs.CMPB.bit.CMPB = T1PR;

			ESCFlagA.FaultskipFlag = 1;
			ESCFlagA.FAULTCONFIRFlag = 0; //�����ڹ����źš�WY
			ESCFlagA.ESCCntMs.StartDelay = 0;
		}

		/*����B�ࣺ��·�ű��̵ּ������ϡ�WY*/
		if ((ESCFlagB.FAULTCONFIRFlag == 1) && (ESCPHASE == 2))
		{
			StateEventFlag_B = STATE_EVENT_FAULT_B;
			ESCFlagB.faultFlag = 1;
			ESCFlagB.PWM_ins_index = 1;
			ESCFlagB.ESC_DutyData = 1.0;
			SET_SCRB_ENABLE(ESC_ACTION_ENABLE);
			SET_MAIN_CONTACT_ACTION_B(ESC_ACTION_ENABLE);
			SET_BYPASS_CONTACT_ACTION_B(ESC_ACTION_ENABLE);
			Delayus(TIME_WRITE_15VOLT_REDAY);
			EnablePWMB();
			Delayus(100);
			SET_IGBT_ENB(IGBT_ENABLE);
			EPwm5Regs.CMPA.bit.CMPA = T1PR;
			EPwm5Regs.CMPB.bit.CMPB = T1PR;
			EPwm6Regs.CMPA.bit.CMPA = T1PR;
			EPwm6Regs.CMPB.bit.CMPB = T1PR;
			ESCFlagB.FaultskipFlag = 1;
			ESCFlagB.FAULTCONFIRFlag = 0;
			ESCFlagB.ESCCntMs.StartDelay = 0;
		}

		/*����C�ࣺ��·�ű��̵ּ������ϡ�WY*/
		if ((ESCFlagC.FAULTCONFIRFlag == 1) && (ESCPHASE == 3))
		{
			StateEventFlag_C = STATE_EVENT_FAULT_C;
			ESCFlagC.faultFlag = 1;
			ESCFlagC.PWM_ins_index = 1;
			ESCFlagC.ESC_DutyData = 1.0;
			SET_SCRC_ENABLE(ESC_ACTION_ENABLE);
			SET_MAIN_CONTACT_ACTION_C(ESC_ACTION_ENABLE);
			SET_BYPASS_CONTACT_ACTION_C(ESC_ACTION_ENABLE);
			Delayus(TIME_WRITE_15VOLT_REDAY);
			EnablePWMC();
			Delayus(100);
			SET_IGBT_ENC(IGBT_ENABLE);
			EPwm7Regs.CMPA.bit.CMPA = T1PR;
			EPwm7Regs.CMPB.bit.CMPB = T1PR;
			EPwm8Regs.CMPA.bit.CMPA = T1PR;
			EPwm8Regs.CMPB.bit.CMPB = T1PR;
			ESCFlagC.FaultskipFlag = 1;
			ESCFlagC.FAULTCONFIRFlag = 0;
			ESCFlagC.ESCCntMs.StartDelay = 0;
		}
		EINT;

		SoeRecData(SoeName);//������־��WY
		WriteFlashConfig(SoeName); //��Flashд�����¼����WY
		CntFaultDelay[counterName] = 0; //��������źż�������WY

		return 1;
	}
	else //�����źż����� <= �����źŴ�������ֵ��WY
	{
		ESCFlagA.ESCCntSec.HWPowerFaultDelay = 0;
		ESCFlagA.ESCCntSec.HWPowerStopDelay = 0;
		ESCFlagB.ESCCntSec.HWPowerFaultDelay = 0;
		ESCFlagB.ESCCntSec.HWPowerStopDelay = 0;
		ESCFlagC.ESCCntSec.HWPowerFaultDelay = 0;
		ESCFlagC.ESCCntSec.HWPowerStopDelay = 0;

		return 0;
	}
}

/*�ú���δʹ�á�WY*/
int16 FaultDetectExtCnt(Uint16 SoeName,Uint16 extCnt,Uint16 FaultDelayTime)
{
	if(FaultDelayTime >= extCnt) //��Ԫ��Ӹ���̫�������
		{
	#if TEST_IGBTFAULTDISABLE == 0
	    FirstFaultStop(ESCFlagA.PHASE);
	    FirstFaultStop(ESCFlagB.PHASE);
	    FirstFaultStop(ESCFlagC.PHASE);
	#if !TEST_RUNTIME
		StateEventFlag_A = STATE_EVENT_FAULT_A;//???
		StateEventFlag_B = STATE_EVENT_FAULT_B;//???
		StateEventFlag_C = STATE_EVENT_FAULT_C;//???
	#endif
        ESCFlagA.faultFlag = 1;
        ESCFlagB.faultFlag = 1;
        ESCFlagC.faultFlag = 1;
	#endif
        ESCFlagA.stopFlag = 1;
        ESCFlagB.stopFlag = 1;
        ESCFlagC.stopFlag = 1;
        ESCFlagA.startFlag = 0;
        ESCFlagB.startFlag = 0;
        ESCFlagC.startFlag = 0;
		SoeRecData(SoeName);
		return 1;
	}else{
		return 0;
	}
}

/*�ú���δʹ�á�WY*/
int16 FaultDetectLong(Uint16 SoeName,Uint16 counterName,Uint32 FaultDelayTime)
{
	if(CntFaultDelayLong[counterName]++ > FaultDelayTime) //��Ԫ��Ӹ���̫�������
	{
		#if TEST_IGBTFAULTDISABLE == 0
        FirstFaultStop(ESCFlagA.PHASE);
        FirstFaultStop(ESCFlagB.PHASE);
        FirstFaultStop(ESCFlagC.PHASE);
			StateEventFlag_A = STATE_EVENT_FAULT_A;
			StateEventFlag_B = STATE_EVENT_FAULT_B;
			StateEventFlag_C = STATE_EVENT_FAULT_C;
	        ESCFlagA.faultFlag = 1;
	        ESCFlagB.faultFlag = 1;
	        ESCFlagC.faultFlag = 1;
		#endif
		SoeRecData(SoeName);
		CntFaultDelayLong[counterName] = 0;
		return 1;
	}else{
		return 0;
		}
}

/*�ú���δʹ�á�WY*/
int16 StandbyDetectLong(Uint16 SoeName,Uint16 counterName,Uint32 StandbyDelayTime)
{
    if(CntFaultDelayLong[counterName]++ > StandbyDelayTime) //��Ԫ��Ӹ���̫�������
    {
        #if TEST_IGBTFAULTDISABLE == 0
        FirstFaultStop(ESCFlagA.PHASE);
        FirstFaultStop(ESCFlagB.PHASE);
        FirstFaultStop(ESCFlagC.PHASE);
        StandByFlag = 1;
        DisablePWMA();
        DisablePWMB();
        DisablePWMC();
        SET_IGBT_ENA(IGBT_DISABLE);
        SET_IGBT_ENB(IGBT_DISABLE);
        SET_IGBT_ENC(IGBT_DISABLE);
        SET_SCRA_ENABLE(ESC_ACTION_DISABLE);
        SET_SCRB_ENABLE(ESC_ACTION_DISABLE);
        SET_SCRC_ENABLE(ESC_ACTION_DISABLE);
        SET_BYPASS_CONTACT_ACTION_A(ESC_ACTION_ENABLE);                            //����·
        SET_BYPASS_CONTACT_ACTION_B(ESC_ACTION_ENABLE);
        SET_BYPASS_CONTACT_ACTION_C(ESC_ACTION_ENABLE);
        StateEventFlag_A = STATE_EVENT_FAULT_A;
        StateEventFlag_B = STATE_EVENT_FAULT_B;
        StateEventFlag_C = STATE_EVENT_FAULT_C;
        StateFlag.waitFlag = 1;
        #endif
        SoeRecData(SoeName);
        CntFaultDelayLong[counterName] = 0;
        return 1;
    }else{
        return 0;
        }
}

/*�ú���δʹ�á�WY*/
int16 StandbyDetect(Uint16 SoeName,Uint16 counterName,Uint32 StandbyDelayTime)
{
    if(CntFaultDelayLong[counterName]++ > StandbyDelayTime) //��Ԫ��Ӹ���̫�������
    {
        #if TEST_IGBTFAULTDISABLE == 0
        FirstFaultStop(ESCFlagA.PHASE);
        FirstFaultStop(ESCFlagB.PHASE);
        FirstFaultStop(ESCFlagC.PHASE);
        StandByFlag = 1;
        DisablePWMA();
        DisablePWMB();
        DisablePWMC();
        SET_IGBT_ENA(IGBT_DISABLE);
        SET_IGBT_ENB(IGBT_DISABLE);
        SET_IGBT_ENC(IGBT_DISABLE);
        SET_SCRA_ENABLE(ESC_ACTION_DISABLE);
        SET_SCRB_ENABLE(ESC_ACTION_DISABLE);
        SET_SCRC_ENABLE(ESC_ACTION_DISABLE);
        SET_BYPASS_CONTACT_ACTION_A(ESC_ACTION_ENABLE);                            //����·
        SET_BYPASS_CONTACT_ACTION_B(ESC_ACTION_ENABLE);
        SET_BYPASS_CONTACT_ACTION_C(ESC_ACTION_ENABLE);
        StateEventFlag_A = STATE_EVENT_FAULT_A;
        StateEventFlag_B = STATE_EVENT_FAULT_B;
        StateEventFlag_C = STATE_EVENT_FAULT_C;
        StateFlag.waitFlag = 1;
        #endif
        SoeRecData(SoeName);
        CntFaultDelayLong[counterName] = 0;
        return 1;
    }else{
        return 0;
        }
}

/*
 * ���ܣ���������źż�������ֵ��WY
 * �������counterName�������źż�����������
 * �������mode���̶�Ϊ0��
 */
void SetFaultDelayCounter(Uint16 counterName,Uint16 mode)
{
	if(mode==0)
		CntFaultDelay[counterName]=0;
	else
		if(CntFaultDelay[counterName] > mode)
			CntFaultDelay[counterName] -= mode;
		else
			CntFaultDelay[counterName] = 0;
}

/*�ú���δʹ�á�WY*/
void SetFaultDelayCounterLong(Uint16 counterName,Uint32 mode)
{
	if(CntFaultDelayLong[counterName] > mode)
		CntFaultDelayLong[counterName] -= mode;
	else
		CntFaultDelayLong[counterName] = 0;
}

/*�ú���δʹ�á�WY*/
void FaultDetectInInt(void)
{}

/*
 * ECAP-1�жϷ�������WY
 * �������A��Ӳ������
 * �ź��������ţ�GPIO94
 */
void EcapINT1(void)
{
	if (StateEventFlag_A == STATE_EVENT_RUN_A) //A�ദ������״̬��WY
	{
		ESCFlagA.FAULTCONFIRFlag = 1; //���ڹ����źš�WY

		if ((softwareFaultWord1.B.ESCFastHardwareOverCurFlagA == 0)) //A�಻����Ӳ���������ϡ�WY
		{
			ESCFlagA.ESCCntMs.StartDelay = 0;
			softwareFaultWord1.B.ESCFastHardwareOverCurFlagA = FaultDetect(SOE_GP_FAULT + 1, CNT_HW_OVER_CUR_A, 0, ESCFlagA.PHASE); //A�����Ӳ���������ϡ�WY
		}
	}
	else
	{
		SetFaultDelayCounter(CNT_HW_OVER_CUR_A, 0); //����A��Ӳ�����������źŴ�����WY
	}

	ECap1Regs.ECCLR.bit.CEVT4 = 1; //��λ�¼�-4��־λ��WY
	ECap1Regs.ECCLR.bit.CEVT3 = 1; //��λ�¼�-3��־λ��WY
	ECap1Regs.ECCLR.bit.CEVT2 = 1; //��λ�¼�-2��־λ��WY
	ECap1Regs.ECCLR.bit.CEVT1 = 1; //��λ�¼�-1��־λ��WY
	ECap1Regs.ECCLR.bit.INT = 1; //��λ�жϱ�־λ��WY

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP4; //��λ�ж��顣WY
}

/*
 * ECAP-2�жϷ�������WY
 * �������B��Ӳ������
 * �ź��������ţ�GPIO85
 */
void EcapINT2(void)
{
	if (StateEventFlag_B == STATE_EVENT_RUN_B) //B�ദ������״̬��WY
	{
		ESCFlagB.FAULTCONFIRFlag = 1; //���ڹ����źš�WY

		if ((softwareFaultWord1.B.ESCFastHardwareOverCurFlagB == 0)) //B�಻����Ӳ���������ϡ�WY
		{
			ESCFlagB.ESCCntMs.StartDelay = 0;
			softwareFaultWord1.B.ESCFastHardwareOverCurFlagB = FaultDetect(SOE_GP_FAULT + 2, CNT_HW_OVER_CUR_B, 0, ESCFlagB.PHASE); //B�����Ӳ���������ϡ�WY
		}
	}
	else
	{
		SetFaultDelayCounter(CNT_HW_OVER_CUR_B, 0);
	}

	ECap2Regs.ECCLR.bit.CEVT4 = 1; //��λ�¼�-4��־λ��WY
	ECap2Regs.ECCLR.bit.CEVT3 = 1; //��λ�¼�-3��־λ��WY
	ECap2Regs.ECCLR.bit.CEVT2 = 1; //��λ�¼�-2��־λ��WY
	ECap2Regs.ECCLR.bit.CEVT1 = 1; //��λ�¼�-1��־λ��WY
	ECap2Regs.ECCLR.bit.INT = 1; //��λ�жϱ�־λ��WY
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP4; //��λ�ж��顣WY
}

/*
 * ECAP-3�жϷ�������WY
 * �������C��Ӳ������
 * �ź��������ţ�GPIO97
 */
void EcapINT3(void)
{
	if (StateEventFlag_C == STATE_EVENT_RUN_C) //C�ദ������״̬��WY
	{
		ESCFlagC.FAULTCONFIRFlag = 1; //���ڹ����źš�WY

		if ((softwareFaultWord1.B.ESCFastHardwareOverCurFlagC == 0)) //C�಻����Ӳ���������ϡ�WY
		{
			ESCFlagC.ESCCntMs.StartDelay = 0;
			softwareFaultWord1.B.ESCFastHardwareOverCurFlagC = FaultDetect(SOE_GP_FAULT + 3, CNT_HW_OVER_CUR_C, 0, ESCFlagC.PHASE); //C�����Ӳ���������ϡ�WY
		}
	}
	else
	{
		SetFaultDelayCounter(CNT_HW_OVER_CUR_C, 0);
	}
	ECap3Regs.ECCLR.bit.CEVT4 = 1; //��λ�¼�-4��־λ��WY
	ECap3Regs.ECCLR.bit.CEVT3 = 1; //��λ�¼�-4��־λ��WY
	ECap3Regs.ECCLR.bit.CEVT2 = 1; //��λ�¼�-4��־λ��WY
	ECap3Regs.ECCLR.bit.CEVT1 = 1; //��λ�¼�-4��־λ��WY
	ECap3Regs.ECCLR.bit.INT = 1; //��λ�жϱ�־λ��WY
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP4; //��λ�ж��顣WY
}

/*
 * ���ܣ����ٹ��ϼ�⡣WY
 * ˵�����ú�����ADC-D-1���жϷ������б����ã�����Ƶ��20K����������50us��WY
 */
void FaultFastDetectInInt(void)
{
//���ڸù����ж��Ƿ����ж���,���ϵ������ƫУ׼��ʱ��ϳ�,����Ҫ����״̬�ж�,��ֹ����ƫУ׼ʱ,����ù���,ʹ״̬λESCFlagA.FAULTCONFIRFlag��1;
	if (PhaseControl == 1)
	{
		/*����A�ࣺ˲ʱֵ��������*/
		if (((GridCurrAF > OUTCUR_OVER_INS) //A���������˲ʱֵ > ����˲ʱֵ���ޡ�WY
				|| (GridBPCurrAF > OUTCUR_OVER_INS)) //A����·����˲ʱֵ > ����˲ʱֵ���ޡ�WY
				&& (StateEventFlag_A != STATE_EVENT_STANDBY_A)) //A�಻���ڳ�ʼ״̬��WY
		{
			ESCFlagA.FAULTCONFIRFlag = 1; //���ڹ����źš�WY

			if (softwareFaultWord1.B.ESCInsOverCurFlagA == 0) //������A��˲ʱֵ�������ϡ�WY
			{
				ESCFlagA.ESCCntMs.StartDelay = 0;
				softwareFaultWord1.B.ESCInsOverCurFlagA = FaultDetect(SOE_GP_FAULT + 4, CNT_INS_OVER_CUR_A, 3, ESCFlagA.PHASE); //����A��˲ʱֵ�������ϡ�WY
			}
		}
		else
		{
			SetFaultDelayCounter(CNT_INS_OVER_CUR_A, 0); //���㣺A��˲ʱֵ���������źŴ�����WY
		}

		/*����B�ࣺ˲ʱֵ��������*/
		if (((GridCurrBF > OUTCUR_OVER_INS) || (GridBPCurrBF > OUTCUR_OVER_INS)) && (StateEventFlag_B != STATE_EVENT_STANDBY_B))
		{
			ESCFlagB.FAULTCONFIRFlag = 1;
			if (softwareFaultWord1.B.ESCInsOverCurFlagB == 0)
			{
				ESCFlagB.ESCCntMs.StartDelay = 0;
				softwareFaultWord1.B.ESCInsOverCurFlagB = FaultDetect(SOE_GP_FAULT + 5, CNT_INS_OVER_CUR_B, 3, ESCFlagB.PHASE);
			}
		}
		else
		{
			SetFaultDelayCounter(CNT_INS_OVER_CUR_B, 0);
		}

		/*����C�ࣺ˲ʱֵ��������*/
		if (((GridCurrCF > OUTCUR_OVER_INS) || (GridBPCurrCF > OUTCUR_OVER_INS)) && (StateEventFlag_C != STATE_EVENT_STANDBY_C))
		{
			ESCFlagC.FAULTCONFIRFlag = 1;
			if (softwareFaultWord1.B.ESCInsOverCurFlagC == 0)
			{
				ESCFlagC.ESCCntMs.StartDelay = 0;
				softwareFaultWord1.B.ESCInsOverCurFlagC = FaultDetect(SOE_GP_FAULT + 6, CNT_INS_OVER_CUR_C, 3, ESCFlagC.PHASE);
			}
		}
		else
		{
			SetFaultDelayCounter(CNT_INS_OVER_CUR_C, 0);
		}
	}
	else if (PhaseControl == 2)
	{
		/*����A�ࣺ˲ʱֵ��������*/
		if (((GridCurrAF < OUTCUR_OVER_INS_NEG) //A���������˲ʱֵ < ����˲ʱֵ���ޡ�WY
				|| (GridBPCurrAF < OUTCUR_OVER_INS_NEG)) //A�ฺ�ص���˲ʱֵ < ����˲ʱֵ���ޡ�WY
				&& (StateEventFlag_A != STATE_EVENT_STANDBY_A)) //A�಻���ڳ�ʼ״̬��WY
		{
			ESCFlagA.FAULTCONFIRFlag = 1; //���ڹ����źš�WY

			if (softwareFaultWord1.B.ESCInsOverCurFlagA == 0) //������A��˲ʱֵ�������ϡ�WY
			{
				ESCFlagA.ESCCntMs.StartDelay = 0;
				softwareFaultWord1.B.ESCInsOverCurFlagA = FaultDetect(SOE_GP_FAULT + 4, CNT_INS_OVER_CUR_A, 3, ESCFlagA.PHASE); //����A��˲ʱֵ�������ϡ�WY
			}
		}
		else
		{
			SetFaultDelayCounter(CNT_INS_OVER_CUR_A, 0); //���㣺A��˲ʱֵ���������źŴ�����WY
		}

		/*����B�ࣺ˲ʱֵ��������*/
		if (((GridCurrBF < OUTCUR_OVER_INS_NEG) || (GridBPCurrBF < OUTCUR_OVER_INS_NEG)) && (StateEventFlag_B != STATE_EVENT_STANDBY_B))
		{
			ESCFlagB.FAULTCONFIRFlag = 1;
			if (softwareFaultWord1.B.ESCInsOverCurFlagB == 0)
			{
				ESCFlagB.ESCCntMs.StartDelay = 0;
				softwareFaultWord1.B.ESCInsOverCurFlagB = FaultDetect(SOE_GP_FAULT + 5, CNT_INS_OVER_CUR_B, 3, ESCFlagB.PHASE);
			}
		}
		else
		{
			SetFaultDelayCounter(CNT_INS_OVER_CUR_B, 0);
		}

		/*����C�ࣺ˲ʱֵ��������*/
		if (((GridCurrCF < OUTCUR_OVER_INS_NEG) || (GridBPCurrCF < OUTCUR_OVER_INS_NEG)) && (StateEventFlag_C != STATE_EVENT_STANDBY_C))
		{
			ESCFlagC.FAULTCONFIRFlag = 1;
			if (softwareFaultWord1.B.ESCInsOverCurFlagC == 0)
			{
				ESCFlagC.ESCCntMs.StartDelay = 0;
				softwareFaultWord1.B.ESCInsOverCurFlagC = FaultDetect(SOE_GP_FAULT + 6, CNT_INS_OVER_CUR_C, 3, ESCFlagC.PHASE);
			}
		}
		else
		{
			SetFaultDelayCounter(CNT_INS_OVER_CUR_C, 0);
		}
	}
	else if (PhaseControl == 0)
	{
		/*����A�ࣺ˲ʱֵ��������*/
		if (((GridCurrAF > OUTCUR_OVER_INS) //A���������˲ʱֵ > ����˲ʱֵ���ޡ�WY
				|| (GridCurrAF < OUTCUR_OVER_INS_NEG) //A���������˲ʱֵ < ����˲ʱֵ���ޡ�WY
				|| (GridBPCurrAF > OUTCUR_OVER_INS) //A����·����˲ʱֵ > ����˲ʱֵ���ޡ�WY
				|| (GridBPCurrAF < OUTCUR_OVER_INS_NEG)) //A����·����˲ʱֵ < ����˲ʱֵ���ޡ�WY
				&& (StateEventFlag_A != STATE_EVENT_STANDBY_A)) //A�಻���ڳ�ʼ״̬��WY
		{
			ESCFlagA.FAULTCONFIRFlag = 1; //���ڹ����źš�WY

			if (softwareFaultWord1.B.ESCInsOverCurFlagA == 0) //������A��˲ʱֵ�������ϡ�WY
			{
				ESCFlagA.ESCCntMs.StartDelay = 0;
				softwareFaultWord1.B.ESCInsOverCurFlagA = FaultDetect(SOE_GP_FAULT + 4, CNT_INS_OVER_CUR_A, 3, ESCFlagA.PHASE); //����A��˲ʱֵ�������ϡ�WY
			}
		}
		else
		{
			SetFaultDelayCounter(CNT_INS_OVER_CUR_A, 0); //���㣺A��˲ʱֵ���������źŴ�����WY
		}

		/*����B�ࣺ˲ʱֵ��������*/
		if (((GridCurrBF > OUTCUR_OVER_INS) || (GridCurrBF < OUTCUR_OVER_INS_NEG) || (GridBPCurrBF > OUTCUR_OVER_INS) || (GridBPCurrBF < OUTCUR_OVER_INS_NEG))
				&& (StateEventFlag_B != STATE_EVENT_STANDBY_B))
		{
			ESCFlagB.FAULTCONFIRFlag = 1;
			if (softwareFaultWord1.B.ESCInsOverCurFlagB == 0)
			{
				ESCFlagB.ESCCntMs.StartDelay = 0;
				softwareFaultWord1.B.ESCInsOverCurFlagB = FaultDetect(SOE_GP_FAULT + 5, CNT_INS_OVER_CUR_B, 3, ESCFlagB.PHASE);
			}
		}
		else
		{
			SetFaultDelayCounter(CNT_INS_OVER_CUR_B, 0);
		}

		/*����C�ࣺ˲ʱֵ��������*/
		if (((GridCurrCF > OUTCUR_OVER_INS) || (GridCurrCF < OUTCUR_OVER_INS_NEG) || (GridBPCurrCF > OUTCUR_OVER_INS) || (GridBPCurrCF < OUTCUR_OVER_INS_NEG))
				&& (StateEventFlag_C != STATE_EVENT_STANDBY_C))
		{
			ESCFlagC.FAULTCONFIRFlag = 1;
			if (softwareFaultWord1.B.ESCInsOverCurFlagC == 0)
			{
				ESCFlagC.ESCCntMs.StartDelay = 0;
				softwareFaultWord1.B.ESCInsOverCurFlagC = FaultDetect(SOE_GP_FAULT + 6, CNT_INS_OVER_CUR_C, 3, ESCFlagC.PHASE);
			}
		}
		else
		{
			SetFaultDelayCounter(CNT_INS_OVER_CUR_C, 0);
		}
	}

	/*A�ֱࣺ�����ݵ�ѹ��ѹ����*/
	if (DccapVoltA > ESCDCVOLITLIMIT) //A��ֱ�����ݵ�ѹ > ���ݵ�ѹ����ֵ.WY
	{
		ESCFlagA.FAULTCONFIRFlag = 1; //A����ڹ��ϡ�WY

		if (softwareFaultWord1.B.ESCDcCapVoltOverVoltFlagA == 0) //������A��ֱ�����ݵ�ѹ��ѹ���ϡ�WY
		{
			ESCFlagA.ESCCntMs.StartDelay = 0;
			softwareFaultWord1.B.ESCDcCapVoltOverVoltFlagA = FaultDetect(SOE_GP_FAULT + 7, CNT_DC_NEUTRAL_OVER_VOLT_A, 10, ESCFlagA.PHASE); //����A��ֱ�����ݵ�ѹ��ѹ���ϡ�WY
		}
	}
	else
	{
		SetFaultDelayCounter(CNT_DC_NEUTRAL_OVER_VOLT_A, 0); //���㣺A��ֱ�����ݵ�ѹ��ѹ�����źŴ���
	}

	/*B�ֱࣺ�����ݵ�ѹ��ѹ����*/
	if (DccapVoltB > ESCDCVOLITLIMIT)
	{
		ESCFlagB.FAULTCONFIRFlag = 1;
		if (softwareFaultWord1.B.ESCDcCapVoltOverVoltFlagB == 0)
		{
			ESCFlagB.ESCCntMs.StartDelay = 0;
			softwareFaultWord1.B.ESCDcCapVoltOverVoltFlagB = FaultDetect(SOE_GP_FAULT + 8, CNT_DC_NEUTRAL_OVER_VOLT_B, 10, ESCFlagB.PHASE);
		}
	}
	else
	{
		SetFaultDelayCounter(CNT_DC_NEUTRAL_OVER_VOLT_B, 0);
	}

	/*C�ֱࣺ�����ݵ�ѹ��ѹ����*/
	if (DccapVoltC > ESCDCVOLITLIMIT)
	{
		ESCFlagC.FAULTCONFIRFlag = 1;
		if (softwareFaultWord1.B.ESCDcCapVoltOverVoltFlagC == 0)
		{
			ESCFlagC.ESCCntMs.StartDelay = 0;
			softwareFaultWord1.B.ESCDcCapVoltOverVoltFlagC = FaultDetect(SOE_GP_FAULT + 9, CNT_DC_NEUTRAL_OVER_VOLT_C, 10, ESCFlagC.PHASE);
		}
	}
	else
	{
		SetFaultDelayCounter(CNT_DC_NEUTRAL_OVER_VOLT_C, 0);
	}

	/*A��:��ѹ��Чֵ��ѹ���ϡ�WY*/
	if (((VoltOutA_rms >= GV_RMS_OVER) //A�ฺ�ص�ѹ��Чֵ > ��ѹ��Чֵ���ޡ�WY
			|| (VoltInA_rms >= GV_RMS_OVER)) //A�������ѹ��Чֵ > ��ѹ��Чֵ���ޡ�WY
			&& (StateEventFlag_A != STATE_EVENT_STANDBY_A)) //A�ദ�ڷ�ͣ��״̬��WY
	{
		ESCFlagA.FAULTCONFIRFlag = 1; //���ڹ����źš�WY

		if (softwareFaultWord1.B.ESCGridRMSOverVoltFlagA == 0) //������A�������ѹ��Чֵ��ѹ���ϡ�WY
		{
			ESCFlagA.ESCCntMs.StartDelay = 0;
			softwareFaultWord1.B.ESCGridRMSOverVoltFlagA = FaultDetect(SOE_GP_FAULT + 10, CNT_RMS_OVER_VOLT_A, 20, ESCFlagA.PHASE); //����A���ѹ��Чֵ��ѹ���ϡ�WY
		}
	}
	else
	{
		SetFaultDelayCounter(CNT_RMS_OVER_VOLT_A, 0); //���㣺A���ѹ��Чֵ��ѹ�����źŴ�����WY
	}

	/*B�ࣺ��ѹ��Чֵ��ѹ���ϡ�WY*/
	if (((VoltOutB_rms >= GV_RMS_OVER) || (VoltInB_rms >= GV_RMS_OVER)) && (StateEventFlag_B != STATE_EVENT_STANDBY_B))
	{
		ESCFlagB.FAULTCONFIRFlag = 1;
		if (softwareFaultWord1.B.ESCGridRMSOverVoltFlagB == 0)
		{
			ESCFlagB.ESCCntMs.StartDelay = 0;
			softwareFaultWord1.B.ESCGridRMSOverVoltFlagB = FaultDetect(SOE_GP_FAULT + 11, CNT_RMS_OVER_VOLT_B, 20, ESCFlagB.PHASE);
		}
	}
	else
	{
		SetFaultDelayCounter(CNT_RMS_OVER_VOLT_B, 0);
	}

	/*C�ࣺ��ѹ��Чֵ��ѹ���ϡ�WY*/
	if (((VoltOutC_rms >= GV_RMS_OVER) || (VoltInC_rms >= GV_RMS_OVER)) && (StateEventFlag_C != STATE_EVENT_STANDBY_C))
	{
		ESCFlagC.FAULTCONFIRFlag = 1;
		if (softwareFaultWord1.B.ESCGridRMSOverVoltFlagC == 0)
		{
			ESCFlagC.ESCCntMs.StartDelay = 0;
			softwareFaultWord1.B.ESCGridRMSOverVoltFlagC = FaultDetect(SOE_GP_FAULT + 12, CNT_RMS_OVER_VOLT_C, 20, ESCFlagC.PHASE);
		}
	}
	else
	{
		SetFaultDelayCounter(CNT_RMS_OVER_VOLT_C, 0);
	}

//TempEnvirProvalue==1ʱ,��������ʵ��,��Ҫ���ȫ��ȫ��ת,���ǿ��ص�Դ��������,�ᱨ�������,������������ʵ���ʱ����Ҫ�Ѹù������ε�.

	/*15V��Դ�������*/
	if (TempEnvirProvalue != 1)
	{
		if (GET_CTRL24_POWER == 1) //15V��Դ�쳣��WY
		{
			CTRL24_POWERFlag = 1; //15V��Դ���ϡ�WY

			ESCFlagA.FAULTCONFIRFlag = 1; //A����ڹ����źš�WY
			ESCFlagB.FAULTCONFIRFlag = 1; //B����ڹ����źš�WY
			ESCFlagC.FAULTCONFIRFlag = 1; //C����ڹ����źš�WY

			if (softwareFaultWord1.B.ESCPowerFailDetectFlag == 0) //������15V��Դ��������ϡ�WY
			{
				ESCFlagA.ESCCntMs.StartDelay = 0;
				ESCFlagB.ESCCntMs.StartDelay = 0;
				ESCFlagC.ESCCntMs.StartDelay = 0;
				softwareFaultWord1.B.ESCPowerFailDetectFlag = PowerFailFaultDetect(SOE_GP_FAULT + 13, CNT_CTRL_POWER_OFF, 10); //����15V��Դ��������ϡ�WY
			}
		}
		else
		{
			SetFaultDelayCounter(CNT_CTRL_POWER_OFF, 0); //���㣺15V��Դ����������źŴ�����WY
		}
	}
}

/*
 * ���ܣ����ϼ�⡣WY
 *
 * ˵�����ú�����RTOS�����б����ã���������Լ40ms��WY
 */
void FaultDetectInMainLoop(void)
{
	/*A���ѹ��ЧֵǷѹ���ϡ�WY*/
	if (((VoltInA_rms <= GV_RMS_UNDER) //A�������ѹ��Чֵ <= ��ѹ��Чֵ���ޡ�WY
			|| (VoltOutA_rms <= GV_RMS_UNDER)) //A�ฺ�ص�ѹ��Чֵ <= ��ѹ��Чֵ���ޡ�WY
			&& ((StateEventFlag_A == STATE_EVENT_RUN_A) //A�ദ������״̬��WY
			|| (StateEventFlag_A == STATE_EVENT_WAIT_A))) //A�ദ�ڴ���״̬��WY
	{
		ESCFlagA.FAULTCONFIRFlag = 1; //A����ڹ����źš�WY

		if (!softwareFaultWord1.B.ESCGridRMSUnderVoltFlagA) //������A���ѹ��ЧֵǷѹ���ϡ�WY
		{
			ESCFlagA.ESCCntMs.StartDelay = 0;
			softwareFaultWord1.B.ESCGridRMSUnderVoltFlagA = FaultDetect(SOE_GP_FAULT + 14, CNT_RMS_UNDER_VOLT_A, 20, ESCFlagA.PHASE); //����A���ѹ��ЧֵǷѹ���ϡ�WY
		}
	}
	else
	{
		SetFaultDelayCounter(CNT_RMS_UNDER_VOLT_A, 0); //���㣺A���ѹ��ЧֵǷѹ�����źŴ�����WY
	}

	/*B���ѹ��ЧֵǷѹ���ϡ�WY*/
	if (((VoltInB_rms <= GV_RMS_UNDER) || (VoltOutB_rms <= GV_RMS_UNDER))
			&& ((StateEventFlag_B == STATE_EVENT_RUN_B) || (StateEventFlag_B == STATE_EVENT_WAIT_B)))
	{
		ESCFlagB.FAULTCONFIRFlag = 1;
		if (!softwareFaultWord1.B.ESCGridRMSUnderVoltFlagB)
		{
			ESCFlagB.ESCCntMs.StartDelay = 0;
			softwareFaultWord1.B.ESCGridRMSUnderVoltFlagB = FaultDetect(SOE_GP_FAULT + 15, CNT_RMS_UNDER_VOLT_B, 20, ESCFlagB.PHASE);
		}
	}
	else
	{
		SetFaultDelayCounter(CNT_RMS_UNDER_VOLT_B, 0);
	}

	/*C���ѹ��ЧֵǷѹ���ϡ�WY*/
	if (((VoltInC_rms <= GV_RMS_UNDER) || (VoltOutC_rms <= GV_RMS_UNDER))
			&& ((StateEventFlag_C == STATE_EVENT_RUN_C) || (StateEventFlag_C == STATE_EVENT_WAIT_C)))
	{
		ESCFlagC.FAULTCONFIRFlag = 1;
		if (!softwareFaultWord1.B.ESCGridRMSUnderVoltFlagC)
		{
			ESCFlagC.ESCCntMs.StartDelay = 0;
			softwareFaultWord1.B.ESCGridRMSUnderVoltFlagC = FaultDetect(SOE_GP_FAULT + 16, CNT_RMS_UNDER_VOLT_C, 20, ESCFlagC.PHASE);
		}
	}
	else
	{
		SetFaultDelayCounter(CNT_RMS_UNDER_VOLT_C, 0);
	}

	/*A�������Чֵ���ع��ϡ�WY*/
	if ((gridCurA_rms > OUT_OVER_LOAD) //A�����������Чֵ > ������Чֵ�������ޡ�WY
			|| (gridCurrBYAF_rms > OUT_OVER_LOAD)) //A����·������Чֵ > ������Чֵ�������ޡ�WY
	{
		ESCFlagA.FAULTCONFIRFlag = 1; //A����ڹ����źš�WY

		if (softwareFaultWord2.B.ESCOverLoadFlagA == 0) //������A��������ع��ϡ�WY
		{
			ESCFlagA.ESCCntMs.StartDelay = 0;
			softwareFaultWord2.B.ESCOverLoadFlagA = FaultDetect(SOE_GP_FAULT + 17, LCNT_LOAD_OVER_CUR_A, 1510, ESCFlagA.PHASE); //����A��������ع��ϡ�WY
		}
	}
	else
	{
		SetFaultDelayCounter(LCNT_LOAD_OVER_CUR_A, 2); //���㣺A��������ع����źŴ�����WY
	}

	/*B�������Чֵ���ع��ϡ�WY*/
	if ((gridCurB_rms > OUT_OVER_LOAD) || (gridCurrBYBF_rms > OUT_OVER_LOAD))
	{
		ESCFlagB.FAULTCONFIRFlag = 1;
		if (softwareFaultWord2.B.ESCOverLoadFlagB == 0)
		{
			ESCFlagB.ESCCntMs.StartDelay = 0;
			softwareFaultWord2.B.ESCOverLoadFlagB = FaultDetect(SOE_GP_FAULT + 18, LCNT_LOAD_OVER_CUR_B, 1510, ESCFlagB.PHASE);
		}
	}
	else
	{
		SetFaultDelayCounter(LCNT_LOAD_OVER_CUR_B, 2);
	}

	/*C�������Чֵ���ع��ϡ�WY*/
	if ((gridCurC_rms > OUT_OVER_LOAD) || (gridCurrBYCF_rms > OUT_OVER_LOAD))
	{
		ESCFlagC.FAULTCONFIRFlag = 1;
		if (softwareFaultWord2.B.ESCOverLoadFlagC == 0)
		{
			ESCFlagC.ESCCntMs.StartDelay = 0;
			softwareFaultWord2.B.ESCOverLoadFlagC = FaultDetect(SOE_GP_FAULT + 19, LCNT_LOAD_OVER_CUR_C, 1510, ESCFlagC.PHASE);
		}
	}
	else
	{
		SetFaultDelayCounter(LCNT_LOAD_OVER_CUR_C, 2);
	}

	/*A�������Чֵ�������ϡ�WY*/
	if ((gridCurA_rms > OUTCUR_OVER_RMS) //A�����������Чֵ > ������Чֵ�������ޡ�WY
			|| (gridCurrBYAF_rms > OUTCUR_OVER_RMS)) //A����·������Чֵ > ������Чֵ�������ޡ�WY
	{
		ESCFlagA.FAULTCONFIRFlag = 1;
		if (softwareFaultWord2.B.ESCRmsOverCurrentFlagA == 0)
		{
			ESCFlagA.ESCCntMs.StartDelay = 0;
			softwareFaultWord2.B.ESCRmsOverCurrentFlagA = FaultDetect(SOE_GP_FAULT + 20, CNT_RMS_OVER_CUR_A, 2, ESCFlagA.PHASE);
		}
	}
	else
	{
		SetFaultDelayCounter(CNT_RMS_OVER_CUR_A, 0);
	}

	/*B�������Чֵ�������ϡ�WY*/
	if ((gridCurB_rms > OUTCUR_OVER_RMS) || (gridCurrBYBF_rms > OUTCUR_OVER_RMS))
	{
		ESCFlagB.FAULTCONFIRFlag = 1;
		if (softwareFaultWord2.B.ESCRmsOverCurrentFlagB == 0)
		{
			ESCFlagB.ESCCntMs.StartDelay = 0;
			softwareFaultWord2.B.ESCRmsOverCurrentFlagB = FaultDetect(SOE_GP_FAULT + 21, CNT_RMS_OVER_CUR_B, 2, ESCFlagB.PHASE);
		}
	}
	else
	{
		SetFaultDelayCounter(CNT_RMS_OVER_CUR_B, 1);
	}

	/*C�������Чֵ�������ϡ�WY*/
	if ((gridCurC_rms > OUTCUR_OVER_RMS) || (gridCurrBYCF_rms > OUTCUR_OVER_RMS))
	{
		ESCFlagC.FAULTCONFIRFlag = 1;
		if (softwareFaultWord2.B.ESCRmsOverCurrentFlagC == 0)
		{
			ESCFlagC.ESCCntMs.StartDelay = 0;
			softwareFaultWord2.B.ESCRmsOverCurrentFlagC = FaultDetect(SOE_GP_FAULT + 22, CNT_RMS_OVER_CUR_C, 2, ESCFlagC.PHASE);
		}
	}
	else
	{
		SetFaultDelayCounter(CNT_RMS_OVER_CUR_C, 1);
	}

	/*A����·�ű��̵ּ���δ��բ���ϡ�WY*/
	if ((fabs(VoltInA_rms - VoltOutA_rms) > 20) // |A�������ѹ��Чֵ - A�ฺ�ص�ѹ��Чֵ| > 20��WY
			&& (StateEventFlag_A == STATE_EVENT_WAIT_A)) //A�ദ�ڴ���״̬��WY
	{
		ESCFlagA.FAULTCONFIRFlag = 1; //A����ڹ����źš�WY

		if (softwareFaultWord3.B.ESCBYRelayFaultA == 0) //������A����·�ű��̵ּ���δ��բ���ϡ�WY
		{
			ESCFlagA.ESCCntMs.StartDelay = 0;
			softwareFaultWord3.B.ESCBYRelayFaultA = BYRelayFaultDetect(SOE_GP_FAULT + 45, CNT_BYRelay_A, 2, ESCFlagA.PHASE); //����A����·�ű��̵ּ���δ��բ���ϡ�WY
			if (softwareFaultWord3.B.ESCBYRelayFaultA == 1)
			{
				ESCBYRelayCNTA = 1; //A����·�ű��̵ּ���״̬�쳣��WY
			}
		}
	}
	else
	{
		SetFaultDelayCounter(CNT_BYRelay_A, 0); //���㣺A����·�ű��̵ּ��������źŴ�����WY
	}

	/*B����·�ű��̵ּ���δ��բ���ϡ�WY*/
	if ((fabs(VoltInB_rms - VoltOutB_rms) > 20) && (StateEventFlag_B == STATE_EVENT_WAIT_B))
	{
		ESCFlagB.FAULTCONFIRFlag = 1;
		if (softwareFaultWord3.B.ESCBYRelayFaultB == 0)
		{
			ESCFlagB.ESCCntMs.StartDelay = 0;
			softwareFaultWord3.B.ESCBYRelayFaultB = BYRelayFaultDetect(SOE_GP_FAULT + 46, CNT_BYRelay_B, 2, ESCFlagB.PHASE);
			if (softwareFaultWord3.B.ESCBYRelayFaultB == 1)
			{
				ESCBYRelayCNTB = 1;
			}
		}
	}
	else
	{
		SetFaultDelayCounter(CNT_BYRelay_B, 0);
	}

	/*C����·�ű��̵ּ���δ��բ���ϡ�WY*/
	if ((fabs(VoltInC_rms - VoltOutC_rms) > 20) && (StateEventFlag_C == STATE_EVENT_WAIT_C))
	{
		ESCFlagC.FAULTCONFIRFlag = 1;

		if (softwareFaultWord3.B.ESCBYRelayFaultC == 0)
		{
			ESCFlagC.ESCCntMs.StartDelay = 0;
			softwareFaultWord3.B.ESCBYRelayFaultC = BYRelayFaultDetect(SOE_GP_FAULT + 47, CNT_BYRelay_C, 2, ESCFlagC.PHASE);
			if (softwareFaultWord3.B.ESCBYRelayFaultC == 1)
			{
				ESCBYRelayCNTC = 1;
			}
		}
	}
	else
	{
		SetFaultDelayCounter(CNT_BYRelay_C, 0);
	}

	/*������Ƶ���ϡ�WY*/
	if ((VoltInA_rms > 15) //A�������ѹ��Чֵ > 15��WY
			|| (VoltInB_rms > 15)  //B�������ѹ��Чֵ > 15��WY
			|| (VoltInC_rms > 15)) //C�������ѹ��Чֵ > 15��WY
	{
		if (GridFreq >= GF_OVER) //����Ƶ�� > ����Ƶ������ֵ��WY
		{
			ESCFlagA.FAULTCONFIRFlag = 1; //A����ڹ����źš�WY
			ESCFlagB.FAULTCONFIRFlag = 1; //B����ڹ����źš�WY
			ESCFlagC.FAULTCONFIRFlag = 1; //C����ڹ����źš�WY

			if (softwareFaultWord3.B.ESCGridOverFreqenceFlag == 0) //�����ڵ�����Ƶ���ϡ�WY
			{
				ESCFlagA.ESCCntMs.StartDelay = 0;
				ESCFlagB.ESCCntMs.StartDelay = 0;
				ESCFlagC.ESCCntMs.StartDelay = 0;
				softwareFaultWord3.B.ESCGridOverFreqenceFlag = THREEPHASEFaultDetect(SOE_GP_FAULT + 35, CNT_OVER_FREQ, 2000); //���ڵ�����Ƶ���ϡ�WY
			}
		}
		else
		{
			SetFaultDelayCounter(CNT_OVER_FREQ, 1); //���㣺������Ƶ�����źŴ�����WY
		}
	}

	/*����ǷƵ���ϡ�WY*/
	if ((VoltInA_rms > 15) //A�������ѹ��Чֵ > 15��WY
			|| (VoltInB_rms > 15) //B�������ѹ��Чֵ > 15��WY
			|| (VoltInC_rms > 15)) //C�������ѹ��Чֵ > 15��WY
	{
		if (GridFreq <= GF_UNDER) //����Ƶ�� < ����Ƶ������ֵ��WY
		{
			ESCFlagA.FAULTCONFIRFlag = 1; //A����ڹ����źš�WY
			ESCFlagB.FAULTCONFIRFlag = 1; //B����ڹ����źš�WY
			ESCFlagC.FAULTCONFIRFlag = 1; //C����ڹ����źš�WY

			if (softwareFaultWord3.B.ESCGridUnderFreqenceFlag == 0) //�����ڵ���ǷƵ���ϡ�WY
			{
				ESCFlagA.ESCCntMs.StartDelay = 0;
				ESCFlagB.ESCCntMs.StartDelay = 0;
				ESCFlagC.ESCCntMs.StartDelay = 0;
				softwareFaultWord3.B.ESCGridUnderFreqenceFlag = THREEPHASEFaultDetect(SOE_GP_FAULT + 36, CNT_UNDER_FREQ, 4000); //���ڵ���ǷƵ���ϡ�WY
			}
		}
		else
		{
			SetFaultDelayCounter(CNT_UNDER_FREQ, 0); //���㣺����ǷƵ�����źŴ�����WY
		}
	}

	/*�����ɢ��Ƭ���¹��ϡ�WY*/
	if (TempData[0] > WindCold.BOARD_OVER_TEMP) //�����ɢ��Ƭ�¶� > �����ɢ��Ƭ�¶����ޡ�WY
	{
		ESCFlagA.FAULTCONFIRFlag = 1; //A����ڹ����źš�WY
		ESCFlagB.FAULTCONFIRFlag = 1; //B����ڹ����źš�WY
		ESCFlagC.FAULTCONFIRFlag = 1; //C����ڹ����źš�WY

		if (softwareFaultWord2.B.ESCOverTemperatureFlagB == 0) //�����ڳ����ɢ��Ƭ���¹��ϡ�WY
		{
			ESCFlagA.ESCCntMs.StartDelay = 0;
			ESCFlagB.ESCCntMs.StartDelay = 0;
			ESCFlagC.ESCCntMs.StartDelay = 0;

			softwareFaultWord2.B.ESCOverTemperatureFlagB = THREEPHASEFaultDetect(SOE_GP_FAULT + 24, CNT_SET_OVER_TEMP_B, 20); //���ڳ����ɢ��Ƭ���¹��ϡ�WY
		}
	}
	else
	{
		SetFaultDelayCounter(CNT_SET_OVER_TEMP_B, 1); //���㣺�����ɢ��Ƭ���¹����źŴ�����WY
	}

	/*��Ԫ��ǹ��¹��ϡ�WY*/
	if (TempData[1] > WindCold.UNIT_OVER_TEMP) //��Ԫ����¶� > ��Ԫ����¶����ޡ�WY
	{
		ESCFlagA.FAULTCONFIRFlag = 1; //A����ڹ����źš�WY
		ESCFlagB.FAULTCONFIRFlag = 1; //B����ڹ����źš�WY
		ESCFlagC.FAULTCONFIRFlag = 1; //C����ڹ����źš�WY

		if (softwareFaultWord2.B.ESCChassisOverTempFlag == 0) //�����ڵ�Ԫ��ǹ��¹��ϡ�WY
		{
			ESCFlagA.ESCCntMs.StartDelay = 0;
			ESCFlagB.ESCCntMs.StartDelay = 0;
			ESCFlagC.ESCCntMs.StartDelay = 0;

			softwareFaultWord2.B.ESCChassisOverTempFlag = THREEPHASEFaultDetect(SOE_GP_FAULT + 30, CNT_SET_BOARD_OVER_TEMP, 20); //���ڵ�Ԫ��ǹ��¹��ϡ�WY
		}
	}
	else
	{
		SetFaultDelayCounter(CNT_SET_BOARD_OVER_TEMP, 1); //���㣺��Ԫ��ǹ��¹��ϡ�WY
	}

	/*���׹��ϡ�WY*/
	if (windColdCtr != 0) //δ���η��׹��ϼ�⡣WY
	{
		if (GET_FLback == 1) //���ڷ��׹��ϡ�WY
		{
			ESCFlagA.FAULTCONFIRFlag = 1; //A����ڹ����źš�WY
			ESCFlagB.FAULTCONFIRFlag = 1; //B����ڹ����źš�WY
			ESCFlagC.FAULTCONFIRFlag = 1; //C����ڹ����źš�WY

			if (softwareFaultWord2.B.ESCFangLeiFaultFlag == 0) //�����ڷ��׹��ϡ�WY
			{
				ESCFlagA.ESCCntMs.StartDelay = 0;
				ESCFlagB.ESCCntMs.StartDelay = 0;
				ESCFlagC.ESCCntMs.StartDelay = 0;

				softwareFaultWord2.B.ESCFangLeiFaultFlag = THREEPHASEFaultDetect(SOE_GP_FAULT + 26, CNT_FANGLEI_FAULT, 20); //���ڷ��׹��ϡ�WY
			}
		}
		else
		{
			SetFaultDelayCounter(CNT_FANGLEI_FAULT, 0); //���㣺���׹��ϡ�WY
		}
	}

	ZeroOffsetJudgment();// �����㷨��ִ��ʱ��Ϊ 888.0*0.006667 =5.920296

//--------------- г�� -------------------//
	if (ResonProtcABCRms > 5)
	{
		ErrorRecord.B.REC_HF = 1;
		if (CntMs.HFOverCurrentCount >= CNT_MS(60))
			StateFlag.HFOverCurrentFlag = 1;
		if (ResonProtcABCRms > 15)
			CntMs.HFOverCurrentCount += 10;
		if (ResonProtcABCRms > 25)
			CntMs.HFOverCurrentCount += 60;
		if (CntMs.HFOverCurrentCount >= CNT_MS(500))//
		{
			if (ESCFlagA.PHASE == 1)
				ESCFlagA.FAULTCONFIRFlag = 1;
			if (ESCFlagB.PHASE == 2)
				ESCFlagB.FAULTCONFIRFlag = 1;
			if (ESCFlagC.PHASE == 3)
				ESCFlagC.FAULTCONFIRFlag = 1;
#if !TEST_NULLPULSE
			if (!softwareFaultWord3.B.ESCResonanceFlautFlag)
				SoeRecData(SOE_GP_FAULT + 34);
			softwareFaultWord3.B.ESCResonanceFlautFlag = 1;
			StateEventFlag_A = STATE_EVENT_FAULT_A;//???
			StateEventFlag_B = STATE_EVENT_FAULT_B;
			StateEventFlag_C = STATE_EVENT_FAULT_C;
			ESCFlagA.faultFlag = 1;
			ESCFlagB.faultFlag = 1;
			ESCFlagC.faultFlag = 1;
#endif
		}
	}
	else
	{
		ErrorRecord.B.REC_HF = 0;
		if (CntMs.HFOverCurrentCount > 0)
		{
			CntMs.HFOverCurrentCount -= 2;//��ʱ�޻ָ�
		}
		else
		{
			CntMs.HFOverCurrentCount = 0;
			StateFlag.HFOverCurrentFlag = 0;//Pϵ���ָ�Ϊ1��ģʽ
		}
	}
}

/*
 * ���ܣ��ڸ�λ�豸ʱ����λ���ϱ�־λ��WY
 */
void FaultReset(void)
{
	if (((ESCFlagA.faultFlag == 0) //A�಻���ڹ��ϡ�WY
			|| (ESCFlagB.faultFlag == 0) //B�಻���ڹ��ϡ�WY
			|| (ESCFlagC.faultFlag == 0)) //C�಻���ڹ��ϡ�WY
			&& (StateFlag.resetFaultOnceTime == 0)) //�˱�־λ�����壬�Һ�Ϊ0��WY
	{
		softwareFaultWord1.all = 0; //��λ���ϱ�־λ��WY
		softwareFaultWord2.all = 0; //��λ���ϱ�־λ��WY
		softwareFaultWord3.all = 0; //��λ���ϱ�־λ��WY
		softwareFaultWord4.all = 0; //��λ���ϱ�־λ��WY
		softwareFaultWord5.all = 0; //��λ���ϱ�־λ��WY
	}
}

/*
 * �����⺯��
 * ��һ�ַ�����PLL������A���Ƕȣ���angleA-pi/3���ͺ���PI/3���ٸ��ݵ�ѹD�ỹԭA���ѹ
 * ����C��Ӧ�ú�A�෴����ӺͿ���Լ�����㡣
 * ������һ�£��ǵò����������ġ����Կ��������ж��������ȷ�ԡ�
 */
/*
 * �ڶ��ְ취������alpha��beta�ĽǶȣ������������Ƕ�Ϊ���ڵ�����
 * �������λ����Ƕ�Ϊ���ڵݼ���
 * ����ֻ��Ҫ�Ƚϼ���ĽǶ�ǰ������ֵ�Ĳ��ֵΪ���򣬸�ֵΪ����
 */
// ������ʱ��ֻ��������ǰ���м�⣬���������������

void GridPhaseSequTest(Uint16 i)
{
	static float theta_c;
	static float theta_c_Prior;

	float a = 0, b = 0, tmp1 = 0;
	if (StateEventFlag_A != STATE_EVENT_RUN_A)
	{
		a = GridVoltAlpha;
		b = GridVoltBeta;

		theta_c = atan2(a, b);// ------- ����rts2800_fpu32.lib    ����ʱ����಻����  4us
// ------- ���ǲ��ø�����������󣬼���ʱ����಻����   0.6us
		if ((a > 0) && (b < 0))
			theta_c = -theta_c + 4.7123889;// 3/2*pi;
		else if ((a > 0) && (b > 0))
			theta_c = -theta_c + 1.5707963;//pi/2;
		else if ((a < 0) && (b > 0))
			theta_c = -theta_c + 1.5707963;//pi/2;
		else if ((a < 0) && (b < 0))
			theta_c = -theta_c + 4.7123889;// 3/2*pi;

		if ((theta_c > 1) && (theta_c < 6))// �ܿ���������
		{
			tmp1 = theta_c - theta_c_Prior;

			if (i)
			{

			}
			else
			{
				if (tmp1 < 0)
				{
					cntGridVoltPhaseSeqFault++;
					if (cntGridVoltPhaseSeqFault > 300)
					{
						FactorySet.Infer.B.VoltagePhaseSequence = ~FactorySet.Infer.B.VoltagePhaseSequence;
						CorrectingAD();
						StateFlag.PLLSafetyFlag = 0;
						cntGridVoltPhaseSeqFault = 0;
					}
				}
				else
				{
					cntGridVoltPhaseSeqFault -= 10;
					if (cntGridVoltPhaseSeqFault <= 1)
						cntGridVoltPhaseSeqFault = 1;
				}
			}
			theta_c_Prior = theta_c;
		}
		else
		{
			theta_c_Prior = 1;
		}
	}
}
