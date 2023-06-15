/* ���ϼ���У�
 * 1��ֱ����ĸ�ߵ�ѹ��ѹ��Ƿѹ
 * 2���������˲ʱֵ����Чֵ����
 * 3��������ѹ˲ʱֵ��ѹ����Чֵ��ѹ��Ƿѹ���������ȱ��
 * 4��Ƶ���쳣����Ƶ��ǷƵ
 * 5��Ӳ�����ϣ�IGBT����������Ͻӿ�
 */
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
//#pragma	CODE_SECTION(FaultDetectInRunning ,"ram2func")
#pragma CODE_SECTION(EcapINT1 ,"ram2func")
#pragma CODE_SECTION(EcapINT2 ,"ram2func")
#pragma CODE_SECTION(EcapINT3 ,"ram2func")
#pragma CODE_SECTION(StateFeedBackJudge ,"ram2func")
// ��������ϱ�־λ faultFlag
// ���Ϻ�ͳһ����  StateEventFlag = STATE_EVENT_FAULT
// �������λ�󣬲����Զ������ֻ�ܾ�����λ���ֶ����͸�λ�ź����
// ����ʱ����9��Ϊ1����2.�����������ɺ�Ҫע��

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
void StateFeedBackJudge(void)
{
    if(GET_BYPASS_CONTACT_ACTION_A == 0){
        if(++BypassContactCntA>=TESTVALAUE){                     // �����������һ��,Ҫ��Ȼ���ܻ���ָ��ض��������
            BypassContactCntA = TESTVALAUE;
            ESCFlagA.ByPassContactFlag = 1;//A����·�ű��ֱպ�
        }
    }else{
        if(--BypassContactCntA<=0){
            BypassContactCntA = 0;
            ESCFlagA.ByPassContactFlag = 0;//A����·�ű��ֶϿ�
        }
    }
    if(GET_BYPASS_CONTACT_ACTION_B == 0){
        if(++BypassContactCntB>=TESTVALAUE){                     // �����������һ��,Ҫ��Ȼ���ܻ���ָ��ض��������
            BypassContactCntB = TESTVALAUE;
            ESCFlagB.ByPassContactFlag = 1;//B����·�ű��ֱպ�
        }
    }else{
        if(--BypassContactCntB<=0){
            BypassContactCntB = 0;
            ESCFlagB.ByPassContactFlag = 0;//B����·�ű��ֶϿ�
        }
    }
    if(GET_BYPASS_CONTACT_ACTION_C == 0){
        if(++BypassContactCntC>=TESTVALAUE){                     // �����������һ��,Ҫ��Ȼ���ܻ���ָ��ض��������
            BypassContactCntC = TESTVALAUE;
            ESCFlagC.ByPassContactFlag = 1;//C����·�ű��ֱպ�
        }
    }else{
        if(--BypassContactCntC<=0){
            BypassContactCntC = 0;
            ESCFlagC.ByPassContactFlag = 0;//C����·�ű��ֶϿ�
        }
    }

    if(GET_MAIN_CONTACT_ACTION_A == 0){
        if(++ContactorFeedBackCntA>=TESTVALAUE){
            ContactorFeedBackCntA = TESTVALAUE;
            ESCFlagA.ContactorFeedBackFlag = 1;//�Ӵ����պ�
        }
    }else{
        if(--ContactorFeedBackCntA<=0){
            ContactorFeedBackCntA = 0;
            ESCFlagA.ContactorFeedBackFlag = 0;//�Ӵ����Ͽ�
        }
    }
    if(GET_MAIN_CONTACT_ACTION_B == 0){
        if(++ContactorFeedBackCntB>=TESTVALAUE){
            ContactorFeedBackCntB = TESTVALAUE;
            ESCFlagB.ContactorFeedBackFlag = 1;//�Ӵ����պ�
        }
    }else{
        if(--ContactorFeedBackCntB<=0){
            ContactorFeedBackCntB = 0;
            ESCFlagB.ContactorFeedBackFlag = 0;//�Ӵ����Ͽ�
        }
    }
    if(GET_MAIN_CONTACT_ACTION_C == 0){
        if(++ContactorFeedBackCntC>=TESTVALAUE){
            ContactorFeedBackCntC = TESTVALAUE;
            ESCFlagC.ContactorFeedBackFlag = 1;//�Ӵ����պ�
        }
    }else{
        if(--ContactorFeedBackCntC<=0){
            ContactorFeedBackCntC = 0;
            ESCFlagC.ContactorFeedBackFlag = 0;//�Ӵ����Ͽ�
        }
    }
}

/*
 * ���ܣ�������
 * �������SoeName������������ʾ�Ĺ�������������WY
 * �������counterName�������źż�������������
 * �������FaultDelayTime�������źŴ�������ֵ��
   �����źż����� < �����źŴ�������ֵ�����������ж���
   �����źż����� > �����źŴ�������ֵ��ִ�й����ж���
 * �������ESCPHASE�����ж��������͵��ࡣ
 */
