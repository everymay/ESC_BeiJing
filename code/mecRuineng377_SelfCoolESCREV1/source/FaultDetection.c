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
	 * 测试程序：
	   记录电网电压有效值VoltInA_rms和负载电压有效值VoltOutA_rms
	 */
#define LENTH 50 //数组长度

	unsigned int voltage_grid_A[LENTH] = {0}; //测试。A相电网电压有效值。WY
	unsigned int voltage_load_A[LENTH] = {0}; //测试。A相负载电压有效值。WY

	unsigned int voltage_grid_B[LENTH] = {0}; //测试。WY
	unsigned int voltage_load_B[LENTH] = {0}; //测试。WY

	unsigned int voltage_grid_C[LENTH] = {0}; //测试。WY
	unsigned int voltage_load_C[LENTH] = {0}; //测试。WY

	unsigned int AD_grid_A[LENTH] = {0}; //测试。A相电网侧采样值。WY
	unsigned int AD_load_A[LENTH] = {0}; //测试。

	unsigned int AD_grid_B[LENTH] = {0}; //测试。
	unsigned int AD_load_B[LENTH] = {0}; //测试。

	unsigned int AD_grid_C[LENTH] = {0}; //测试。
	unsigned int AD_load_C[LENTH] = {0}; //测试。


	unsigned int index_A = 0; //测试。A相数组索引。WY
	unsigned int index_B = 0; //测试。数组索引。WY
	unsigned int index_C = 0; //测试。数组索引。WY

/*
 * 功能：故障停机预处理。WY
 */