int16 FaultDetect(Uint16 SoeName,Uint16 counterName,Uint16 FaultDelayTime,Uint16 ESCPHASE)
{
	/*
	 * �����źż����� > �����źŴ�������ֵ��ִ�й����ж���WY
	 * Ŀ�ģ���ֹ��⡣
	 */
	if (CntFaultDelay[counterName]++ >= FaultDelayTime)
	{
		DINT;

		if (CTRL24_POWERFlag == 1)
		{
			EPwm1Regs.AQCSFRC.bit.CSFA = EPwmRegsAQCSFRCclear; //PWMǿ�Ƴ�������ߵ�ƽ�����ȫ�����У���WY
			CTRL24_POWERFlag = 0;
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

int16 THREEPHASEFaultDetect(Uint16 SoeName,Uint16 counterName,Uint16 FaultDelayTime)
{
    if(CntFaultDelay[counterName]++ >= FaultDelayTime) //��Ԫ��Ӹ���̫�������
    {
        DINT;
        if(CTRL24_POWERFlag == 1){
            EPwm1Regs.AQCSFRC.bit.CSFA = EPwmRegsAQCSFRCclear;  //0:ȥ�����ǿ��(ռ�ձȿ���ת��);1:���ǿ���õ�(ȫ��ת��);2:���ǿ���ø�(��ת);
            CTRL24_POWERFlag = 0;
        }
        if((StateEventFlag_A == STATE_EVENT_WAIT_A)&&\
                (StateEventFlag_B == STATE_EVENT_WAIT_B)&&\
                (StateEventFlag_C == STATE_EVENT_WAIT_C))
        {
            SET_POWER_CTRL(1);
            Delayus(TIME_WRITE_15VOLT_REDAY);  //���ڿ���Ӳ������������Ӳ����·�м�����PWMEN/��բ��ʹ�ܹ���������,���Բ��ܼ���ʱ;���򵱵���ֵС�ڼ�����ʱ,ʹ�ܻ����¸���,��ʱ��������ܼ�ʱ�Ĺص�ʹ���ź�,��ը��.
        }
        if((ESCFlagA.FAULTCONFIRFlag == 1)&&(ESCBYRelayCNTA != 1)){     //ȷ������һ�෢������,���й����߼��л�
//            ESCFlagA.PWMcurrDirFlag = 0;
//            ESCFlagA.FaultJudgeFlag = 1;
            StateEventFlag_A = STATE_EVENT_FAULT_A;
            ESCFlagA.faultFlag = 1;
            FirstFaultStop(ESCFlagA.PHASE);
            ESCFlagA.ESCCntMs.CutCurrDelay = 0;
        }
        if((ESCFlagB.FAULTCONFIRFlag == 1)&&(ESCBYRelayCNTB != 1)){
//            ESCFlagB.PWMcurrDirFlag = 0;
//            ESCFlagB.FaultJudgeFlag = 1;
            StateEventFlag_B = STATE_EVENT_FAULT_B;
            ESCFlagB.faultFlag = 1;
            FirstFaultStop(ESCFlagB.PHASE);
            ESCFlagB.ESCCntMs.CutCurrDelay = 0;
        }
        if((ESCFlagC.FAULTCONFIRFlag == 1)&&(ESCBYRelayCNTC != 1)){
//            ESCFlagC.PWMcurrDirFlag = 0;
//            ESCFlagC.FaultJudgeFlag = 1;
            StateEventFlag_C = STATE_EVENT_FAULT_C;
            ESCFlagC.faultFlag = 1;
            FirstFaultStop(ESCFlagC.PHASE);
            ESCFlagC.ESCCntMs.CutCurrDelay = 0;
        }
        EINT;
        SoeRecData(SoeName);
        WriteFlashConfig(SoeName);
        CntFaultDelay[counterName]=0;
        return 1;
    }else{
        ESCFlagA.ESCCntSec.HWPowerFaultDelay = 0;
        ESCFlagA.ESCCntSec.HWPowerStopDelay =0;
        ESCFlagB.ESCCntSec.HWPowerFaultDelay = 0;
        ESCFlagB.ESCCntSec.HWPowerStopDelay =0;
        ESCFlagC.ESCCntSec.HWPowerFaultDelay = 0;
        ESCFlagC.ESCCntSec.HWPowerStopDelay =0;

        return 0;
    }
}

int16 PowerFailFaultDetect(Uint16 SoeName,Uint16 counterName,Uint16 FaultDelayTime)
{
    if(CntFaultDelay[counterName]++ >= FaultDelayTime) //��Ԫ��Ӹ���̫�������
    {
        DINT;
        if(CTRL24_POWERFlag == 1){
            EPwm1Regs.AQCSFRC.bit.CSFA = EPwmRegsAQCSFRCclear;  //0:ȥ�����ǿ��(ռ�ձȿ���ת��);1:���ǿ���õ�(ȫ��ת��);2:���ǿ���ø�(��ת);
            CTRL24_POWERFlag = 0;
        }
            SET_POWER_CTRL(1);

        if((ESCFlagA.FAULTCONFIRFlag == 1)&&(ESCBYRelayCNTA != 1)){     //ȷ������һ�෢������,���й����߼��л�
//            ESCFlagA.PWMcurrDirFlag = 0;
//            ESCFlagA.FaultJudgeFlag = 1;
            StateEventFlag_A = STATE_EVENT_FAULT_A;
            ESCFlagA.faultFlag = 1;
            FirstFaultStop(ESCFlagA.PHASE);
            ESCFlagA.ESCCntMs.CutCurrDelay = 0;
        }
        if((ESCFlagB.FAULTCONFIRFlag == 1)&&(ESCBYRelayCNTB != 1)){
//            ESCFlagB.PWMcurrDirFlag = 0;
//            ESCFlagB.FaultJudgeFlag = 1;
            StateEventFlag_B = STATE_EVENT_FAULT_B;
            ESCFlagB.faultFlag = 1;
            FirstFaultStop(ESCFlagB.PHASE);
            ESCFlagB.ESCCntMs.CutCurrDelay = 0;
        }
        if((ESCFlagC.FAULTCONFIRFlag == 1)&&(ESCBYRelayCNTC != 1)){
//            ESCFlagC.PWMcurrDirFlag = 0;
//            ESCFlagC.FaultJudgeFlag = 1;
            StateEventFlag_C = STATE_EVENT_FAULT_C;
            ESCFlagC.faultFlag = 1;
            FirstFaultStop(ESCFlagC.PHASE);
            ESCFlagC.ESCCntMs.CutCurrDelay = 0;
        }
        EINT;
        SoeRecData(SoeName);
        WriteFlashConfig(SoeName);
        CntFaultDelay[counterName]=0;
        return 1;
    }else{
        ESCFlagA.ESCCntSec.HWPowerFaultDelay = 0;
        ESCFlagA.ESCCntSec.HWPowerStopDelay =0;
        ESCFlagB.ESCCntSec.HWPowerFaultDelay = 0;
        ESCFlagB.ESCCntSec.HWPowerStopDelay =0;
        ESCFlagC.ESCCntSec.HWPowerFaultDelay = 0;
        ESCFlagC.ESCCntSec.HWPowerStopDelay =0;

        return 0;
    }
}


int16 BYRelayFaultDetect(Uint16 SoeName,Uint16 counterName,Uint16 FaultDelayTime,Uint16 ESCPHASE)
{
    if(CntFaultDelay[counterName]++ >= FaultDelayTime) //��Ԫ��Ӹ���̫�������
    {
        DINT;
        if(CTRL24_POWERFlag == 1){
            EPwm1Regs.AQCSFRC.bit.CSFA = EPwmRegsAQCSFRCclear;  //0:ȥ�����ǿ��(ռ�ձȿ���ת��);1:���ǿ���õ�(ȫ��ת��);2:���ǿ���ø�(��ת);
            CTRL24_POWERFlag = 0;
        }
            SET_POWER_CTRL(1);
            Delayus(TIME_WRITE_15VOLT_REDAY);
        if((ESCFlagA.FAULTCONFIRFlag == 1)&&(ESCPHASE == 1)){     //ȷ������һ�෢������,���й����߼��л�
//          ESCFlagA.PWMcurrDirFlag = 0;
//          ESCFlagA.FaultJudgeFlag = 1;
            StateEventFlag_A = STATE_EVENT_FAULT_A;
            ESCFlagA.faultFlag = 1;
            ESCFlagA.PWM_ins_index = 1;
            ESCFlagA.ESC_DutyData = 1.0;
            SET_SCRA_ENABLE(ESC_ACTION_ENABLE);                 //����բ��
            SET_MAIN_CONTACT_ACTION_A(ESC_ACTION_ENABLE);
            SET_BYPASS_CONTACT_ACTION_A(ESC_ACTION_ENABLE);     //����·
            Delayus(TIME_WRITE_15VOLT_REDAY);
            EnablePWMA();
            Delayus(100);                                                      //��Ҫ�Ӹ���ʱʱ��,����PWM�ļĴ���������������
            SET_IGBT_ENA(IGBT_ENABLE);                                         //��IGBTʹ��
            EPwm3Regs.CMPA.bit.CMPA = T1PR;     //1��ֱͨ      //����Ӳ���߼�����,CMPA,CMPB��������ֵ,�������ĸ�����ȫͨ.
            EPwm3Regs.CMPB.bit.CMPB = T1PR;     //2�ܲ�ֱͨ
            EPwm4Regs.CMPA.bit.CMPA = T1PR;     //3��ֱͨ
            EPwm4Regs.CMPB.bit.CMPB = T1PR;     //4�ܲ�ֱͨ
            ESCFlagA.FaultskipFlag = 1;
            ESCFlagA.FAULTCONFIRFlag = 0;
            ESCFlagA.ESCCntMs.StartDelay = 0;
        }
        if((ESCFlagB.FAULTCONFIRFlag == 1)&&(ESCPHASE == 2)){
//          ESCFlagB.PWMcurrDirFlag = 0;
//          ESCFlagB.FaultJudgeFlag = 1;
            StateEventFlag_B = STATE_EVENT_FAULT_B;
            ESCFlagB.faultFlag = 1;
            ESCFlagB.PWM_ins_index = 1;
            ESCFlagB.ESC_DutyData = 1.0;
            SET_SCRB_ENABLE(ESC_ACTION_ENABLE);                 //����բ��
            SET_MAIN_CONTACT_ACTION_B(ESC_ACTION_ENABLE);
            SET_BYPASS_CONTACT_ACTION_B(ESC_ACTION_ENABLE);     //����·
            Delayus(TIME_WRITE_15VOLT_REDAY);
            EnablePWMB();
            Delayus(100);                                                      //��Ҫ�Ӹ���ʱʱ��,����PWM�ļĴ���������������
            SET_IGBT_ENB(IGBT_ENABLE);                                         //��IGBTʹ��
            EPwm5Regs.CMPA.bit.CMPA = T1PR;     //1��ֱͨ  //����Ӳ���߼�����,CMPA,CMPB��������ֵ,�������ĸ�����ȫͨ.
            EPwm5Regs.CMPB.bit.CMPB = T1PR;     //2�ܲ�ֱͨ
            EPwm6Regs.CMPA.bit.CMPA = T1PR;     //3��ֱͨ
            EPwm6Regs.CMPB.bit.CMPB = T1PR;     //4�ܲ�ֱͨ
            ESCFlagB.FaultskipFlag = 1;
            ESCFlagB.FAULTCONFIRFlag = 0;
            ESCFlagB.ESCCntMs.StartDelay = 0;
        }
        if((ESCFlagC.FAULTCONFIRFlag == 1)&&(ESCPHASE == 3)){
//          ESCFlagC.PWMcurrDirFlag = 0;
//          ESCFlagC.FaultJudgeFlag = 1;
            StateEventFlag_C = STATE_EVENT_FAULT_C;
            ESCFlagC.faultFlag = 1;
            ESCFlagC.PWM_ins_index = 1;
            ESCFlagC.ESC_DutyData = 1.0;
            SET_SCRC_ENABLE(ESC_ACTION_ENABLE);                 //����բ��
            SET_MAIN_CONTACT_ACTION_C(ESC_ACTION_ENABLE);
            SET_BYPASS_CONTACT_ACTION_C(ESC_ACTION_ENABLE);     //����·
            Delayus(TIME_WRITE_15VOLT_REDAY);
            EnablePWMC();
            Delayus(100);                                                      //��Ҫ�Ӹ���ʱʱ��,����PWM�ļĴ���������������
            SET_IGBT_ENC(IGBT_ENABLE);                                         //��IGBTʹ��
            EPwm7Regs.CMPA.bit.CMPA = T1PR;     //1��ֱͨ  //����Ӳ���߼�����,CMPA,CMPB��������ֵ,�������ĸ�����ȫͨ.
            EPwm7Regs.CMPB.bit.CMPB = T1PR;     //2�ܲ�ֱͨ
            EPwm8Regs.CMPA.bit.CMPA = T1PR;     //3��ֱͨ
            EPwm8Regs.CMPB.bit.CMPB = T1PR;     //4�ܲ�ֱͨ
            ESCFlagC.FaultskipFlag = 1;
            ESCFlagC.FAULTCONFIRFlag = 0;
            ESCFlagC.ESCCntMs.StartDelay = 0;
        }
        EINT;
        SoeRecData(SoeName);
        WriteFlashConfig(SoeName);
        CntFaultDelay[counterName]=0;
        return 1;
    }else{
        ESCFlagA.ESCCntSec.HWPowerFaultDelay = 0;
        ESCFlagA.ESCCntSec.HWPowerStopDelay =0;
        ESCFlagB.ESCCntSec.HWPowerFaultDelay = 0;
        ESCFlagB.ESCCntSec.HWPowerStopDelay =0;
        ESCFlagC.ESCCntSec.HWPowerFaultDelay = 0;
        ESCFlagC.ESCCntSec.HWPowerStopDelay =0;

        return 0;
    }
}

//�������Ƕ������͵�,�����ж�,����ִ��,����SOE
//SoeName:SOE�Ĺ�����,������
//counterName:SOE��ͨ����,����������SOE��������ͬ.
//FaultDelayTime:���ϳ���ʱ���趨.����������ֵ>=�趨ֵ,�������Ӧ������,����ִ��,����SOE
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

//�������Ǹ������͵�,�����ж�,����ִ��,����SOE
//SoeName:SOE�Ĺ�����,������
//counterName:SOE��ͨ����
//FaultDelayTime:���ϳ���ʱ���趨.����������ֵ>�趨ֵ,�������Ӧ������,����ִ��,����SOE
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

//���������ֵ
//counterName:ͨ����.
//mode:���ģʽ 0:ֱ������ ����:������-=modeֵ
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
//����������͵ļ�����ֵ
//counterName:ͨ����.
//mode:���ģʽ 0:ֱ������
void SetFaultDelayCounterLong(Uint16 counterName,Uint32 mode)
{
	if(CntFaultDelayLong[counterName] > mode)
		CntFaultDelayLong[counterName] -= mode;
	else
		CntFaultDelayLong[counterName] = 0;
}

void FaultDetectInInt(void)                  // �������жϣ���������,ÿ��AD�ж�����һ��,�������λ��.�����IGBTFAULT_DLYTIME�Ķ���
{}
  /********************* Ecap�ж�  ******************************/
void EcapINT1(void)
{
    //--------------- ESC ����Ӳ����������A--------------//
//    TIMER1 = ECap1Regs.CAP1;
//    TIMER2 = ECap1Regs.CAP2;
//    TIMER3 = ECap1Regs.CAP3;
//    TIMER4 = ECap1Regs.CAP4;
//        if(StateEventFlag == STATE_EVENT_RUN){
//            if(ECap1Regs.ECFLG.bit.CEVT2 != 0){
//                if(TIMER2 > TIMER1){
//                    TIMERPERIODVAL1 = TIMER2 - TIMER1;
//                }else{
//                    TIMERPERIODVAL1 = (0xFFFFFFFF - TIMER1) + TIMER2;   //ע������˳��,��ֹ���!
//                }
//                if((ECap1Regs.ECFLG.bit.CEVT3 != 0)&&(ECap1Regs.ECFLG.bit.CEVT4 == 0)){
//                    CAPTIEMRFlag = 1;
//                }else if((ECap1Regs.ECFLG.bit.CEVT3 != 0)&&(ECap1Regs.ECFLG.bit.CEVT4 != 0)){
//                    if(TIMER4 > TIMER3){
//                        TIMERPERIODVAL2 = TIMER4 - TIMER3;
//                    }else{
//                        TIMERPERIODVAL2 = (0xFFFFFFFF - TIMER3) + TIMER4;   //ע������˳��,��ֹ���!
//                    }
//                    if((TIMERPERIODVAL1 > 100*CAPPERIODVAL)||(TIMERPERIODVAL2 > 100*CAPPERIODVAL))   CAPTIEMRFlag = 1;
//                    else                      CAPTIEMRFlag = 0;
//                }else{
//                    if(TIMERPERIODVAL1 > 100*CAPPERIODVAL)   CAPTIEMRFlag = 1;
//                    else                      CAPTIEMRFlag = 0;
//                }`
//            }else{
//                CAPTIEMRFlag = 1;
//            }
//            if((softwareFaultWord1.B.hardwareOverCurrentFlag == 0)/*&&(CAPTIEMRFlag != 0)*/){
//                CAPTIEMRFlag = 0;
//                SET_IGBT_EN1(IGBT_FAULT);                           //�ر�IGBT��ʹ���ź����  IO�ź�����
//            softwareFaultWord1.B.hardwareOverCurrentFlag = FaultDetect(SOE_GP_FAULT+9,CNT_HW_OVER_CUR,0);
//        }
//    }
    if(StateEventFlag_A == STATE_EVENT_RUN_A){
        ESCFlagA.FAULTCONFIRFlag = 1;
        if((softwareFaultWord1.B.ESCFastHardwareOverCurFlagA == 0)/*&&(CAPTIEMRFlag != 0)*/){
            ESCFlagA.ESCCntMs.StartDelay = 0;
            softwareFaultWord1.B.ESCFastHardwareOverCurFlagA = FaultDetect(SOE_GP_FAULT+1,CNT_HW_OVER_CUR_A,0,ESCFlagA.PHASE);
        }
    }else{
        SetFaultDelayCounter(CNT_HW_OVER_CUR_A,0);
    }
//   ECap1Regs.ECCTL2.bit.REARM = 1;
    ECap1Regs.ECCLR.bit.CEVT4 = 1;
    ECap1Regs.ECCLR.bit.CEVT3 = 1;
    ECap1Regs.ECCLR.bit.CEVT2 = 1;
    ECap1Regs.ECCLR.bit.CEVT1 = 1;
    ECap1Regs.ECCLR.bit.INT = 1;
//    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;
//    ECap1Regs.ECCLR.all=0xFFFF;                //clare all flag
}
void EcapINT2(void)
{
    //--------------- ESC ����Ӳ����������B--------------//
    if(StateEventFlag_B == STATE_EVENT_RUN_B){
        ESCFlagB.FAULTCONFIRFlag = 1;
        if((softwareFaultWord1.B.ESCFastHardwareOverCurFlagB == 0)/*&&(CAPTIEMRFlag != 0)*/){
            ESCFlagB.ESCCntMs.StartDelay = 0;
            softwareFaultWord1.B.ESCFastHardwareOverCurFlagB = FaultDetect(SOE_GP_FAULT+2,CNT_HW_OVER_CUR_B,0,ESCFlagB.PHASE);
        }
    }else{
        SetFaultDelayCounter(CNT_HW_OVER_CUR_B,0);
    }
    ECap2Regs.ECCLR.bit.CEVT4 = 1;
    ECap2Regs.ECCLR.bit.CEVT3 = 1;
    ECap2Regs.ECCLR.bit.CEVT2 = 1;
    ECap2Regs.ECCLR.bit.CEVT1 = 1;
    ECap2Regs.ECCLR.bit.INT = 1;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;
}
void EcapINT3(void)
{
    //--------------- ESC ����Ӳ����������C--------------//
    if(StateEventFlag_C == STATE_EVENT_RUN_C){
        ESCFlagC.FAULTCONFIRFlag = 1;
        if((softwareFaultWord1.B.ESCFastHardwareOverCurFlagC == 0)/*&&(CAPTIEMRFlag != 0)*/){
            ESCFlagC.ESCCntMs.StartDelay = 0;
            softwareFaultWord1.B.ESCFastHardwareOverCurFlagC = FaultDetect(SOE_GP_FAULT+3,CNT_HW_OVER_CUR_C,0,ESCFlagC.PHASE);
        }
    }else{
        SetFaultDelayCounter(CNT_HW_OVER_CUR_C,0);
    }
    ECap3Regs.ECCLR.bit.CEVT4 = 1;
    ECap3Regs.ECCLR.bit.CEVT3 = 1;
    ECap3Regs.ECCLR.bit.CEVT2 = 1;
    ECap3Regs.ECCLR.bit.CEVT1 = 1;
    ECap3Regs.ECCLR.bit.INT = 1;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;
}

/*
 * ���ܣ����ٹ��ϼ�⡣WY
 * ˵�����ú�����ADC-1���жϷ������б����á�WY
 */
void FaultFastDetectInInt(void)                  // �������жϣ����ٱ���
{
//���ڸù����ж��Ƿ����ж���,���ϵ������ƫУ׼��ʱ��ϳ�,����Ҫ����״̬�ж�,��ֹ����ƫУ׼ʱ,����ù���,ʹ״̬λESCFlagA.FAULTCONFIRFlag��1;
	if (PhaseControl == 1)
	{
		if (((GridCurrAF > OUTCUR_OVER_INS) || (GridBPCurrAF > OUTCUR_OVER_INS)) && (StateEventFlag_A != STATE_EVENT_STANDBY_A))
		{
			ESCFlagA.FAULTCONFIRFlag = 1;
			if (softwareFaultWord1.B.ESCInsOverCurFlagA == 0)
			{
				ESCFlagA.ESCCntMs.StartDelay = 0;
				softwareFaultWord1.B.ESCInsOverCurFlagA = FaultDetect(SOE_GP_FAULT + 4, CNT_INS_OVER_CUR_A, 3, ESCFlagA.PHASE);
			}
		}
		else
		{
			SetFaultDelayCounter(CNT_INS_OVER_CUR_A, 0);
		}
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
		if (((GridCurrAF < OUTCUR_OVER_INS_NEG) || (GridBPCurrAF < OUTCUR_OVER_INS_NEG)) && (StateEventFlag_A != STATE_EVENT_STANDBY_A))
		{
			ESCFlagA.FAULTCONFIRFlag = 1;
			if (softwareFaultWord1.B.ESCInsOverCurFlagA == 0)
			{
				ESCFlagA.ESCCntMs.StartDelay = 0;
				softwareFaultWord1.B.ESCInsOverCurFlagA = FaultDetect(SOE_GP_FAULT + 4, CNT_INS_OVER_CUR_A, 3, ESCFlagA.PHASE);
			}
		}
		else
		{
			SetFaultDelayCounter(CNT_INS_OVER_CUR_A, 0);
		}
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
//--------------- ESC˲ʱֵ�������A ------------------//
		if (((GridCurrAF > OUTCUR_OVER_INS) || (GridCurrAF < OUTCUR_OVER_INS_NEG) || (GridBPCurrAF > OUTCUR_OVER_INS) || (GridBPCurrAF < OUTCUR_OVER_INS_NEG))
				&& (StateEventFlag_A != STATE_EVENT_STANDBY_A))
		{
			ESCFlagA.FAULTCONFIRFlag = 1;
			if (softwareFaultWord1.B.ESCInsOverCurFlagA == 0)
			{
				ESCFlagA.ESCCntMs.StartDelay = 0;
				softwareFaultWord1.B.ESCInsOverCurFlagA = FaultDetect(SOE_GP_FAULT + 4, CNT_INS_OVER_CUR_A, 3, ESCFlagA.PHASE);
			}
		}
		else
		{
			SetFaultDelayCounter(CNT_INS_OVER_CUR_A, 0);
		}
//--------------- ESC˲ʱֵ�������B ------------------//
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
//--------------- ESC˲ʱֵ�������C ------------------//
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
//------------------ֱ�����ݵ�ѹ��ѹ����A-----------------//
	if (DccapVoltA > ESCDCVOLITLIMIT)
	{
		ESCFlagA.FAULTCONFIRFlag = 1;
		if (softwareFaultWord1.B.ESCDcCapVoltOverVoltFlagA == 0)
		{
			ESCFlagA.ESCCntMs.StartDelay = 0;
			softwareFaultWord1.B.ESCDcCapVoltOverVoltFlagA = FaultDetect(SOE_GP_FAULT + 7, CNT_DC_NEUTRAL_OVER_VOLT_A, 10, ESCFlagA.PHASE);
		}
	}
	else
	{
		SetFaultDelayCounter(CNT_DC_NEUTRAL_OVER_VOLT_A, 0);
	}
//------------------ֱ�����ݵ�ѹ��ѹ����B-----------------//
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
//------------------ֱ�����ݵ�ѹ��ѹ����C-----------------//
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

//--------------- ������ѹ��Чֵ��ѹ����λA--------------//
	if (((VoltOutA_rms >= GV_RMS_OVER) || (VoltInA_rms >= GV_RMS_OVER)) && (StateEventFlag_A != STATE_EVENT_STANDBY_A))
	{
		ESCFlagA.FAULTCONFIRFlag = 1;
		if (softwareFaultWord1.B.ESCGridRMSOverVoltFlagA == 0)
		{
			ESCFlagA.ESCCntMs.StartDelay = 0;
			softwareFaultWord1.B.ESCGridRMSOverVoltFlagA = FaultDetect(SOE_GP_FAULT + 10, CNT_RMS_OVER_VOLT_A, 20, ESCFlagA.PHASE);
		}
	}
	else
	{
		SetFaultDelayCounter(CNT_RMS_OVER_VOLT_A, 0);
	}
//--------------- ������ѹ��Чֵ��ѹ����λB--------------//
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
//--------------- ������ѹ��Чֵ��ѹ����λC--------------//
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

//---------------15V������------------//
//TempEnvirProvalue==1ʱ,��������ʵ��,��Ҫ���ȫ��ȫ��ת,���ǿ��ص�Դ��������,�ᱨ�������,������������ʵ���ʱ����Ҫ�Ѹù������ε�.
	if (TempEnvirProvalue != 1)
	{
		if (GET_CTRL24_POWER == 1)
		{
			CTRL24_POWERFlag = 1;
			ESCFlagA.FAULTCONFIRFlag = 1;
			ESCFlagB.FAULTCONFIRFlag = 1;
			ESCFlagC.FAULTCONFIRFlag = 1;
			if (softwareFaultWord1.B.ESCPowerFailDetectFlag == 0)
			{
				ESCFlagA.ESCCntMs.StartDelay = 0;
				ESCFlagB.ESCCntMs.StartDelay = 0;
				ESCFlagC.ESCCntMs.StartDelay = 0;
				softwareFaultWord1.B.ESCPowerFailDetectFlag = PowerFailFaultDetect(SOE_GP_FAULT + 13, CNT_CTRL_POWER_OFF, 10);
			}
		}
		else
		{
			SetFaultDelayCounter(CNT_CTRL_POWER_OFF, 0);
		}
	}
}

void FaultDetectInMainLoop(void)
{
    //--------------- ������ѹ��ЧֵǷѹ����λA--------------//
    if(  ((VoltInA_rms <= GV_RMS_UNDER)||(VoltOutA_rms <= GV_RMS_UNDER)) &&\
            ((StateEventFlag_A == STATE_EVENT_RUN_A)||(StateEventFlag_A == STATE_EVENT_WAIT_A))  )
    {
        ESCFlagA.FAULTCONFIRFlag = 1;
        if(!softwareFaultWord1.B.ESCGridRMSUnderVoltFlagA){
            ESCFlagA.ESCCntMs.StartDelay = 0;
            softwareFaultWord1.B.ESCGridRMSUnderVoltFlagA = FaultDetect(SOE_GP_FAULT+14,CNT_RMS_UNDER_VOLT_A,20,ESCFlagA.PHASE);
        }
    }else{
        SetFaultDelayCounter(CNT_RMS_UNDER_VOLT_A,0);
    }
    //--------------- ������ѹ��ЧֵǷѹ����λB--------------//
    if(  ((VoltInB_rms <= GV_RMS_UNDER)||(VoltOutB_rms <= GV_RMS_UNDER)) &&\
            ((StateEventFlag_B == STATE_EVENT_RUN_B)||(StateEventFlag_B == STATE_EVENT_WAIT_B))  )
    {
        ESCFlagB.FAULTCONFIRFlag = 1;
        if(!softwareFaultWord1.B.ESCGridRMSUnderVoltFlagB){
            ESCFlagB.ESCCntMs.StartDelay = 0;
            softwareFaultWord1.B.ESCGridRMSUnderVoltFlagB = FaultDetect(SOE_GP_FAULT+15,CNT_RMS_UNDER_VOLT_B,20,ESCFlagB.PHASE);
        }
    }else{
        SetFaultDelayCounter(CNT_RMS_UNDER_VOLT_B,0);
    }
    //--------------- ������ѹ��ЧֵǷѹ����λC--------------//
    if(  ((VoltInC_rms <= GV_RMS_UNDER)||(VoltOutC_rms <= GV_RMS_UNDER)) &&\
            ((StateEventFlag_C == STATE_EVENT_RUN_C)||(StateEventFlag_C == STATE_EVENT_WAIT_C))  )
    {
        ESCFlagC.FAULTCONFIRFlag = 1;
        if(!softwareFaultWord1.B.ESCGridRMSUnderVoltFlagC){
            ESCFlagC.ESCCntMs.StartDelay = 0;
            softwareFaultWord1.B.ESCGridRMSUnderVoltFlagC = FaultDetect(SOE_GP_FAULT+16,CNT_RMS_UNDER_VOLT_C,20,ESCFlagC.PHASE);
        }
    }else{
        SetFaultDelayCounter(CNT_RMS_UNDER_VOLT_C,0);
    }
   //--------------- ESC������� A------------------------//   //һ����  78A
	if((gridCurA_rms > OUT_OVER_LOAD)||(gridCurrBYAF_rms > OUT_OVER_LOAD))
	{
        ESCFlagA.FAULTCONFIRFlag = 1;
		if(softwareFaultWord2.B.ESCOverLoadFlagA == 0){
		    ESCFlagA.ESCCntMs.StartDelay = 0;
		    softwareFaultWord2.B.ESCOverLoadFlagA = FaultDetect(SOE_GP_FAULT+17,LCNT_LOAD_OVER_CUR_A,1510,ESCFlagA.PHASE);
		}
	}else{
	    SetFaultDelayCounter(LCNT_LOAD_OVER_CUR_A,2);
	}
   //--------------- ESC������� B------------------------//
    if((gridCurB_rms > OUT_OVER_LOAD)||(gridCurrBYBF_rms > OUT_OVER_LOAD))
    {
        ESCFlagB.FAULTCONFIRFlag = 1;
        if(softwareFaultWord2.B.ESCOverLoadFlagB == 0){
            ESCFlagB.ESCCntMs.StartDelay = 0;
            softwareFaultWord2.B.ESCOverLoadFlagB = FaultDetect(SOE_GP_FAULT+18,LCNT_LOAD_OVER_CUR_B,1510,ESCFlagB.PHASE);
        }
    }else{
        SetFaultDelayCounter(LCNT_LOAD_OVER_CUR_B,2);
    }
    //--------------- ESC������� C------------------------//
     if((gridCurC_rms > OUT_OVER_LOAD)||(gridCurrBYCF_rms > OUT_OVER_LOAD))
     {
         ESCFlagC.FAULTCONFIRFlag = 1;
         if(softwareFaultWord2.B.ESCOverLoadFlagC == 0){
             ESCFlagC.ESCCntMs.StartDelay = 0;
             softwareFaultWord2.B.ESCOverLoadFlagC = FaultDetect(SOE_GP_FAULT+19,LCNT_LOAD_OVER_CUR_C,1510,ESCFlagC.PHASE);
         }
     }else{
         SetFaultDelayCounter(LCNT_LOAD_OVER_CUR_C,2);
     }

    //--------------- ESC��Чֵ�������A -------------------////60����   91A
    if( (gridCurA_rms > OUTCUR_OVER_RMS)||(gridCurrBYAF_rms > OUTCUR_OVER_RMS) )
	{
        ESCFlagA.FAULTCONFIRFlag = 1;
		if(softwareFaultWord2.B.ESCRmsOverCurrentFlagA == 0){
		    ESCFlagA.ESCCntMs.StartDelay = 0;
			softwareFaultWord2.B.ESCRmsOverCurrentFlagA = FaultDetect(SOE_GP_FAULT+20,CNT_RMS_OVER_CUR_A,2,ESCFlagA.PHASE);
		}
	}else{
		SetFaultDelayCounter(CNT_RMS_OVER_CUR_A,0);
	}
    //--------------- ESC��Чֵ�������B -------------------//
    if( (gridCurB_rms > OUTCUR_OVER_RMS)||(gridCurrBYBF_rms > OUTCUR_OVER_RMS) )
    {
        ESCFlagB.FAULTCONFIRFlag = 1;
        if(softwareFaultWord2.B.ESCRmsOverCurrentFlagB == 0){
            ESCFlagB.ESCCntMs.StartDelay = 0;
            softwareFaultWord2.B.ESCRmsOverCurrentFlagB = FaultDetect(SOE_GP_FAULT+21,CNT_RMS_OVER_CUR_B,2,ESCFlagB.PHASE);
        }
    }else{
        SetFaultDelayCounter(CNT_RMS_OVER_CUR_B,1);
    }
    //--------------- ESC��Чֵ�������C -------------------//
    if( (gridCurC_rms > OUTCUR_OVER_RMS)||(gridCurrBYCF_rms > OUTCUR_OVER_RMS) )
    {
        ESCFlagC.FAULTCONFIRFlag = 1;
        if(softwareFaultWord2.B.ESCRmsOverCurrentFlagC == 0){
            ESCFlagC.ESCCntMs.StartDelay = 0;
            softwareFaultWord2.B.ESCRmsOverCurrentFlagC = FaultDetect(SOE_GP_FAULT+22,CNT_RMS_OVER_CUR_C,2,ESCFlagC.PHASE);
        }
    }else{
        SetFaultDelayCounter(CNT_RMS_OVER_CUR_C,1);
    }

    //----------------ESC A����·�ű��̵ּ���δ��բ-----------------//
    if((fabs(VoltInA_rms-VoltOutA_rms) > 20)&&(StateEventFlag_A == STATE_EVENT_WAIT_A))
    {
        ESCFlagA.FAULTCONFIRFlag = 1;
        if(softwareFaultWord3.B.ESCBYRelayFaultA == 0){
            ESCFlagA.ESCCntMs.StartDelay = 0;
            softwareFaultWord3.B.ESCBYRelayFaultA = BYRelayFaultDetect(SOE_GP_FAULT+45,CNT_BYRelay_A,2,ESCFlagA.PHASE);
            if(softwareFaultWord3.B.ESCBYRelayFaultA == 1)
            {
                ESCBYRelayCNTA = 1;
            }
        }
    }else{
        SetFaultDelayCounter(CNT_BYRelay_A,0);
    }
//    if((ESCBYRelayCNTA == 1)&&(fabs(gridCurA_rms-gridCurrBYAF_rms)>10)){   //�жϸ�ѹ�˴ű��̵ּ�����
//        DisablePWMA();
//        SET_IGBT_ENA(IGBT_DISABLE);
//        SET_MAIN_CONTACT_ACTION_A(ESC_ACTION_DISABLE);
//    }

    //----------------ESC B����·�ű��̵ּ���δ��բ-----------------//
    if((fabs(VoltInB_rms-VoltOutB_rms) > 20)&&(StateEventFlag_B == STATE_EVENT_WAIT_B))
    {
        ESCFlagB.FAULTCONFIRFlag = 1;
        if(softwareFaultWord3.B.ESCBYRelayFaultB == 0){
            ESCFlagB.ESCCntMs.StartDelay = 0;
            softwareFaultWord3.B.ESCBYRelayFaultB = BYRelayFaultDetect(SOE_GP_FAULT+46,CNT_BYRelay_B,2,ESCFlagB.PHASE);
            if(softwareFaultWord3.B.ESCBYRelayFaultB == 1)
            {
                ESCBYRelayCNTB = 1;
            }
        }
    }else{
        SetFaultDelayCounter(CNT_BYRelay_B,0);
    }
//    if((ESCBYRelayCNTB == 1)&&(fabs(gridCurB_rms-gridCurrBYBF_rms)>20)){   //�жϸ�ѹ�˴ű��̵ּ�����
//        DisablePWMB();
//        SET_IGBT_ENB(IGBT_DISABLE);
//        SET_MAIN_CONTACT_ACTION_B(ESC_ACTION_DISABLE);
//    }
    //----------------ESC C����·�ű��̵ּ���δ��բ-----------------//
    if((fabs(VoltInC_rms-VoltOutC_rms) > 20)&&(StateEventFlag_C == STATE_EVENT_WAIT_C))
    {
        ESCFlagC.FAULTCONFIRFlag = 1;
        if(softwareFaultWord3.B.ESCBYRelayFaultC == 0){
            ESCFlagC.ESCCntMs.StartDelay = 0;
            softwareFaultWord3.B.ESCBYRelayFaultC = BYRelayFaultDetect(SOE_GP_FAULT+47,CNT_BYRelay_C,2,ESCFlagC.PHASE);
            if(softwareFaultWord3.B.ESCBYRelayFaultC == 1)
            {
                ESCBYRelayCNTC = 1;
            }
        }
    }else{
        SetFaultDelayCounter(CNT_BYRelay_C,0);
    }
//    if((ESCBYRelayCNTC == 1)&&(fabs(gridCurC_rms-gridCurrBYCF_rms)>20)){   //�жϸ�ѹ�˴ű��̵ּ�����
//        DisablePWMC();
//        SET_IGBT_ENC(IGBT_DISABLE);
//        SET_MAIN_CONTACT_ACTION_C(ESC_ACTION_DISABLE);
//    }

//    //----------------ESC A����·�ű��̵ּ���δ��բ-----------------//
//    if((fabs(VoltInA_rms-VoltOutA_rms) > 20)&&(gridCurrBYAF_rms < 1.5f)&&(StateEventFlag_A == STATE_EVENT_WAIT_A))
//    {
//        ESCFlagA.FAULTCONFIRFlag = 1;
//        if(softwareFaultWord3.B.ESCBYRelayFaultA == 0){
//            softwareFaultWord3.B.ESCBYRelayFaultA = BYRelayFaultDetect(SOE_GP_FAULT+45,CNT_BYRelay_A,2,ESCFlagA.PHASE);
//            if(softwareFaultWord3.B.ESCBYRelayFaultA == 1)
//            {
//                ESCBYRelayCNTA = 1;
//            }
//        }
//    }else{
//        SetFaultDelayCounter(CNT_BYRelay_A,0);
//    }
//    //----------------ESC B����·�ű��̵ּ���δ��բ-----------------//
//    if((fabs(VoltInB_rms-VoltOutB_rms) > 20)&&(gridCurrBYBF_rms < 1.5f)&&(StateEventFlag_B == STATE_EVENT_WAIT_B))
//    {
//        ESCFlagB.FAULTCONFIRFlag = 1;
//        if(softwareFaultWord3.B.ESCBYRelayFaultB == 0){
//            softwareFaultWord3.B.ESCBYRelayFaultB = BYRelayFaultDetect(SOE_GP_FAULT+46,CNT_BYRelay_B,2,ESCFlagB.PHASE);
//            if(softwareFaultWord3.B.ESCBYRelayFaultB == 1)
//            {
//                ESCBYRelayCNTB = 1;
//            }
//        }
//    }else{
//        SetFaultDelayCounter(CNT_BYRelay_B,0);
//    }
//    //----------------ESC C����·�ű��̵ּ���δ��բ-----------------//
//    if((fabs(VoltInC_rms-VoltOutC_rms) > 20)&&(gridCurrBYCF_rms < 1.5f)&&(StateEventFlag_C == STATE_EVENT_WAIT_C))
//    {
//        ESCFlagC.FAULTCONFIRFlag = 1;
//        if(softwareFaultWord3.B.ESCBYRelayFaultC == 0){
//            softwareFaultWord3.B.ESCBYRelayFaultC = BYRelayFaultDetect(SOE_GP_FAULT+47,CNT_BYRelay_C,2,ESCFlagC.PHASE);
//            if(softwareFaultWord3.B.ESCBYRelayFaultC == 1)
//            {
//                ESCBYRelayCNTC = 1;
//            }
//        }
//    }else{
//        SetFaultDelayCounter(CNT_BYRelay_C,0);
//    }

//    //----------------ESC A��ߵ�ѹ�ű���δ��բ-----------------//
//    //�ù�����Ϊ���ж��豸������ͣ��ת������ʱ��,�жϸߵ�ѹ�ű��̵ּ����Ƿ�����.
//    if((fabs(VoltInA_rms-VoltOutA_rms) > 20)&&(StateEventFlag_A == STATE_EVENT_WAIT_A))
//    {
//        ESCFlagA.FAULTCONFIRFlag = 1;
//        if(softwareFaultWord3.B.ESCHighLowRelayA == 0){
//            softwareFaultWord3.B.ESCHighLowRelayA = FaultDetect(SOE_GP_FAULT+42,CNT_HIGHLOWRELAY_A,10,ESCFlagA.PHASE);
//            if(softwareFaultWord3.B.ESCHighLowRelayA == 1)
//            {
//                ESCHighLowRelayCNTA = 1;
//            }
//        }
//    }else{
//        SetFaultDelayCounter(CNT_HIGHLOWRELAY_A,0);
//    }
//    //----------------ESC B��ߵ�ѹ�ű���δ��բ-----------------//
//    if((fabs(VoltInB_rms-VoltOutB_rms) > 20)&&(StateEventFlag_B == STATE_EVENT_WAIT_B))
//    {
//        ESCFlagB.FAULTCONFIRFlag = 1;
//        if(softwareFaultWord3.B.ESCHighLowRelayB == 0){
//            softwareFaultWord3.B.ESCHighLowRelayB = FaultDetect(SOE_GP_FAULT+43,CNT_HIGHLOWRELAY_B,10,ESCFlagB.PHASE);
//            if(softwareFaultWord3.B.ESCHighLowRelayB == 1)
//            {
//                ESCHighLowRelayCNTB = 1;
//            }
//        }
//    }else{
//        SetFaultDelayCounter(CNT_HIGHLOWRELAY_B,0);
//    }
//    //----------------ESC C��ߵ�ѹ�ű���δ��բ-----------------//
//    if((fabs(VoltInC_rms-VoltOutC_rms) > 20)&&(StateEventFlag_C == STATE_EVENT_WAIT_C))
//    {
//        ESCFlagC.FAULTCONFIRFlag = 1;
//        if(softwareFaultWord3.B.ESCHighLowRelayC == 0){
//            softwareFaultWord3.B.ESCHighLowRelayC = FaultDetect(SOE_GP_FAULT+44,CNT_HIGHLOWRELAY_C,10,ESCFlagC.PHASE);
//            if(softwareFaultWord3.B.ESCHighLowRelayC == 1)
//            {
//                ESCHighLowRelayCNTC = 1;
//            }
//        }
//    }else{
//        SetFaultDelayCounter(CNT_HIGHLOWRELAY_C,0);
//    }

    //----------------ESC A��̼���������--------------------//
    if(((VoltInA_rms-VoltOutA_rms) > 20)&&(StateEventFlag_A == STATE_EVENT_RUN_A)&&(VoltInA_rms > 100))
    {
            ESCFlagA.FAULTCONFIRFlag = 1;
            if(softwareFaultWord3.B.ESCSicFaultFlagA == 0){
                ESCFlagA.ESCCntMs.StartDelay = 0;
                softwareFaultWord3.B.ESCSicFaultFlagA = FaultDetect(SOE_GP_FAULT+39,CNT_SICFAULT_A,2,ESCFlagA.PHASE);
                if(softwareFaultWord3.B.ESCSicFaultFlagA == 1)
                {
                    ESCSicFaultCNTA = 1;
                }
            }
    }else{
        SetFaultDelayCounter(CNT_SICFAULT_A,0);
    }
    //----------------ESC B��̼���������--------------------//
    if(((VoltInB_rms-VoltOutB_rms) > 20)&&(StateEventFlag_B == STATE_EVENT_RUN_B)&&(VoltInB_rms > 100))
    {
            ESCFlagB.FAULTCONFIRFlag = 1;
            if(softwareFaultWord3.B.ESCSicFaultFlagB == 0){
                ESCFlagB.ESCCntMs.StartDelay = 0;
                softwareFaultWord3.B.ESCSicFaultFlagB = FaultDetect(SOE_GP_FAULT+39,CNT_SICFAULT_B,2,ESCFlagB.PHASE);
                if(softwareFaultWord3.B.ESCSicFaultFlagB == 1)
                {
                    ESCSicFaultCNTB = 1;
                }
            }
    }else{
        SetFaultDelayCounter(CNT_SICFAULT_B,0);
    }
    //----------------ESC C��̼���������--------------------//
    if(((VoltInC_rms-VoltOutC_rms) > 20)&&(StateEventFlag_C == STATE_EVENT_RUN_C)&&(VoltInC_rms > 100))
    {
            ESCFlagC.FAULTCONFIRFlag = 1;
            if(softwareFaultWord3.B.ESCSicFaultFlagC == 0){
                ESCFlagC.ESCCntMs.StartDelay = 0;
                softwareFaultWord3.B.ESCSicFaultFlagC = FaultDetect(SOE_GP_FAULT+39,CNT_SICFAULT_C,2,ESCFlagC.PHASE);
                if(softwareFaultWord3.B.ESCSicFaultFlagC == 1)
                {
                    ESCSicFaultCNTC = 1;
                }
            }
    }else{
        SetFaultDelayCounter(CNT_SICFAULT_C,0);
    }

//    //----------------ESC A��̼���������--------------------//
//    if(((fabs(VoltInA_rms-VoltOutA_rms) > 10) || (fabs(gridCurA_rms-gridCurrBYAF_rms) > 10))&&(StateEventFlag_A == STATE_EVENT_RUN_A))
//    {
//        if(((VoltInA_rms/VoltOutA_rms)>0.86)||((VoltInA_rms/VoltOutA_rms)<0.51)||((gridCurrBYAF_rms/gridCurA_rms)>0.96)||((gridCurrBYAF_rms/gridCurA_rms)<0.51))
//        {
//            ESCFlagA.FAULTCONFIRFlag = 1;
//            if(softwareFaultWord3.B.ESCSicFaultFlagA == 0){
//                softwareFaultWord3.B.ESCSicFaultFlagA = FaultDetect(SOE_GP_FAULT+39,CNT_SICFAULT_A,10,ESCFlagA.PHASE);
//                if(softwareFaultWord3.B.ESCSicFaultFlagA == 1)
//                {
//                    ESCSicFaultCNTA = 1;
//                }
//            }
//        }
//    }else{
//        SetFaultDelayCounter(CNT_SICFAULT_A,1);
//    }
//    //----------------ESC B��̼���������--------------------//
//    if(((fabs(VoltInB_rms-VoltOutB_rms) > 10) || (fabs(gridCurB_rms-gridCurrBYBF_rms) > 10))&&(StateEventFlag_B == STATE_EVENT_RUN_B))
//    {
//        if(((VoltInB_rms/VoltOutB_rms)>0.86)||((VoltInB_rms/VoltOutB_rms)<0.51)||((gridCurrBYBF_rms/gridCurB_rms)>0.96)||((gridCurrBYBF_rms/gridCurB_rms)<0.51))
//        {
//            ESCFlagB.FAULTCONFIRFlag = 1;
//            if(softwareFaultWord3.B.ESCSicFaultFlagB == 0){
//                softwareFaultWord3.B.ESCSicFaultFlagB = FaultDetect(SOE_GP_FAULT+40,CNT_SICFAULT_B,10,ESCFlagB.PHASE);
//                if(softwareFaultWord3.B.ESCSicFaultFlagB == 1)
//                {
//                    ESCSicFaultCNTB = 1;
//                }
//            }
//        }
//    }else{
//        SetFaultDelayCounter(CNT_SICFAULT_B,1);
//    }
//    //----------------ESC C��̼���������--------------------//
//    if(((fabs(VoltInC_rms-VoltOutC_rms) > 10) || (fabs(gridCurC_rms-gridCurrBYCF_rms) > 10))&&(StateEventFlag_C == STATE_EVENT_RUN_C))
//    {
//        if(((VoltInC_rms/VoltOutC_rms)>0.96)||((VoltInC_rms/VoltOutC_rms)<0.51)||((gridCurrBYCF_rms/gridCurC_rms)>0.96)||((gridCurrBYCF_rms/gridCurC_rms)<0.51))
//        {
//            ESCFlagC.FAULTCONFIRFlag = 1;
//            if(softwareFaultWord3.B.ESCSicFaultFlagC == 0){
//                softwareFaultWord3.B.ESCSicFaultFlagC = FaultDetect(SOE_GP_FAULT+41,CNT_SICFAULT_C,10,ESCFlagC.PHASE);
//                if(softwareFaultWord3.B.ESCSicFaultFlagC == 1)
//                {
//                    ESCSicFaultCNTC = 1;
//                }
//            }
//        }
//    }else{
//        SetFaultDelayCounter(CNT_SICFAULT_C,1);
//    }
	//--------------- ����Ƶ�ʹ�Ƶ ------------------------//
    if((VoltInA_rms>15)||(VoltInB_rms>15)||(VoltInC_rms>15))
    {
        if(GridFreq >= GF_OVER)
        {
            ESCFlagA.FAULTCONFIRFlag = 1;
            ESCFlagB.FAULTCONFIRFlag = 1;
            ESCFlagC.FAULTCONFIRFlag = 1;
            if(softwareFaultWord3.B.ESCGridOverFreqenceFlag == 0){
                ESCFlagA.ESCCntMs.StartDelay = 0;
                ESCFlagB.ESCCntMs.StartDelay = 0;
                ESCFlagC.ESCCntMs.StartDelay = 0;
                softwareFaultWord3.B.ESCGridOverFreqenceFlag = THREEPHASEFaultDetect(SOE_GP_FAULT+35,CNT_OVER_FREQ,2000);
            }
        }else{
            SetFaultDelayCounter(CNT_OVER_FREQ,1);
        }
    }

	//--------------- ����Ƶ��ǷƵ ------------------------//
    if((VoltInA_rms>15)||(VoltInB_rms>15)||(VoltInC_rms>15))
    {
        if(GridFreq <= GF_UNDER)
        {
            ESCFlagA.FAULTCONFIRFlag = 1;
            ESCFlagB.FAULTCONFIRFlag = 1;
            ESCFlagC.FAULTCONFIRFlag = 1;
            if(softwareFaultWord3.B.ESCGridUnderFreqenceFlag == 0){
                ESCFlagA.ESCCntMs.StartDelay = 0;
                ESCFlagB.ESCCntMs.StartDelay = 0;
                ESCFlagC.ESCCntMs.StartDelay = 0;
                softwareFaultWord3.B.ESCGridUnderFreqenceFlag = THREEPHASEFaultDetect(SOE_GP_FAULT+36,CNT_UNDER_FREQ,4000);//ǷƵ��ʱ��Ҫ�Գ���Ƿѹ.��Ƿѹ���ȱ�����
            }
        }else{
            SetFaultDelayCounter(CNT_UNDER_FREQ,0);
        }
    }

//	//--------------- ���������� ---------����ͣ����ʱ����-----------//
//	if((!StateFlag.SequenceAutoFlag)&&(StateEventFlag == STATE_EVENT_STANDBY))
//		GridPhaseSequTest(1);
//
//	if(!StateFlag.startOnecRec &&  CntMs.StartDelay >= CNT_MS(100)){
//		StateFlag.startOnecRec=1;
//		SoeRecData(SOE_GP_EVENT+7);		//װ���ϵ��¼�
//	}

    //--------------- ɢ��Ƭ�¶ȹ��� ------------------------//   //ɢ�����¶�(������¶�)  80
    if(TempData[0] > WindCold.BOARD_OVER_TEMP)
    {
        ESCFlagA.FAULTCONFIRFlag = 1;
        ESCFlagB.FAULTCONFIRFlag = 1;
        ESCFlagC.FAULTCONFIRFlag = 1;
        if(softwareFaultWord2.B.ESCOverTemperatureFlagB == 0){
            ESCFlagA.ESCCntMs.StartDelay = 0;
            ESCFlagB.ESCCntMs.StartDelay = 0;
            ESCFlagC.ESCCntMs.StartDelay = 0;
            softwareFaultWord2.B.ESCOverTemperatureFlagB = THREEPHASEFaultDetect(SOE_GP_FAULT+24,CNT_SET_OVER_TEMP_B,20);
        }
    }else{
        SetFaultDelayCounter(CNT_SET_OVER_TEMP_B,1);
    }
    //--------------- ESC��Ԫ���ǹ��� ------------------------//  //�����ڲ��¶�  70
	if(TempData[1] > WindCold.UNIT_OVER_TEMP)
	{
        ESCFlagA.FAULTCONFIRFlag = 1;
        ESCFlagB.FAULTCONFIRFlag = 1;
        ESCFlagC.FAULTCONFIRFlag = 1;
		if(softwareFaultWord2.B.ESCChassisOverTempFlag == 0){
            ESCFlagA.ESCCntMs.StartDelay = 0;
            ESCFlagB.ESCCntMs.StartDelay = 0;
            ESCFlagC.ESCCntMs.StartDelay = 0;
		    softwareFaultWord2.B.ESCChassisOverTempFlag = THREEPHASEFaultDetect(SOE_GP_FAULT+30,CNT_SET_BOARD_OVER_TEMP,20);
		}
	}else{
		SetFaultDelayCounter(CNT_SET_BOARD_OVER_TEMP,1);
	}

    //--------------- ESC ��װ��������λ--------------//  //δʹ��
//    if(GET_UnitSUCCESS() == 1/*&&((StateEventFlag == STATE_EVENT_RUN)||(StateEventFlag == STATE_EVENT_WAIT))*/){
//        if(softwareFaultWord1.B.contractNoOpen == 0){
//            softwareFaultWord1.B.contractNoOpen = FaultDetect(SOE_GP_FAULT+16,CNT_BY_FEEDBACK_FAULT,20);
//        }
//    }else{
//        SetFaultDelayCounter(CNT_BY_FEEDBACK_FAULT,0);
//    }
//

    //--------------- ESC ���׹���λ--------------//
	if(windColdCtr != 0){     //ʹ�øò���, ����Ϊ0 ��Ϊ���ڲ�װ�����������ʱ��,��ֹ���ù���;����Ϊ1��Ϊ�� �ȵ�װ������֮���ٿ��Ǹù���;
        if(GET_FLback == 1){
            ESCFlagA.FAULTCONFIRFlag = 1;
            ESCFlagB.FAULTCONFIRFlag = 1;
            ESCFlagC.FAULTCONFIRFlag = 1;
            if(softwareFaultWord2.B.ESCFangLeiFaultFlag == 0){
                ESCFlagA.ESCCntMs.StartDelay = 0;
                ESCFlagB.ESCCntMs.StartDelay = 0;
                ESCFlagC.ESCCntMs.StartDelay = 0;
                softwareFaultWord2.B.ESCFangLeiFaultFlag = THREEPHASEFaultDetect(SOE_GP_FAULT+26,CNT_FANGLEI_FAULT,20);
            }
        }else{
            SetFaultDelayCounter(CNT_FANGLEI_FAULT,0);
        }
	}

	//�翹�����±���
//	if(0/*(TEMP1 == 1)||(TEMP2 == 0)*/){	//(inductanceTemLc > INDUCE_OVER_TEMP) //cntForInduceOverTemp
//		if(softwareFaultWord2.B.induceOverTempFlag == 0){
//			softwareFaultWord2.B.induceOverTempFlag = FaultDetect(SOE_GP_FAULT+19,CNT_INDUCTANCE_OVER_TEMP,4000);
//		}
//	}else{
//		SetFaultDelayCounter(CNT_INDUCTANCE_OVER_TEMP,0);
//	}

	ZeroOffsetJudgment();// �����㷨��ִ��ʱ��Ϊ 888.0*0.006667 =5.920296

	//����������
//	if( ((HarmTHD[TotalHarmDistorionVoltA].THD>VoltHarmOver)&&(gpVoltA_rms > 380*0.7f))||\
//		((HarmTHD[TotalHarmDistorionVoltB].THD>VoltHarmOver)&&(gpVoltB_rms > 380*0.7f))||\
//		((HarmTHD[TotalHarmDistorionVoltC].THD>VoltHarmOver)&&(gpVoltC_rms > 380*0.7f)) ){
//			if(EnCapSwitch.B.overVoltHarmFaltEn == 0){
//			if(softwareFaultWord2.B.overVoltHarm == 0){
//				softwareFaultWord2.B.overVoltHarm = FaultDetect(SOE_GP_FAULT+32,31,4000);
//			}
//		}
//	}else{
//		SetFaultDelayCounter( ,0);
//	}

//	if(BYIOin4 == 0){		//����ڹ���
//		if(softwareFaultWord2.B.airOverTemp == 0){
//			softwareFaultWord2.B.airOverTemp = FaultDetect(SOE_GP_FAULT+27,27,2000);
//		}
//	}else{
//		SetFaultDelayCounter(27,0);
//	}

	//--------------- г�� -------------------//
	if(ResonProtcABCRms > 5)
	{
        ErrorRecord.B.REC_HF = 1;
        if(CntMs.HFOverCurrentCount >= CNT_MS(60))      StateFlag.HFOverCurrentFlag = 1;
        if(ResonProtcABCRms > 15)                       CntMs.HFOverCurrentCount+=10;
        if(ResonProtcABCRms > 25)                       CntMs.HFOverCurrentCount+=60;
		if(CntMs.HFOverCurrentCount >= CNT_MS(500)) //
		{
	        if(ESCFlagA.PHASE == 1)          ESCFlagA.FAULTCONFIRFlag = 1;
	        if(ESCFlagB.PHASE == 2)          ESCFlagB.FAULTCONFIRFlag = 1;
	        if(ESCFlagC.PHASE == 3)          ESCFlagC.FAULTCONFIRFlag = 1;
#if !TEST_NULLPULSE
			if(!softwareFaultWord3.B.ESCResonanceFlautFlag)				SoeRecData(SOE_GP_FAULT+34);
			softwareFaultWord3.B.ESCResonanceFlautFlag = 1;
			StateEventFlag_A = STATE_EVENT_FAULT_A;    //???
			StateEventFlag_B = STATE_EVENT_FAULT_B;
			StateEventFlag_C = STATE_EVENT_FAULT_C;
	        ESCFlagA.faultFlag = 1;
	        ESCFlagB.faultFlag = 1;
	        ESCFlagC.faultFlag = 1;
#endif
		}
	}else{
        ErrorRecord.B.REC_HF = 0;
		if(CntMs.HFOverCurrentCount > 0){
			CntMs.HFOverCurrentCount -= 2;									//��ʱ�޻ָ�
		}else{
			CntMs.HFOverCurrentCount 	=0;
			StateFlag.HFOverCurrentFlag = 0;								//Pϵ���ָ�Ϊ1��ģʽ
		}
	}
}

void FaultReset(void)
{
	if(((ESCFlagA.faultFlag == 0)||(ESCFlagB.faultFlag == 0)||(ESCFlagC.faultFlag == 0)) && (StateFlag.resetFaultOnceTime == 0))
	{
	//-	resetFaultOnceTime = 1;
		softwareFaultWord1.all = 0;
		softwareFaultWord2.all = 0;
		softwareFaultWord3.all = 0;
		softwareFaultWord4.all = 0;
		softwareFaultWord5.all = 0;
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
	static float theta_c ;
	static float theta_c_Prior ;

	float a=0, b=0,  tmp1 =0;
	if(StateEventFlag_A != STATE_EVENT_RUN_A)
	{
		a = GridVoltAlpha;
		b = GridVoltBeta ;

		theta_c = atan2(a,b);   // ------- ����rts2800_fpu32.lib    ����ʱ����಻����  4us
                                                      // ------- ���ǲ��ø�����������󣬼���ʱ����಻����   0.6us
		if((a>0)&&(b<0))
			theta_c  = -theta_c + 4.7123889; // 3/2*pi;
		else if ((a>0) && (b>0))
			theta_c = -theta_c + 1.5707963;  //pi/2;
		else if ((a<0) && (b>0))
			theta_c = -theta_c + 1.5707963;  //pi/2;
		else if ((a<0) && (b<0))
			theta_c = -theta_c + 4.7123889; // 3/2*pi;

		if((theta_c > 1) && (theta_c < 6) )  // �ܿ���������
		{
			tmp1 = theta_c - theta_c_Prior;

			if(i){
//				if(tmp1 < 0)
//				{
//					if(softwareFaultWord1.B.gridVoltPhaseSeqFaultFlag == 0){
//						softwareFaultWord1.B.gridVoltPhaseSeqFaultFlag = FaultDetect(SOE_GP_FAULT+4,CNT_PHASE_SEQUENCE,1000);
//					}
//				}else{
//					SetFaultDelayCounter(CNT_PHASE_SEQUENCE,10);
//				}
			}else{
			    if(tmp1 < 0){
			    	cntGridVoltPhaseSeqFault++;
			    	if(cntGridVoltPhaseSeqFault > 300)
			    	{
			    		FactorySet.Infer.B.VoltagePhaseSequence = ~FactorySet.Infer.B.VoltagePhaseSequence;
			    		CorrectingAD();
			    		StateFlag.PLLSafetyFlag = 0;
						cntGridVoltPhaseSeqFault=0;
			    	}
			    }else{
					cntGridVoltPhaseSeqFault-=10;
					if(cntGridVoltPhaseSeqFault <= 1)
						cntGridVoltPhaseSeqFault = 1;
				}
			}
			theta_c_Prior = theta_c;
		}else{
			theta_c_Prior = 1;
		}
	}
}


// -----------------------NO MORE-----------------------------//