void FirstFaultStop(Uint16 ESCPHASE)
{
	/*处理A相。WY*/
	if ((ESCFlagA.FAULTCONFIRFlag == 1) //A相存在故障信号。WY
			&& (ESCPHASE == 1)) //A相。WY
	{
		SET_IGBT_ENA(IGBT_FAULT); //失能A相PWM
		DisablePWMA(); //封锁A相PWM
		SET_SCRA_ENABLE(ESC_ACTION_ENABLE); //开通A相旁路晶闸管。WY
		SET_MAIN_CONTACT_ACTION_A(ESC_ACTION_DISABLE); //断开A相主路磁保持继电器。WY
		SET_BYPASS_CONTACT_ACTION_A(ESC_ACTION_ENABLE); //闭合A相旁路磁保持继电器。WY
		ESCFlagA.FaultskipFlag = 1;
		ESCFlagA.FAULTCONFIRFlag = 0; //故障信号已得到处理。WY
		ESCFlagA.ESCCntMs.StartDelay = 0;
		ESCFlagA.HWPowerFAULTFlag = 1; //A相硬件存在故障。WY
		ESCFlagA.ESCCntSec.HWPowerFaultDelay = 0;
	}

	/*处理B相。WY*/
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

	/*处理C相。WY*/
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
 * 功能：检测主路和旁路磁保持继电器的通断状态。WY
 * 说明：该函数在ADC-D-1的中断ISR中被调用。WY
 */
void StateFeedBackJudge(void)
{
	/*处理A相。WY*/
	if (GET_BYPASS_CONTACT_ACTION_A == 0) //检测到A相旁路磁保持继电器闭合信号。WY
	{
		if (++BypassContactCntA >= TESTVALAUE) //多次检测。WY
		{
			BypassContactCntA = TESTVALAUE;
			ESCFlagA.ByPassContactFlag = 1; //A相旁路磁保持继电器闭合。WY
		}
	}
	else //检测到A相旁路磁保持继电器断开信号。WY
	{
		if (--BypassContactCntA <= 0) //多次检测。WY
		{
			BypassContactCntA = 0;
			ESCFlagA.ByPassContactFlag = 0; //A相旁路磁保持继电器断开。WY
		}
	}

	/*处理B相。WY*/
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

	/*处理C相。WY*/
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

	/*处理A相*/
	if (GET_MAIN_CONTACT_ACTION_A == 0) //检测到A相主路磁保持继电器闭合的信号。WY
	{
		if (++ContactorFeedBackCntA >= TESTVALAUE) //多次检测。WY
		{
			ContactorFeedBackCntA = TESTVALAUE;
			ESCFlagA.ContactorFeedBackFlag = 1;//A相主路磁保持继电器闭合。WY
		}
	}
	else //检测到A相主路磁保持继电器断开的信号。WY
	{
		if (--ContactorFeedBackCntA <= 0)
		{
			ContactorFeedBackCntA = 0;
			ESCFlagA.ContactorFeedBackFlag = 0;
		}
	}

	/*处理B相*/
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

	/*处理C相*/
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
 * 功能：检测故障。WY
 *
 * 输入参数SoeName：触控屏上显示的故障类型索引。WY
 * 输入参数counterName：故障信号计数器的索引。
 * 输入参数FaultDelayTime：故障信号次数上限值。
   故障信号计数器 < 故障信号次数上限值，不作故障判定；
   故障信号计数器 > 故障信号次数上限值，执行故障判定。
 * 输入参数ESCPHASE：待判定故障类型的相。
 */
int16 FaultDetect(Uint16 SoeName, Uint16 counterName, Uint16 FaultDelayTime, Uint16 ESCPHASE)
{
	/*
	 * 故障信号计数器 > 故障信号次数上限值，执行故障判定。WY
	 * 目的：防止误测。
	 */
	if (CntFaultDelay[counterName]++ >= FaultDelayTime)
	{
		DINT;

		if (CTRL24_POWERFlag == 1) //15V电源故障。WY
		{
			EPwm1Regs.AQCSFRC.bit.CSFA = EPwmRegsAQCSFRCclear; //PWM强制持续输出高电平，风机停机。WY
			CTRL24_POWERFlag = 0; //已响应。WY
		}

		if ((StateEventFlag_A == STATE_EVENT_WAIT_A) //A相处于待机状态。WY
				&& (StateEventFlag_B == STATE_EVENT_WAIT_B) //B相处于待机状态。WY
				&& (StateEventFlag_C == STATE_EVENT_WAIT_C)) //C相处于待机状态。WY
		{
			SET_POWER_CTRL(1); //15V电源上电。WY
			Delayus(TIME_WRITE_15VOLT_REDAY);
		}

		/*处理A相*/
		if ((ESCFlagA.FAULTCONFIRFlag == 1) //A相存在故障信号。WY
				&& (ESCPHASE == 1) //A相。WY
				&& (ESCBYRelayCNTA != 1)) //A相旁路磁保持继电器状态正常。WY
		{
			StateEventFlag_A = STATE_EVENT_FAULT_A;//切转状态机至：A相故障停机。WY
			ESCFlagA.faultFlag = 1; //A相存在故障。WY
			FirstFaultStop(ESCFlagA.PHASE); //故障停机预处理。WY
			ESCFlagA.ESCCntMs.CutCurrDelay = 0;
		}

		/*处理B相*/
		if ((ESCFlagB.FAULTCONFIRFlag == 1) && (ESCPHASE == 2) && (ESCBYRelayCNTB != 1))
		{
			StateEventFlag_B = STATE_EVENT_FAULT_B;
			ESCFlagB.faultFlag = 1;
			FirstFaultStop(ESCFlagB.PHASE);
			ESCFlagB.ESCCntMs.CutCurrDelay = 0;
		}

		/*处理C相*/
		if ((ESCFlagC.FAULTCONFIRFlag == 1) && (ESCPHASE == 3) && (ESCBYRelayCNTC != 1))
		{
			StateEventFlag_C = STATE_EVENT_FAULT_C;
			ESCFlagC.faultFlag = 1;
			FirstFaultStop(ESCFlagC.PHASE);
			ESCFlagC.ESCCntMs.CutCurrDelay = 0;
		}

		EINT;

		SoeRecData(SoeName); //生成日志。WY
		WriteFlashConfig(SoeName); //向Flash写入故障录波。WY
		CntFaultDelay[counterName] = 0; //清零故障信号计数器。WY

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
 * 功能：故障检测。WY
 * 输入参数SoeName：触控屏上显示的故障类型索引。WY
 * 输入参数counterName：故障信号计数器的索引。
 * 输入参数FaultDelayTime：故障信号次数上限值。
   故障信号计数器 < 故障信号次数上限值，不作故障判定；
   故障信号计数器 > 故障信号次数上限值，执行故障判定。
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
 * 功能：15V电源故障检测。WY
 * 输入参数SoeName：触控屏上显示的故障类型索引。WY
 * 输入参数counterName：故障信号计数器的索引。
 * 输入参数FaultDelayTime：故障信号次数上限值。
   故障信号计数器 < 故障信号次数上限值，不作故障判定；
   故障信号计数器 > 故障信号次数上限值，执行故障判定。
 */
int16 PowerFailFaultDetect(Uint16 SoeName,Uint16 counterName,Uint16 FaultDelayTime)
{
	/*
	 * 故障信号计数器 > 故障信号次数上限值，执行故障判定。WY
	 * 目的：防止误测。
	 */
	if (CntFaultDelay[counterName]++ >= FaultDelayTime)
	{
		DINT;
		if (CTRL24_POWERFlag == 1) //15V电源故障，等待响应。WY
		{
			EPwm1Regs.AQCSFRC.bit.CSFA = EPwmRegsAQCSFRCclear; //PWM强制持续输出高电平，风机停机。WY
			CTRL24_POWERFlag = 0; //已响应。WY
		}
		SET_POWER_CTRL(1); //启动15V电源。WY

		/*处理A相*/
		if ((ESCFlagA.FAULTCONFIRFlag == 1) //A相存在故障信号。WY
				&& (ESCBYRelayCNTA != 1)) //A相旁路磁保持继电器状态正常。WY
		{
			StateEventFlag_A = STATE_EVENT_FAULT_A;//切转状态机至：A相故障停机。WY
			ESCFlagA.faultFlag = 1; //A相存在故障。WY
			FirstFaultStop(ESCFlagA.PHASE); //故障停机预处理。WY
			ESCFlagA.ESCCntMs.CutCurrDelay = 0;
		}

		/*处理B相*/
		if ((ESCFlagB.FAULTCONFIRFlag == 1) && (ESCBYRelayCNTB != 1))
		{
			StateEventFlag_B = STATE_EVENT_FAULT_B;
			ESCFlagB.faultFlag = 1;
			FirstFaultStop(ESCFlagB.PHASE);
			ESCFlagB.ESCCntMs.CutCurrDelay = 0;
		}

		/*处理C相*/
		if ((ESCFlagC.FAULTCONFIRFlag == 1) && (ESCBYRelayCNTC != 1))
		{
			StateEventFlag_C = STATE_EVENT_FAULT_C;
			ESCFlagC.faultFlag = 1;
			FirstFaultStop(ESCFlagC.PHASE);
			ESCFlagC.ESCCntMs.CutCurrDelay = 0;
		}

		EINT;

		SoeRecData(SoeName); //生成日志。WY
		WriteFlashConfig(SoeName); //向Flash写入故障录波。WY
		CntFaultDelay[counterName] = 0; //清零故障信号计数器。WY

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
 * 功能：旁路磁保持继电器故障检测。WY
 * 输入参数SoeName：触控屏上显示的故障类型索引。WY
 * 输入参数counterName：故障信号计数器的索引。
 * 输入参数FaultDelayTime：故障信号次数上限值。
   故障信号计数器 < 故障信号次数上限值，不作故障判定；
   故障信号计数器 > 故障信号次数上限值，执行故障判定。
 * 输入参数ESCPHASE：待判定故障类型的相。
 */
int16 BYRelayFaultDetect(Uint16 SoeName, Uint16 counterName, Uint16 FaultDelayTime, Uint16 ESCPHASE)
{
	/*
	 * 故障信号计数器 > 故障信号次数上限值，执行故障判定。WY
	 * 目的：防止误测。
	 */
	if (CntFaultDelay[counterName]++ >= FaultDelayTime)
	{
		DINT;

		if (CTRL24_POWERFlag == 1) //15V电源故障，等待执行响应。WY
		{
			EPwm1Regs.AQCSFRC.bit.CSFA = EPwmRegsAQCSFRCclear; //PWM强制持续输出高电平，风机停机。WY
			CTRL24_POWERFlag = 0; //已响应。WY
		}

		SET_POWER_CTRL(1); //15V电源上电。WY
		Delayus(TIME_WRITE_15VOLT_REDAY);

		/*处理A相：旁路磁保持继电器故障。WY*/
		if ((ESCFlagA.FAULTCONFIRFlag == 1) //A相存在故障信号。WY
				&& (ESCPHASE == 1)) //A相。WY
		{
			StateEventFlag_A = STATE_EVENT_FAULT_A; //切换状态机至：A相故障停机。WY
			ESCFlagA.faultFlag = 1; //A相存在故障。WY
			ESCFlagA.PWM_ins_index = 1; //直通主路PWM。WY
			ESCFlagA.ESC_DutyData = 1.0; //PWM占空比为1。WY
			SET_SCRA_ENABLE(ESC_ACTION_ENABLE); //开通A相旁路晶闸管。WY
			SET_MAIN_CONTACT_ACTION_A(ESC_ACTION_ENABLE); //闭合主路磁保持继电器。WY
			SET_BYPASS_CONTACT_ACTION_A(ESC_ACTION_ENABLE); //闭合旁路磁保持继电器。WY
			Delayus(TIME_WRITE_15VOLT_REDAY);
			EnablePWMA(); //软件解锁A相PWM。WY
			Delayus(100);
			SET_IGBT_ENA(IGBT_ENABLE); //硬件使能A相PWM。WY

			EPwm3Regs.CMPA.bit.CMPA = T1PR;
			EPwm3Regs.CMPB.bit.CMPB = T1PR;
			EPwm4Regs.CMPA.bit.CMPA = T1PR;
			EPwm4Regs.CMPB.bit.CMPB = T1PR;

			ESCFlagA.FaultskipFlag = 1;
			ESCFlagA.FAULTCONFIRFlag = 0; //不存在故障信号。WY
			ESCFlagA.ESCCntMs.StartDelay = 0;
		}

		/*处理B相：旁路磁保持继电器故障。WY*/
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

		/*处理C相：旁路磁保持继电器故障。WY*/
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

		SoeRecData(SoeName);//生成日志。WY
		WriteFlashConfig(SoeName); //向Flash写入故障录波。WY
		CntFaultDelay[counterName] = 0; //清零故障信号计数器。WY

		return 1;
	}
	else //故障信号计数器 <= 故障信号次数上限值。WY
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

/*该函数未使用。WY*/
int16 FaultDetectExtCnt(Uint16 SoeName,Uint16 extCnt,Uint16 FaultDelayTime)
{
	if(FaultDelayTime >= extCnt) //单元外接干扰太大造成误报
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

/*该函数未使用。WY*/
int16 FaultDetectLong(Uint16 SoeName,Uint16 counterName,Uint32 FaultDelayTime)
{
	if(CntFaultDelayLong[counterName]++ > FaultDelayTime) //单元外接干扰太大造成误报
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

/*该函数未使用。WY*/
int16 StandbyDetectLong(Uint16 SoeName,Uint16 counterName,Uint32 StandbyDelayTime)
{
    if(CntFaultDelayLong[counterName]++ > StandbyDelayTime) //单元外接干扰太大造成误报
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
        SET_BYPASS_CONTACT_ACTION_A(ESC_ACTION_ENABLE);                            //切旁路
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

/*该函数未使用。WY*/
int16 StandbyDetect(Uint16 SoeName,Uint16 counterName,Uint32 StandbyDelayTime)
{
    if(CntFaultDelayLong[counterName]++ > StandbyDelayTime) //单元外接干扰太大造成误报
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
        SET_BYPASS_CONTACT_ACTION_A(ESC_ACTION_ENABLE);                            //切旁路
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
 * 功能：清零故障信号计数器的值。WY
 * 输入参数counterName：故障信号计数器的索引
 * 输入参数mode：固定为0。
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

/*该函数未使用。WY*/
void SetFaultDelayCounterLong(Uint16 counterName,Uint32 mode)
{
	if(CntFaultDelayLong[counterName] > mode)
		CntFaultDelayLong[counterName] -= mode;
	else
		CntFaultDelayLong[counterName] = 0;
}

/*该函数未使用。WY*/
void FaultDetectInInt(void)
{}

/*
 * ECAP-1中断服务函数。WY
 * 捕获对象：A相硬件过流
 * 信号输入引脚：GPIO94
 */
void EcapINT1(void)
{
	if (StateEventFlag_A == STATE_EVENT_RUN_A) //A相处于运行状态。WY
	{
		ESCFlagA.FAULTCONFIRFlag = 1; //存在故障信号。WY

		if ((softwareFaultWord1.B.ESCFastHardwareOverCurFlagA == 0)) //A相不存在硬件过流故障。WY
		{
			ESCFlagA.ESCCntMs.StartDelay = 0;
			softwareFaultWord1.B.ESCFastHardwareOverCurFlagA = FaultDetect(SOE_GP_FAULT + 1, CNT_HW_OVER_CUR_A, 0, ESCFlagA.PHASE); //A相存在硬件过流故障。WY
		}
	}
	else
	{
		SetFaultDelayCounter(CNT_HW_OVER_CUR_A, 0); //清零A相硬件过流故障信号次数。WY
	}

	ECap1Regs.ECCLR.bit.CEVT4 = 1; //复位事件-4标志位。WY
	ECap1Regs.ECCLR.bit.CEVT3 = 1; //复位事件-3标志位。WY
	ECap1Regs.ECCLR.bit.CEVT2 = 1; //复位事件-2标志位。WY
	ECap1Regs.ECCLR.bit.CEVT1 = 1; //复位事件-1标志位。WY
	ECap1Regs.ECCLR.bit.INT = 1; //复位中断标志位。WY

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP4; //复位中断组。WY
}

/*
 * ECAP-2中断服务函数。WY
 * 捕获对象：B相硬件过流
 * 信号输入引脚：GPIO85
 */
void EcapINT2(void)
{
	if (StateEventFlag_B == STATE_EVENT_RUN_B) //B相处于运行状态。WY
	{
		ESCFlagB.FAULTCONFIRFlag = 1; //存在故障信号。WY

		if ((softwareFaultWord1.B.ESCFastHardwareOverCurFlagB == 0)) //B相不存在硬件过流故障。WY
		{
			ESCFlagB.ESCCntMs.StartDelay = 0;
			softwareFaultWord1.B.ESCFastHardwareOverCurFlagB = FaultDetect(SOE_GP_FAULT + 2, CNT_HW_OVER_CUR_B, 0, ESCFlagB.PHASE); //B相存在硬件过流故障。WY
		}
	}
	else
	{
		SetFaultDelayCounter(CNT_HW_OVER_CUR_B, 0);
	}

	ECap2Regs.ECCLR.bit.CEVT4 = 1; //复位事件-4标志位。WY
	ECap2Regs.ECCLR.bit.CEVT3 = 1; //复位事件-3标志位。WY
	ECap2Regs.ECCLR.bit.CEVT2 = 1; //复位事件-2标志位。WY
	ECap2Regs.ECCLR.bit.CEVT1 = 1; //复位事件-1标志位。WY
	ECap2Regs.ECCLR.bit.INT = 1; //复位中断标志位。WY
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP4; //复位中断组。WY
}

/*
 * ECAP-3中断服务函数。WY
 * 捕获对象：C相硬件过流
 * 信号输入引脚：GPIO97
 */
void EcapINT3(void)
{
	if (StateEventFlag_C == STATE_EVENT_RUN_C) //C相处于运行状态。WY
	{
		ESCFlagC.FAULTCONFIRFlag = 1; //存在故障信号。WY

		if ((softwareFaultWord1.B.ESCFastHardwareOverCurFlagC == 0)) //C相不存在硬件过流故障。WY
		{
			ESCFlagC.ESCCntMs.StartDelay = 0;
			softwareFaultWord1.B.ESCFastHardwareOverCurFlagC = FaultDetect(SOE_GP_FAULT + 3, CNT_HW_OVER_CUR_C, 0, ESCFlagC.PHASE); //C相存在硬件过流故障。WY
		}
	}
	else
	{
		SetFaultDelayCounter(CNT_HW_OVER_CUR_C, 0);
	}
	ECap3Regs.ECCLR.bit.CEVT4 = 1; //复位事件-4标志位。WY
	ECap3Regs.ECCLR.bit.CEVT3 = 1; //复位事件-4标志位。WY
	ECap3Regs.ECCLR.bit.CEVT2 = 1; //复位事件-4标志位。WY
	ECap3Regs.ECCLR.bit.CEVT1 = 1; //复位事件-4标志位。WY
	ECap3Regs.ECCLR.bit.INT = 1; //复位中断标志位。WY
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP4; //复位中断组。WY
}

/*
 * 功能：快速故障检测。WY
 * 说明：该函数在ADC-D-1的中断服务函数中被调用，调用频率20K，调用周期50us。WY
 */
void FaultFastDetectInInt(void)
{
//由于该故障判断是放在中断中,而上电进行零偏校准的时间较长,所以要加上状态判断,防止在零偏校准时,进入该故障,使状态位ESCFlagA.FAULTCONFIRFlag置1;
	if (PhaseControl == 1)
	{
		/*处理A相：瞬时值过流故障*/
		if (((GridCurrAF > OUTCUR_OVER_INS) //A相电网电流瞬时值 > 电流瞬时值上限。WY
				|| (GridBPCurrAF > OUTCUR_OVER_INS)) //A相旁路电流瞬时值 > 电流瞬时值上限。WY
				&& (StateEventFlag_A != STATE_EVENT_STANDBY_A)) //A相不处于初始状态。WY
		{
			ESCFlagA.FAULTCONFIRFlag = 1; //存在故障信号。WY

			if (softwareFaultWord1.B.ESCInsOverCurFlagA == 0) //不存在A相瞬时值过流故障。WY
			{
				ESCFlagA.ESCCntMs.StartDelay = 0;
				softwareFaultWord1.B.ESCInsOverCurFlagA = FaultDetect(SOE_GP_FAULT + 4, CNT_INS_OVER_CUR_A, 3, ESCFlagA.PHASE); //产生A相瞬时值过流故障。WY
			}
		}
		else
		{
			SetFaultDelayCounter(CNT_INS_OVER_CUR_A, 0); //清零：A相瞬时值过流故障信号次数。WY
		}

		/*处理B相：瞬时值过流故障*/
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

		/*处理C相：瞬时值过流故障*/
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
		/*处理A相：瞬时值过流故障*/
		if (((GridCurrAF < OUTCUR_OVER_INS_NEG) //A相电网电流瞬时值 < 电流瞬时值下限。WY
				|| (GridBPCurrAF < OUTCUR_OVER_INS_NEG)) //A相负载电流瞬时值 < 电流瞬时值下限。WY
				&& (StateEventFlag_A != STATE_EVENT_STANDBY_A)) //A相不处于初始状态。WY
		{
			ESCFlagA.FAULTCONFIRFlag = 1; //存在故障信号。WY

			if (softwareFaultWord1.B.ESCInsOverCurFlagA == 0) //不存在A相瞬时值过流故障。WY
			{
				ESCFlagA.ESCCntMs.StartDelay = 0;
				softwareFaultWord1.B.ESCInsOverCurFlagA = FaultDetect(SOE_GP_FAULT + 4, CNT_INS_OVER_CUR_A, 3, ESCFlagA.PHASE); //产生A相瞬时值过流故障。WY
			}
		}
		else
		{
			SetFaultDelayCounter(CNT_INS_OVER_CUR_A, 0); //清零：A相瞬时值过流故障信号次数。WY
		}

		/*处理B相：瞬时值过流故障*/
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

		/*处理C相：瞬时值过流故障*/
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
		/*处理A相：瞬时值过流故障*/
		if (((GridCurrAF > OUTCUR_OVER_INS) //A相电网电流瞬时值 > 电流瞬时值上限。WY
				|| (GridCurrAF < OUTCUR_OVER_INS_NEG) //A相电网电流瞬时值 < 电流瞬时值下限。WY
				|| (GridBPCurrAF > OUTCUR_OVER_INS) //A相旁路电流瞬时值 > 电流瞬时值上限。WY
				|| (GridBPCurrAF < OUTCUR_OVER_INS_NEG)) //A相旁路电流瞬时值 < 电流瞬时值下限。WY
				&& (StateEventFlag_A != STATE_EVENT_STANDBY_A)) //A相不处于初始状态。WY
		{
			ESCFlagA.FAULTCONFIRFlag = 1; //存在故障信号。WY

			if (softwareFaultWord1.B.ESCInsOverCurFlagA == 0) //不存在A相瞬时值过流故障。WY
			{
				ESCFlagA.ESCCntMs.StartDelay = 0;
				softwareFaultWord1.B.ESCInsOverCurFlagA = FaultDetect(SOE_GP_FAULT + 4, CNT_INS_OVER_CUR_A, 3, ESCFlagA.PHASE); //产生A相瞬时值过流故障。WY
			}
		}
		else
		{
			SetFaultDelayCounter(CNT_INS_OVER_CUR_A, 0); //清零：A相瞬时值过流故障信号次数。WY
		}

		/*处理B相：瞬时值过流故障*/
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

		/*处理C相：瞬时值过流故障*/
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

	/*A相：直流电容电压过压故障*/
	if (DccapVoltA > ESCDCVOLITLIMIT) //A相直流电容电压 > 电容电压上限值.WY
	{
		ESCFlagA.FAULTCONFIRFlag = 1; //A相存在故障。WY

		if (softwareFaultWord1.B.ESCDcCapVoltOverVoltFlagA == 0) //不存在A相直流电容电压过压故障。WY
		{
			ESCFlagA.ESCCntMs.StartDelay = 0;
			softwareFaultWord1.B.ESCDcCapVoltOverVoltFlagA = FaultDetect(SOE_GP_FAULT + 7, CNT_DC_NEUTRAL_OVER_VOLT_A, 10, ESCFlagA.PHASE); //产生A相直流电容电压过压故障。WY
		}
	}
	else
	{
		SetFaultDelayCounter(CNT_DC_NEUTRAL_OVER_VOLT_A, 0); //清零：A相直流电容电压过压故障信号次数
	}

	/*B相：直流电容电压过压故障*/
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

	/*C相：直流电容电压过压故障*/
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

	/*A相:电压有效值过压故障。WY*/
	if (((VoltOutA_rms >= GV_RMS_OVER) //A相负载电压有效值 > 电压有效值上限。WY
			|| (VoltInA_rms >= GV_RMS_OVER)) //A相电网电压有效值 > 电压有效值上限。WY
			&& (StateEventFlag_A != STATE_EVENT_STANDBY_A)) //A相处于非停机状态。WY
	{
		ESCFlagA.FAULTCONFIRFlag = 1; //存在故障信号。WY

		if (softwareFaultWord1.B.ESCGridRMSOverVoltFlagA == 0) //不存在A相电网电压有效值过压故障。WY
		{
			ESCFlagA.ESCCntMs.StartDelay = 0;
			softwareFaultWord1.B.ESCGridRMSOverVoltFlagA = FaultDetect(SOE_GP_FAULT + 10, CNT_RMS_OVER_VOLT_A, 20, ESCFlagA.PHASE); //产生A相电压有效值过压故障。WY
		}
	}
	else
	{
		SetFaultDelayCounter(CNT_RMS_OVER_VOLT_A, 0); //清零：A相电压有效值过压故障信号次数。WY
	}

	/*B相：电压有效值过压故障。WY*/
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

	/*C相：电压有效值过压故障。WY*/
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

//TempEnvirProvalue==1时,是做温升实验,需要风机全部全速转,但是开关电源带不起来,会报掉电故障,所以在做温升实验的时候需要把该故障屏蔽掉.

	/*15V电源掉电故障*/
	if (TempEnvirProvalue != 1)
	{
		if (GET_CTRL24_POWER == 1) //15V电源异常。WY
		{
			CTRL24_POWERFlag = 1; //15V电源故障。WY

			ESCFlagA.FAULTCONFIRFlag = 1; //A相存在故障信号。WY
			ESCFlagB.FAULTCONFIRFlag = 1; //B相存在故障信号。WY
			ESCFlagC.FAULTCONFIRFlag = 1; //C相存在故障信号。WY

			if (softwareFaultWord1.B.ESCPowerFailDetectFlag == 0) //不存在15V电源掉电检测故障。WY
			{
				ESCFlagA.ESCCntMs.StartDelay = 0;
				ESCFlagB.ESCCntMs.StartDelay = 0;
				ESCFlagC.ESCCntMs.StartDelay = 0;
				softwareFaultWord1.B.ESCPowerFailDetectFlag = PowerFailFaultDetect(SOE_GP_FAULT + 13, CNT_CTRL_POWER_OFF, 10); //产生15V电源掉电检测故障。WY
			}
		}
		else
		{
			SetFaultDelayCounter(CNT_CTRL_POWER_OFF, 0); //清零：15V电源掉电检测故障信号次数。WY
		}
	}
}

/*
 * 功能：故障检测。WY
 *
 * 说明：该函数在RTOS任务中被调用，调用周期约40ms。WY
 */
void FaultDetectInMainLoop(void)
{
	/*A相电压有效值欠压故障。WY*/
	if (((VoltInA_rms <= GV_RMS_UNDER) //A相电网电压有效值 <= 电压有效值下限。WY
			|| (VoltOutA_rms <= GV_RMS_UNDER)) //A相负载电压有效值 <= 电压有效值下限。WY
			&& ((StateEventFlag_A == STATE_EVENT_RUN_A) //A相处于运行状态。WY
			|| (StateEventFlag_A == STATE_EVENT_WAIT_A))) //A相处于待机状态。WY
	{
		ESCFlagA.FAULTCONFIRFlag = 1; //A相存在故障信号。WY

		if (!softwareFaultWord1.B.ESCGridRMSUnderVoltFlagA) //不存在A相电压有效值欠压故障。WY
		{
			ESCFlagA.ESCCntMs.StartDelay = 0;
			softwareFaultWord1.B.ESCGridRMSUnderVoltFlagA = FaultDetect(SOE_GP_FAULT + 14, CNT_RMS_UNDER_VOLT_A, 20, ESCFlagA.PHASE); //产生A相电压有效值欠压故障。WY
		}
	}
	else
	{
		SetFaultDelayCounter(CNT_RMS_UNDER_VOLT_A, 0); //清零：A相电压有效值欠压故障信号次数。WY
	}

	/*B相电压有效值欠压故障。WY*/
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

	/*C相电压有效值欠压故障。WY*/
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

	/*A相电流有效值过载故障。WY*/
	if ((gridCurA_rms > OUT_OVER_LOAD) //A相电网电流有效值 > 电流有效值过载上限。WY
			|| (gridCurrBYAF_rms > OUT_OVER_LOAD)) //A相旁路电流有效值 > 电流有效值过载上限。WY
	{
		ESCFlagA.FAULTCONFIRFlag = 1; //A相存在故障信号。WY

		if (softwareFaultWord2.B.ESCOverLoadFlagA == 0) //不存在A相输出过载故障。WY
		{
			ESCFlagA.ESCCntMs.StartDelay = 0;
			softwareFaultWord2.B.ESCOverLoadFlagA = FaultDetect(SOE_GP_FAULT + 17, LCNT_LOAD_OVER_CUR_A, 1510, ESCFlagA.PHASE); //产生A相输出过载故障。WY
		}
	}
	else
	{
		SetFaultDelayCounter(LCNT_LOAD_OVER_CUR_A, 2); //清零：A相输出过载故障信号次数。WY
	}

	/*B相电流有效值过载故障。WY*/
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

	/*C相电流有效值过载故障。WY*/
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

	/*A相电流有效值过流故障。WY*/
	if ((gridCurA_rms > OUTCUR_OVER_RMS) //A相电网电流有效值 > 电流有效值过流上限。WY
			|| (gridCurrBYAF_rms > OUTCUR_OVER_RMS)) //A相旁路电流有效值 > 电流有效值过流上限。WY
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

	/*B相电流有效值过流故障。WY*/
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

	/*C相电流有效值过流故障。WY*/
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

	/*A相旁路磁保持继电器未合闸故障。WY*/
	if ((fabs(VoltInA_rms - VoltOutA_rms) > 20) // |A相电网电压有效值 - A相负载电压有效值| > 20。WY
			&& (StateEventFlag_A == STATE_EVENT_WAIT_A)) //A相处于待机状态。WY
	{
		ESCFlagA.FAULTCONFIRFlag = 1; //A相存在故障信号。WY

		if (softwareFaultWord3.B.ESCBYRelayFaultA == 0) //不存在A相旁路磁保持继电器未合闸故障。WY
		{
			ESCFlagA.ESCCntMs.StartDelay = 0;
			softwareFaultWord3.B.ESCBYRelayFaultA = BYRelayFaultDetect(SOE_GP_FAULT + 45, CNT_BYRelay_A, 2, ESCFlagA.PHASE); //产生A相旁路磁保持继电器未合闸故障。WY
			if (softwareFaultWord3.B.ESCBYRelayFaultA == 1)
			{
				ESCBYRelayCNTA = 1; //A相旁路磁保持继电器状态异常。WY
			}
		}
	}
	else
	{
		SetFaultDelayCounter(CNT_BYRelay_A, 0); //清零：A相旁路磁保持继电器故障信号次数。WY
	}

	/*B相旁路磁保持继电器未合闸故障。WY*/
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

	/*C相旁路磁保持继电器未合闸故障。WY*/
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

	/*电网过频故障。WY*/
	if ((VoltInA_rms > 15) //A相电网电压有效值 > 15。WY
			|| (VoltInB_rms > 15)  //B相电网电压有效值 > 15。WY
			|| (VoltInC_rms > 15)) //C相电网电压有效值 > 15。WY
	{
		if (GridFreq >= GF_OVER) //电网频率 > 电网频率上限值。WY
		{
			ESCFlagA.FAULTCONFIRFlag = 1; //A相存在故障信号。WY
			ESCFlagB.FAULTCONFIRFlag = 1; //B相存在故障信号。WY
			ESCFlagC.FAULTCONFIRFlag = 1; //C相存在故障信号。WY

			if (softwareFaultWord3.B.ESCGridOverFreqenceFlag == 0) //不存在电网过频故障。WY
			{
				ESCFlagA.ESCCntMs.StartDelay = 0;
				ESCFlagB.ESCCntMs.StartDelay = 0;
				ESCFlagC.ESCCntMs.StartDelay = 0;
				softwareFaultWord3.B.ESCGridOverFreqenceFlag = THREEPHASEFaultDetect(SOE_GP_FAULT + 35, CNT_OVER_FREQ, 2000); //存在电网过频故障。WY
			}
		}
		else
		{
			SetFaultDelayCounter(CNT_OVER_FREQ, 1); //清零：电网过频故障信号次数。WY
		}
	}

	/*电网欠频故障。WY*/
	if ((VoltInA_rms > 15) //A相电网电压有效值 > 15。WY
			|| (VoltInB_rms > 15) //B相电网电压有效值 > 15。WY
			|| (VoltInC_rms > 15)) //C相电网电压有效值 > 15。WY
	{
		if (GridFreq <= GF_UNDER) //电网频率 < 电网频率下限值。WY
		{
			ESCFlagA.FAULTCONFIRFlag = 1; //A相存在故障信号。WY
			ESCFlagB.FAULTCONFIRFlag = 1; //B相存在故障信号。WY
			ESCFlagC.FAULTCONFIRFlag = 1; //C相存在故障信号。WY

			if (softwareFaultWord3.B.ESCGridUnderFreqenceFlag == 0) //不存在电网欠频故障。WY
			{
				ESCFlagA.ESCCntMs.StartDelay = 0;
				ESCFlagB.ESCCntMs.StartDelay = 0;
				ESCFlagC.ESCCntMs.StartDelay = 0;
				softwareFaultWord3.B.ESCGridUnderFreqenceFlag = THREEPHASEFaultDetect(SOE_GP_FAULT + 36, CNT_UNDER_FREQ, 4000); //存在电网欠频故障。WY
			}
		}
		else
		{
			SetFaultDelayCounter(CNT_UNDER_FREQ, 0); //清零：电网欠频故障信号次数。WY
		}
	}

	/*出风口散热片过温故障。WY*/
	if (TempData[0] > WindCold.BOARD_OVER_TEMP) //出风口散热片温度 > 出风口散热片温度上限。WY
	{
		ESCFlagA.FAULTCONFIRFlag = 1; //A相存在故障信号。WY
		ESCFlagB.FAULTCONFIRFlag = 1; //B相存在故障信号。WY
		ESCFlagC.FAULTCONFIRFlag = 1; //C相存在故障信号。WY

		if (softwareFaultWord2.B.ESCOverTemperatureFlagB == 0) //不存在出风口散热片过温故障。WY
		{
			ESCFlagA.ESCCntMs.StartDelay = 0;
			ESCFlagB.ESCCntMs.StartDelay = 0;
			ESCFlagC.ESCCntMs.StartDelay = 0;

			softwareFaultWord2.B.ESCOverTemperatureFlagB = THREEPHASEFaultDetect(SOE_GP_FAULT + 24, CNT_SET_OVER_TEMP_B, 20); //存在出风口散热片过温故障。WY
		}
	}
	else
	{
		SetFaultDelayCounter(CNT_SET_OVER_TEMP_B, 1); //清零：出风口散热片过温故障信号次数。WY
	}

	/*单元外壳过温故障。WY*/
	if (TempData[1] > WindCold.UNIT_OVER_TEMP) //单元外壳温度 > 单元外壳温度上限。WY
	{
		ESCFlagA.FAULTCONFIRFlag = 1; //A相存在故障信号。WY
		ESCFlagB.FAULTCONFIRFlag = 1; //B相存在故障信号。WY
		ESCFlagC.FAULTCONFIRFlag = 1; //C相存在故障信号。WY

		if (softwareFaultWord2.B.ESCChassisOverTempFlag == 0) //不存在单元外壳过温故障。WY
		{
			ESCFlagA.ESCCntMs.StartDelay = 0;
			ESCFlagB.ESCCntMs.StartDelay = 0;
			ESCFlagC.ESCCntMs.StartDelay = 0;

			softwareFaultWord2.B.ESCChassisOverTempFlag = THREEPHASEFaultDetect(SOE_GP_FAULT + 30, CNT_SET_BOARD_OVER_TEMP, 20); //存在单元外壳过温故障。WY
		}
	}
	else
	{
		SetFaultDelayCounter(CNT_SET_BOARD_OVER_TEMP, 1); //清零：单元外壳过温故障。WY
	}

	/*防雷故障。WY*/
	if (windColdCtr != 0) //未屏蔽防雷故障检测。WY
	{
		if (GET_FLback == 1) //存在防雷故障。WY
		{
			ESCFlagA.FAULTCONFIRFlag = 1; //A相存在故障信号。WY
			ESCFlagB.FAULTCONFIRFlag = 1; //B相存在故障信号。WY
			ESCFlagC.FAULTCONFIRFlag = 1; //C相存在故障信号。WY

			if (softwareFaultWord2.B.ESCFangLeiFaultFlag == 0) //不存在防雷故障。WY
			{
				ESCFlagA.ESCCntMs.StartDelay = 0;
				ESCFlagB.ESCCntMs.StartDelay = 0;
				ESCFlagC.ESCCntMs.StartDelay = 0;

				softwareFaultWord2.B.ESCFangLeiFaultFlag = THREEPHASEFaultDetect(SOE_GP_FAULT + 26, CNT_FANGLEI_FAULT, 20); //存在防雷故障。WY
			}
		}
		else
		{
			SetFaultDelayCounter(CNT_FANGLEI_FAULT, 0); //清零：防雷故障。WY
		}
	}

	ZeroOffsetJudgment();// 定标算法的执行时间为 888.0*0.006667 =5.920296

//--------------- 谐振 -------------------//
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
			CntMs.HFOverCurrentCount -= 2;//反时限恢复
		}
		else
		{
			CntMs.HFOverCurrentCount = 0;
			StateFlag.HFOverCurrentFlag = 0;//P系数恢复为1倍模式
		}
	}
}

/*
 * 功能：在复位设备时，复位故障标志位。WY
 */
void FaultReset(void)
{
	if (((ESCFlagA.faultFlag == 0) //A相不存在故障。WY
			|| (ESCFlagB.faultFlag == 0) //B相不存在故障。WY
			|| (ESCFlagC.faultFlag == 0)) //C相不存在故障。WY
			&& (StateFlag.resetFaultOnceTime == 0)) //此标志位无意义，且恒为0。WY
	{
		softwareFaultWord1.all = 0; //复位故障标志位。WY
		softwareFaultWord2.all = 0; //复位故障标志位。WY
		softwareFaultWord3.all = 0; //复位故障标志位。WY
		softwareFaultWord4.all = 0; //复位故障标志位。WY
		softwareFaultWord5.all = 0; //复位故障标志位。WY
	}
}

/*
 * 相序检测函数
 * 第一种方法：PLL锁定了A相电角度，将angleA-pi/3，滞后了PI/3，再根据电压D轴还原A相电压
 * 这样C相应该和A相反向，相加和可以约等于零。
 * 若相序不一致，是得不到这个结果的。所以可以用来判断相序的正确性。
 */
/*
 * 第二种办法：计算alpha和beta的角度，如果是正序，则角度为周期递增，
 * 若相序错位，则角度为周期递减。
 * 这样只需要比较计算的角度前后两次值的差，正值为正序，负值为乱序。
 */
// 因计算费时，只能在启动前进行检测，其它情况不做运算

void GridPhaseSequTest(Uint16 i)
{
	static float theta_c;
	static float theta_c_Prior;

	float a = 0, b = 0, tmp1 = 0;
	if (StateEventFlag_A != STATE_EVENT_RUN_A)
	{
		a = GridVoltAlpha;
		b = GridVoltBeta;

		theta_c = atan2(a, b);// ------- 采用rts2800_fpu32.lib    计算时间最多不超过  4us
// ------- 但是采用浮点快速运算表后，计算时间最多不超过   0.6us
		if ((a > 0) && (b < 0))
			theta_c = -theta_c + 4.7123889;// 3/2*pi;
		else if ((a > 0) && (b > 0))
			theta_c = -theta_c + 1.5707963;//pi/2;
		else if ((a < 0) && (b > 0))
			theta_c = -theta_c + 1.5707963;//pi/2;
		else if ((a < 0) && (b < 0))
			theta_c = -theta_c + 4.7123889;// 3/2*pi;

		if ((theta_c > 1) && (theta_c < 6))// 避开过零区域
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
