/*
 * VariblesDef.h
 *
 *  Created on: 2013-4-9
 *      Author: Administrator
 *  待优化：相干的变量形成结构体来调用，方便寻找和模块化，结构体避免变量过多；
 *  应对结构体的命名尽可能详细。对结构体对象可简述命名，减少阅读的复杂性。
 */

#ifndef VARIBLESDEF_H_
#define VARIBLESDEF_H_

#include "DSP28x_Project.h"

#pragma DATA_SECTION(outputCurRatio,            "CLADataLS01");
#pragma DATA_SECTION(outputCurRatioCurrA,       "CLADataLS01");
#pragma DATA_SECTION(outputCurRatioCurrB,       "CLADataLS01");
#pragma DATA_SECTION(outputCurRatioCurrC,       "CLADataLS01");
#pragma DATA_SECTION(negCurCompPerc,            "CLADataLS01");
#pragma DATA_SECTION(ESC_DutyDataA,             "CLADataLS01");
#pragma DATA_SECTION(ESC_DutyDataB,             "CLADataLS01");
#pragma DATA_SECTION(ESC_DutyDataC,             "CLADataLS01");
#pragma DATA_SECTION(PWMcurrDirFlagA,           "CLADataLS01");
#pragma DATA_SECTION(PWMcurrDirFlagB,           "CLADataLS01");
#pragma DATA_SECTION(PWMcurrDirFlagC,           "CLADataLS01");
#pragma DATA_SECTION(dbg_claWave,               "CLADataLS01");
#pragma DATA_SECTION(dbg_claPwmReg,             "CLADataLS01");

#pragma DATA_SECTION(VoltInAF,               "CLADataLS01");
#pragma DATA_SECTION(VoltInBF,               "CLADataLS01");
#pragma DATA_SECTION(VoltInCF,               "CLADataLS01");

#pragma DATA_SECTION(tempbuff,             "CLADataLS01");

float TIMER1 = 0, TIMER2 = 0, TIMER3 = 0, TIMER4 = 0, TIMERPERIODVAL1 = 0, TIMERPERIODVAL2 = 0, PERIODVAL1 = 0,
		PERIODVAL2 = 0;

int CAPTIEMRFlag = 0;

float ESC_DutyDataA = 1.0f,ESC_DutyDataB = 1.0f,ESC_DutyDataC = 1.0f;
float OUTCUR_OVER_INS	= 300;	// peak : 144.342*1.414 * 1.3 = 265
float OUTCUR_OVER_INS_NEG= -300;
float OUTCUR_OVER_RMS	= 174;	// RMS  : 115*1.2 = 173
float OUTCUR_FAULTCONTACT = 30;
float OUT_OVER_LOAD		= 130;	// RMS  : 115*1.1 = 158
float OutCurMaxLimit	= 200;	// Qpeak: 115*1.732=250
float OutCurMaxLimitNeg	= 200;	// Qpeak: 115*1.732=250
float reactPowGivenLimit = 80 ;
float REACTPOWERLIMIT = 0;
float REACTPOWERLIMITPH = 0;
float PosReactPowGivenLimit;
float NegReactPowGivenLimit;
float WholeOutCurRmsLimit = 115.0f*115.0f; // 谐波电流有效值50A

float CurrRefDCorr=1.0f;
float HarmLimtCorr=1.0f;
float CapaReactCorr=1.0f;
float CapaReactCorrA = 1.0f;
float CapaReactCorrB = 1.0f;
float CapaReactCorrC = 1.0f;
float CapaUnbalCorr=1.0f;
Uint16 OutCurATims = 0;
Uint16 OutCurBTims = 0;
Uint16 OutCurCTims = 0;
Uint16 ContactTims = 0;
Uint16 ChargeVoltage = 2;
#pragma DATA_SECTION(T1PR,            "CLADataLS01");
#if PWM_FREQUENCE_8KHZ
float T1PR = 4688;
#elif PWM_FREQUENCE_9p6KHZ
float T1PR = 3906;
#elif PWM_FREQUENCE_12p8KHZ
float T1PR = 3906;
#elif PWM_FREQUENCE_16KHZ
float T1PR = 1250;
#endif
float PWM_PERIOD_MAX = 2334;  	//
float PWM_PERIOD_MIN = 2444;  	//
float PIlim_I = 2984;   		//  12K   2930*0.98/0.866*0.9 = 2984
float FUNT1PR = 1000;
float quarterPeriod = 0;  // spwm /2= 3750/2=1875
float GridVoltAF = 0;
float GridVoltBF = 0;
float GridVoltCF = 0;

float DccapVoltA = 0; //A相直流电容电压。WY

float DccapVoltB = 0; //B相直流电容电压。WY

float DccapVoltC = 0; //C相直流电容电压。WY

float GridBPCurrAF = 0;
float GridBPCurrBF = 0;
float GridBPCurrCF = 0;
float GridVoltAF_Beta = 0;
float GridVoltBF_Beta = 0;
float GridVoltCF_Beta = 0;
float LoadVoltUF_Beta = 0;
float LoadVoltVF_Beta = 0;
float LoadVoltWF_Beta = 0;
float LoadVoltUF = 0;
float LoadVoltVF = 0;
float LoadVoltWF = 0;
float VoltInAF = 0;
float VoltInBF = 0;
float VoltInCF = 0;
float VoltInAF_Beta = 0;
float VoltInBF_Beta = 0;
float VoltInCF_Beta = 0;
float VoltOutAF = 0;
float VoltOutBF = 0;
float VoltOutCF = 0;
float VoltOutAF_Beta = 0;
float VoltOutBF_Beta = 0;
float VoltOutCF_Beta = 0;
float GridCurrAF = 0;
float GridCurrBF = 0;
float GridCurrCF = 0;
float GridVoltAlpha = 0;
float GridVoltBeta = 0;
float GridVoltRms = 0;
float GridVoltRms_F = 0;
float LoadVoltRms = 0;
float LoadVoltRms_F = 0;
//float GridPLLVoltD=0.1;
//float GridPLLVoltQ=0.2;
//float GridPLLVoltDn=0.01;
//float GridPLLVoltQn=0.02;
float GridFreq = 50;
float GridVoltTheta = 0;
float GridCurrTheta = 0;
//float VoltThetaB = 0;
//float VoltThetaC = 0;
float VoltTheta2 = 0;
float VoltCurrCalibrPhase;
float LoadCurrCalibrPhase;
float GridCurrCalibrPhase;
float InvCurrCalibrPhase;
float ThreeHarmGridVoltRms = 0;
float ThreeResSin =0;
float GridCurrZA = 0;
float GridCurrZB = 0;
float GridCurrZC = 0;
float GridCurrA = 0,GridCurrB = 0,GridCurrC = 0;
float GridCurrHvA=0,GridCurrHvB=0,GridCurrHvC=0;
float LoadRealCurHvA=0,LoadRealCurHvB=0,LoadRealCurHvC=0;
float GridZeroCur = 0;
float GridCurRms = 0;
float GridCurRms_F = 0;
float GridCurBYRms = 0;
float GridCurBYRms_F = 0;
float CurrRefRms = 0;
float CurrRefRms_F = 0;
float GridFundaCurD=0;
float GridFundaCurQ=0;
float GridFundaCurND=0;
float GridFundaCurNQ=0;
float Rec_sa = 0;
float Rec_sb = 0;
float Rec_sc = 0;
float LoadRealCurA = 0;
float LoadRealCurB = 0;
float LoadRealCurC = 0;
float LoadRealCurAF = 0;
float LoadRealCurBF = 0;
float LoadRealCurCF = 0;
float LoadRealCurZA = 0;
float LoadRealCurZB = 0;
float LoadRealCurZC = 0;
float LoadRealCurZ = 0;
float LoadCurRms = 0;
float LoadCurRms_F = 0;
float LoadFundaCurZ=0;
float NegCurCompZ=0;
float LoadFundaCurD=0;
float LoadFundaCurQ=0;
float LoadFundaCurND=0;
float LoadFundaCurNQ=0;
float ApfOutCurA = 0;
float ApfOutCurB = 0;
float ApfOutCurC = 0;
float ApfOutCurAD = 0;
float ApfOutCurBD = 0;
float ApfOutCurCD = 0;
float ApfOutCurZA = 0;
float ApfOutCurZB = 0;
float ApfOutCurZC = 0;
float ApfOutCurZ = 0;
float ApfOutCurZ_F = 0;
float InvFundaCurD = 0;
float InvFundaCurQ = 0;
float InvFundaCurND = 0;
float InvFundaCurNQ = 0;
float GridVoltDA=0;
float GridVoltQA=0;
float GridVoltDB=0;
float GridVoltQB=0;
float GridVoltDC=0;
float GridVoltQC=0;
float GridRealCurDA=0;
float GridRealCurQA=0;
float GridRealCurDB=0;
float GridRealCurQB=0;
float GridRealCurDC=0;
float GridRealCurQC=0;
float LoadRealCurDA=0;
float LoadRealCurQA=0;
float LoadRealCurDB=0;
float LoadRealCurQB=0;
float LoadRealCurDC=0;
float LoadRealCurQC=0;
float IverRealCurDA = 0;
float IverRealCurDB = 0;
float IverRealCurDC = 0;
float IverRealCurQA = 0;
float IverRealCurQB = 0;
float IverRealCurQC = 0;
float ApfOutCurRms = 0;
float ApfOutCurRms_F = 0;
float dcVoltF = 0;
float PreviousDcVoltF = 0;
float dcVoltUp = 0;
float dcVoltUpF = 0;
float dcVoltDn = 0;
float dcVoltDnF = 0;
float GridActPower = 0;
float GridReactPower = 0 ;
float GridApparentPower = 0;
float GridActPowerPhA=0;
float GridReactPowerPhA=0;
float GridApparentPowerPhA=0;
float GridCosFiPhA=0;
float GridActPowerPhB=0;
float GridReactPowerPhB=0;
float GridApparentPowerPhB=0;
float GridCosFiPhB=0;
float GridActPowerPhC=0;
float GridReactPowerPhC=0;
float GridApparentPowerPhC=0;
float GridCosFiPhC=0;

//float Voltuplimit=0;
//float Voltlowlimit=0;
//float Voltoverlimitcompentarvalue=0;
//float Voltunderlimitcompentarvalue=0;

float Esc_CurPhaseA = 0,Esc_CurPhaseB = 0,Esc_CurPhaseC = 0;
//float Esc_VoltPhaseA = 0,Esc_VoltPhaseB = 0,Esc_VoltPhaseC = 0;
//int FunContDelayFlag = 0;
int ArithFlagA = 0,ArithFlagB = 0,ArithFlagC = 0;
int TestArithFlagA = 1,TestArithFlagB = 1,TestArithFlagC = 1;
Uint16 GridcurrCnt = 0;

int PeakStopFlagA = 0,PeakStopFlagB = 0,PeakStopFlagC = 0;

float LoadActPowerPhA=0;
float LoadReactPowerPhA=0;
float LoadApparentPowerPhA=0;
float LoadCosFiPhA=0;
float LoadActPowerPhB=0;
float LoadReactPowerPhB=0;
float LoadApparentPowerPhB=0;
float LoadCosFiPhB=0;
float LoadActPowerPhC=0;
float LoadReactPowerPhC=0;
float LoadApparentPowerPhC=0;
float LoadCosFiPhC=0;
float MinVolRms = 0;
float MinLoadCosFi = 0;
float LoadCurRmsUnban = 0;
float GridCurRmsUnban = 0;
float AllCapCapacityPhA=0,AllCapCapacityPhB=0,AllCapCapacityPhC=0;
float RUICA=0,RUICB=0,RUICC=0;

int16 MeanPos=0,MeanPos_F=0;
int16 VoltPos=0,VoltPrvPos=0;
int CapVoltPos=0,CapVoltPosCur=0;
int CurrThisPos =0,CurrPrvPos=(MEANPOINT_QUARTER_NUM-MEANPOINT_QUARTER);
float CSReactPowerave = 0;
float GridCosFi = 0;
float TransformVoltH2L=1,TransformVoltL2H=1,TransformCurrL2H=1,TransformCurrH2L=1;
float PrvFreq=50;
Uint16 cntSyncSucc = 0;
Uint16 WaveRecordHandler=0;

float LoadFundaCurZRms,CurFundNgARms,CurFundNgBRms,CurFundNgCRms;
float CurFundNgA,CurFundNgB,CurFundNgC;
float CurrHarmRefZ,ApfOutCurZRms,LoadRealCurZRms;
float CurrRefARms,CurrRefBRms,CurrRefCRms;
float WholeRmsLimit,WholeZRmsLimit,WholeRmsLimitPH;
float ResonProtcABC;
float ResonProtcABCRms;
// 有效值的计算变量
float VoltGivenStart = 0;
float VoltInA_rms = 0;
float VoltInB_rms = 0;
float VoltInC_rms = 0;
float VoltOutA_rms = 0;
float VoltOutB_rms = 0;
float VoltOutC_rms = 0;
float apfOutCurA_rms = 0;
float apfOutCurB_rms = 0;
float apfOutCurC_rms = 0;
float loadCurA_rms = 0;
float loadCurB_rms = 0;
float loadCurC_rms = 0;
float gridCurA_rms = 0;
float gridCurB_rms = 0;
float gridCurC_rms = 0;
float gridCurrBYAF_rms = 0;
float gridCurrBYBF_rms = 0;
float gridCurrBYCF_rms = 0;
int cntGridVoltPhaseSeqFault = 0;
float TotalHarmDistorionGridCurr,TotalHarmDistorionLoadCurr,TotalHarmDistorionVolt;
int16 EscStandyDelayCnt = 0;

int CTRL24_POWERFlag = 0;

Uint16 windColdCtr;
int EscStandyDelayCnta1 = 0,EscStandyDelayCntb1 = 0,EscStandyDelayCntc1 = 0,EscStandyDelayCnta2 = 0,EscStandyDelayCntb2 = 0,EscStandyDelayCntc2 = 0;
extern int StateSkipFlagA = 0,StateSkipFlagB = 0,StateSkipFlagC = 0;
int GV_RMS_UNDER_PowerOnForRecharge;
//计数器使用，用于时间计数等
int RecordWaveFlag = 0;
int CurrentProperty = 0;
int SubordinateStatus = 0;
int MatchCondition = 0;
int StandbyRecoveryTime = 0;
int Mode_PP = 0;
float AutoJudgeRms_F = 0;
float AutoStartCurr = 0;
float AutoWaitCurr = 0;
float AutoStartMode = 1;
float AutoModeCurr = 0;
float R_SSwitchTime = 0;
float StandCosFiLimit = 0;
float StandUbanCurPer = 0;
int16 LowPowerTimCnt=0;
int CntFor100us = 0;
int CntFor300us = 0;
float manualubanlanccurA = 0;
float manualubanlanccurB = 0;
float manualubanlanccurC = 0;
extern int cntGridRMSOverVolt_RlyProt = 0;
volatile float theta_c = 0;
volatile float theta_c_Prior = 0;
Uint16 SCIBMASTERENABLE = 0;
Uint16 SCICMASTERENABLE = 0;
Uint16 SCIDMASTERENABLE = 0;
//  记录调试数据使用
#pragma DATA_SECTION(debugW, "WAVEDATA1")
float debugW[DEBUG_WAVE_GROUP][DEBUG_WAVE_LEN];          //用于观测 基波、输出谐波
int16 CntForRec=0;
//
// 采样和滤波参数定义零偏定义
#pragma DATA_SECTION(tmpOffsetValue, "EBSS1")
float tmpOffsetValue[18]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
long GridCurrPrvAD[3];
long GridCurrPrvADFlag=0;
int adcAutoCount = 0;

float dcBusVolt = 0.1;         // 采样值
float dcBusVoltGiven = 700;  // SPWM直流侧给定最小值
float kp_Dc_Div =  0.5; // 实验得到 15   --- 9.0721;   //软起可以放在20分之一
float ki_Dc_Div = 0.005;
float kp_Dc = 1.2;//计算的值可以用   3.9，波形会震荡。取小于它的值 0918  // 2.34375;
float ki_Dc = 0.003;/// 0.1;
float currentRefD = 0;
float currentRefQ = 0;
float currentRefQa = 0;
float currentRefQb = 0;
float currentRefQc = 0;
float currentRefA = 0;
float currentRefB = 0;
float currentRefC = 0;
float GpVoltA_rmsReciprocal = 0;
float GpVoltB_rmsReciprocal = 0;
float GpVoltC_rmsReciprocal = 0;
float kp_CurA = 6;
float kp_CurB = 6;
float kp_CurC = 6;
float kp_Cur = 6;//4.0;(16K)    // 4.5;     //7.9743;//5.3162;//0.26581;    //  3.320313;   // 3.320313   0.33203125
float ki_Cur = 0.005; //0.005;//0.005; //0.05;

float ReactLoadCompCurQ = 0;
float ReactLoadCompCurA = 0;
float ReactLoadCompCurB = 0;
float ReactLoadCompCurC = 0;
float reactPowerCompCurQ = 0;
float CapreactPowerCurQ=0;
float ReactPowerCurQPA = 0;
float ReactPowerCurQPB = 0;
float ReactPowerCurQPC = 0;
float CapReactPowerCurQPA = 0;
float CapReactPowerCurQPB = 0;
float CapReactPowerCurQPC = 0;
float reactPowerCompCurQa = 0;
float reactPowerCompCurQb = 0;
float reactPowerCompCurQc = 0;

float GridResSin = 0.01 ;
float GridResCos = 0.02 ;

volatile int StateEventFlag_A = STATE_EVENT_STANDBY_A; //A相状态机事件。WY
volatile int StateEventFlag_B = STATE_EVENT_STANDBY_B; //B相状态机事件。WY
volatile int StateEventFlag_C = STATE_EVENT_STANDBY_C; //C相状态机事件。WY

float PIlim_Udc = 120;
float I_ins_index = 0;
int PWM_ins_indexA = 0,PWM_ins_indexB = 0,PWM_ins_indexC = 0;
//int DisPWM_ins_index = 0;
float reactPowerGiven = 0;
float constantCosFai = 1;
float PFConsin = 0;
float PWMcurrDirFlagA,PWMcurrDirFlagB,PWMcurrDirFlagC;
int ESCHighLowRelayCNTA = 0,ESCHighLowRelayCNTB = 0,ESCHighLowRelayCNTC = 0;

int ESCSicFaultCNTA = 0; //A相SiC管状态标志位。0，正常；1，异常。WY
int ESCSicFaultCNTB = 0; //B相SiC管状态标志位。0，正常；1，异常。WY
int ESCSicFaultCNTC = 0; //C相SiC管状态标志位。0，正常；1，异常。WY

int ESCBYRelayCNTA = 0; //A相旁路磁保持继电器状态标志位。0，正常（上电默认）；1，异常。WY
int ESCBYRelayCNTB = 0; //B相旁路磁保持继电器状态标志位。0，正常（上电默认）；1，异常。WY
int ESCBYRelayCNTC = 0; //C相旁路磁保持继电器状态标志位。0，正常（上电默认）；1，异常。WY

int TurnOffPeakFaultFlagA = 0,TurnOffPeakFaultFlagB = 0,TurnOffPeakFaultFlagC = 0;
long tempbuff = 0;
float ArithVAL=3,ArithVal=3;

float dbg_claWave[6];
float dbg_claPwmReg[8];

//float VoltupLIMIT = 0;
//float VoltlowLIMIT = 0;
//float VoltoverlimitcompentarVALUE = 0;
//float VoltunderlimitcompentarVALUE = 0;

float VolttargetA=0,VolttargetB=0,VolttargetC=0,VoltThreSholdA=0,VoltThreSholdB=0,VoltThreSholdC=0,Backdiffvalue=0;

int16 RemoteReactpowerOrder=0;
Uint16 RemoteStateRunStop=0;
Uint16 RemoteStateWait=0;
Uint16 RemoteStateFault=0;
//PWM输出
float PwmVa=0,PwmVb=0,PwmVc=0;
float PwmVaN=0,PwmVbN=0,PwmVcN=0;
int OverTempCnt = 0,OverBoardCnt=0,OverLoadCnt=0,OverVoltCnt=0,UnderVoltCnt=0;

//float Q2D_ratio =   0; //4.71239 = wL= 2*pi*50*0.00015=  0.047124
//int Q2D_ratio = 0;
int PhaseControl = 0;
//float DeadAdjust = 0;
float LoadCurlimit = 0; //
float Curr_MinCtrlFAN = 20;
float Curr_MaxCtrlFAN = 60;
float Temp_MinCtrlFAN = 40;
float Temp_MaxCtrlFAN = 60;
float FanStartMinDUTY = 0.4;

float RateLoadCurrLimit = 0; //负载额定电流限定值。WY

int CapacitySelection = 0;
float SyncHardwareLead=1100;
float SampleLead=100;
float GV_RMS_OVER_RLY_PROT = 460;
float GV_INS_OVER =  800 ; // 400*1.15*1.414*1.05 = 682.962
float GV_RMS_OVER  = 460;  // 400*1.15 = 460
float GV_RMS_UNDER = 340;  // 400*0.85 = 340   试验设定20
float GF_OVER      = 63 ;  // 50*1.1 = 55
float GF_UNDER     = 40 ;  // 50*0.9 = 45

float ESCDCVOLITLIMIT = 800; //直流电容电压上限值。WY  // 380*1.414*1.15 + 100 = 718

float VoltFallResponseLimit    = 200;  // 380*1.414*0.8 = 456    欠压值过长可能导致逆变器失控 设置为0.8倍
float GV_NEGTIVE_UP     = 60;  // 暂定不对称50% ；待定？？？ 装置在何种不对称电网工况下，无法连续运行，则设定为故障位
float VoltHarmOver = 0;
float DC_ERR_LIMIT  =  50;
float TempData[10];
float CTLFANDATA[10];
float CURRData[10];
float UnCurrData[3];
float UNCurDiffData[10];
float CurrentUnbalanceRegularVoltage[3];
int VoltageOverflow[3]={0,0,0};
int centreGravity=0;
float centreGravityValue=0;
float currentUnbalance;
#endif
Uint16 PIAutoJust=0;
int TempEnvirProvalue=0;
float ESCTESTDATA1,ESCTESTDATA2,ESCTESTDATA3,ESCTESTDATA4;
Uint16 SendOrderCount;
float MU_LCD_RATIO=1.0f,MU_MultRatio=1.0f;
int MU_ALLCOUNT=1,PWM_address = 0,APF_ID = 0x10;
Uint16 CapEmergTim = 0;
float storageUsed = 0;   // 已用容量
float GridNeutralCurRms = 0;  // 电网中线电流
float harmCompPerc;
int VolUpCo = 400;
int VolDownCo = 360;
int VolUpSurCo = 390;
int VolDownSurCo = 370;
int VolSurTime = 0;

int StableValueA=0,StableValueB=0,StableValueC=0;

int RechageskipstopFlag = 0;
int ESCtestVAL1 = 0,ESCtestVAL2 = 0;
float dutytmpA1,dutytmpA2,dutytmpB1,dutytmpB2,dutytmpC1,dutytmpC2;

float dutytmp,dutytmp1,TESEINSGridIn,TESETarget,TESEPIerr,testvalue1,FeedVAl,DEBUGduty,DEBUGData,PhaseValue,TESTRMSGridInVAL,NumeratorValue,DenominatorVAL;
float VolttargetCorrA,VolttargetCorrB,VolttargetCorrC;
int ConstantCurrInsFlagA = 0,ConstantCurrInsFlagB = 0,ConstantCurrInsFlagC = 0;
float CurrTargetTemper;
float VolPIOutPwmVa = 1;
float VolPIOutPwmVb = 1;
float VolPIOutPwmVc = 1;
float PIVAL = 3.141593f;
float VolCor = 100;
float VolCorU = 100;
float VolCorV = 100;
float VolCorW = 100;
float ModeSwitchingCurrent = 0;
float OutputCurrent = 0;
float reactPrCompPerc = 1;
float restantReactCurrent = 0;
float transfRatio = 100;   // 下发参数为X:5   600/5= 120
float TargetCurrentUnbalance;
float ConstantReactivePower;
float transfRatioVirtu = 100;
float dcCapVoltRatio = 0.293;    //200欧 600-2048 0.293少1.5倍
float loadVoltRatio = 0.293;
float gridVoltRatio = 0.4395;    //
float outputCurRatio =0.061;     //12.2欧  50A--1707
float bypassCurrRatio = 0.061;
float outputCurRatioCurrA = 0.167;
float outputCurRatioCurrB = 0.167;
float outputCurRatioCurrC = 0.167;
float outputCurBypassCurrA = 0.167;
float outputCurBypassCurrB = 0.167;
float outputCurBypassCurrC = 0.167;
float DcCapVoltRatioVirtu = 0.293;
float dcVoltUpRatioVirtu_reciprocal =1;
float loadVoltRatioVirtu = 0.293;
float gridVoltRatioVirtu = 0.4395;
float outputCurRatioVirtu = 0.061;
float bypassCurrRatioVirtu = 0.061;
float loadCurRatio =0.13;		//0.00325*40=0.13(200A:5A)
//float gridCurRatio =0.13;
volatile int cntForSpecCstop = 0;
volatile int cntForStartDelay = 0;

float dcVoltDiv2Err = 0;
float DCtestMirr = 0;
float NegCurLimit = 63;
float S_RSwitchTime = 0;
float TransformRatioVal = 1;		//高压侧变压器变比值,由TrasformRatioParall计算得出
int forwardPoints = 100;
float capTsctr = 0;//捕获数值
int cntForSyncInit = 0;//同步初始化
Uint16 cntForSyncErr  = 0;
int syncState = 0;//同步状态机
float pwmTsGiven = 4688;//捕获PI的给定
float POINT_CAP1 = 0;//信号错分
float MAX_CAP1 = 0;
float MIN_CAP1 = 0;
float syncErr = 0;//捕获PI控制器
float syncErrPrior = 0;
Uint16 pwmTBPRD = 0;
Uint16 PrvPwmTBPRD=0;
Uint16 RemoteAddress=0xFF;
float syncErrLimit = 1000;
int cntForSyncOut = 0;// 发送翻转电平
float outCurTempLimit = 0;
float outCurSinkTempLimit = 0;
float outCurBoardTempLimit = 0;
Uint16 DropLetMarkFlag = 0;
int cntForCurMax = 0;
int cntForAutoStIn30 = 0;
int cntForRepFaultA = 0;
int cntForRepFaultB = 0;
int cntForRepFaultC = 0;
float negCurCompD = 0;
float negCurCompQ = 0;
float negCurCompPerc = 1;
float THD_K = 1;
float THD_B = 0;
float PreviousDcVoltFTo2 = 0;
float UnderVoltageLimit = 0.8;
float32 UdcBalanceCurr;
float32 AutoCurrSharing = 1;
float dcBusVtInc = 0;

//0,返回整形数  其他,返回浮点数并且original/ratio,original是变量,ratio是变比
//BGMODLE_3P_MEC
#define USER_PARAM_RATIO_3P_MEC_DEFUALT {\
    0,1000,5,0,0,0,\
	10,100,0,0,0,0,0,0,10,10\
}
//#define USER_PARAM_RATIO_3P_APF_DEFUALT {\
//	0,1000,0,1000,0,1000,0,0,-1,1,\
//	100,0,0,0,5\
//}\
//BGMODLE_3P_APF,0x60~6f,0x70~7f,0x80~8d
//value = 下发值*协议当中的倍率/程序当中的倍率//根据结构体来结算倍率
#define FACTORY_PARAM_RATIO_3P_APF_DEFUALT {\
	1,1,1,1,1,0,10,10,10,10,\
	100,10,10,1000,1000,1000,1000,0,0,1,\
	1,1,1000,1,10,1,1,100,10000,100,\
	10000,100,10000,1000,10000,0,0,10,10,1,\
	10,10,10,10,10,10,1000,1000,1000,100,\
	100,100,100,1,1,1,100,100,100,100,\
	100,100,100,1,1,1,1,1,1,1,\
	1,1,10000,10000,10,\
	0,0,0,0,0,0,0,0,0,0,\
	0,0,0,0,0,0,0,0,0\
}//94


#define VARIZEROOFFSETRATIO	{\
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,\
		1000,1000,1000,1000,1000\
}

#define ESCVARIZEROOFFSETRATIO {\
        1,1,1,1,1,1,1,1,1,1,1,1\
}
#define CURRSRC_HMI_RATIO_DEFUALT {\
	0,0,0,0,0,0,0,100,100,10,\
	0,1000,1000,0,0\
}
#define CURRSRC_REMOTE_RATIO_485_DEFUALT {\
	0,0,0,0,0,0,0,0,0,0,\
	0,0,0,0,0,0,100,100,0,0,0\
}
#define FAULT_PARAM_RATIO_3P_CalibCorr_DEFUALT {\
	100,10,10,10\
}
#define FAULT_PARAM_RATIO_3P_GenFFTCoeff_DEFUALT {\
	100,10,10,10,\
}

#if FIXED_CAP_EN
Uni_EnCapSwitch EnCapSwitch;
int CapRunSwitchPower =20;
int CapRunSwitchBack =0;
#endif

float VoltDerivative;
int debugDispFlag = 0;
int16 calcuFlag =-1;
int OverVoltHarm=0;
int VoltDQSource=0;
float NegaQPowerCalib=0;
float NegaDPowerCalib=0;
float ReactivePowerCurrCalib=0;
float ReactivePowerCurrCalibA=1;
float ReactivePowerCurrCalibB=1;
float ReactivePowerCurrCalibC=1;
float ReactivePowerCurrCalibQ=1;
float NegaDPowerCalibD=1;
float NegaQPowerCalibQ=1;
float UbanCompLoadCurND=0;
float UbanCompLoadCurNQ=0;
float ActivePowerCurrCalib=0;
float ActivePowerCurrCalibPH=0;
float PllCalibrPhase = 0 ;
int16 VoltLeadPoints=0;
int16 TestMU_RATIO;
Uint16 SequenceJudgeTimes =0;
Uint16 ReceiveFSM =0;
Uint16 SendFSM =0;
Uint16 SoeRemoteBuf=0;
Uint16 StorageTypeFlag =0;
//Uint32 EleAD[6];
Uint16 GfDebug1 =0;
Uint16 GfDebug2 =0;
Uint16 GfDebug3 =0;
Uint16 GfDebug4 =0;
float SeqJud[3];
float DirJud[3];
float *pPwmV1;
float *pPwmV2;
float *pPwmV3;
float *pPwmVN1;
float *pPwmVN2;
float *pPwmVN3;

int SpuriousIn,SpuriousOut;
//谐波计算
Uint16 RmsCaluBlockCnt = 0;
Uint16 RmsCaluCnt = 0;
float VolforwardA,VolforwardB,VolforwardC;
float APFReactPowerOut=0.0f;
float APFReactPowerOutA=0.0f;
float APFReactPowerOutB=0.0f;
float APFReactPowerOutC=0.0f;


int Duty_cyc_FLAG=0;

Uint16 RandIDH=0;
Uint16 RandIDL=0;
int16  StartDelayTim=0;
Uint16 FlashStartTim=0;
Uint16 RandTim=0,RandTimCount=0;
Uint16 IDDelayReportEnable=0;
Uint16 CapFlagDown=0;				//电容器组投切    标志位
Uint16 SwitchTimeOn=0;          	//投计时
Uint16 SwitchTimeOff=0;          	//切计时
Uint16 TimSwitchOnStart=0;		//投延时计时标志位
Uint16 TimSwitchOffStart=0;		//切延时计时标志位
Uint16 AutoSwitchOnCount=0;		//自动投组数统计
Uint16 AutoSwitchOffCount=0;		//自动切组数统计
Uint16 AutoSwitchOn=0;			//自动投标志
Uint16 AutoSwitchOff=0;			//自动切标志
Uint16 FlashRecordTim = 0;
Uint16 ReFFTDelayTim = 30;
Uint16 CpuHeatDelayTime = 0;
float CapStartCapcity = 0;
float CapStartLimit=0;
Uint16 CapCount=0,CapCountA=0,CapCountB=0,CapCountC=0;
Uint16 CapCompenNum=0,CapDisperNum=0;
Uint16 DisperACount=0,DisperBCount=0,DisperCCount=0;
Uint16 DisperAOffCount=0,DisperBOffCount=0,DisperCOffCount=0;
Uint16 PIAdjustTim=0;
#pragma DATA_SECTION(CapSrcDataBuff, "EBSS1")
#pragma DATA_SECTION(CntFaultDelay, "EBSS1")
#pragma DATA_SECTION(CntFaultDelayLong, "EBSS1")
int16 CapSrcDataBuff[70];

/*
 * 功能：检测到的故障信号次数。
 *
 * [26]：检测到的A相旁路磁保继电器闭合故障信号次数
 */
Uint16 CntFaultDelay[CNT_FAULT_DETECT_NUM];

Uint32 CntFaultDelayLong[LCNT_FAULT_DETECT_NUM];

SOFT_FAULT_BIT1 softwareFaultWord1;
SOFT_FAULT_BIT2 softwareFaultWord2;
SOFT_FAULT_BIT3 softwareFaultWord3;
SOFT_FAULT_BIT4 softwareFaultWord4;
SOFT_FAULT_BIT5 softwareFaultWord5;
Uni_State StateEventFlag1;

#pragma DATA_SECTION(HarmTHD, "EBSS1")
STRU_HarmTHD HarmTHD[HARM_CALU_NUM];
VirtulADStru VirtulAD={0,0,0,0,0,0,0,0,0,0,2076,2080,2080,2083,2081,2078,2053,2062,2056,1,1,1,1,1};
VirtulADStruVAL VirtulADVAL;
VirtulADStruval VirtulADval={2076,2080,2080,2083,2081,2078,2053,2062,2056,2054,2052,2055,0,0,0};
STRU_WinCold WindCold={95,130,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
#pragma DATA_SECTION(SyncInformation, "EBSS2")
Stru_SyncInformation SyncInformation;
Stru_Time Time;

struct STRU_CNT_MS CntMs;
struct STRU_CNT_SEC CntSec;
struct STRU_FAURCD RecordFlash;

Uint16 IOstate1=0;
Uint16 IOstate2=0;
int16 Choose1 = 0;
int16 Choose2 = 0;
int16 Choose3 = 0;
int16 Choose4 = 0;
int16 Choose5 = 0;

int GridvoltCntA=0,GridvoltCntB=0,GridvoltCntC=0,CURRCTRLCNT = 0,TEMPCTRLCNT;
int ESCFlagValA = 0,ESCFlagValB = 0,ESCFlagValC = 0,ESCDUTYFlagA = 1,ESCDUTYFlagB = 1,ESCDUTYFlagC = 1;

int TESTVALUE = 0;
int BypassContactCntA = 0,BypassContactCntB = 0,BypassContactCntC = 0;
int FanCTLCnt = 0;
int ContactorFeedBackCntA = 0,ContactorFeedBackCntB = 0,ContactorFeedBackCntC = 0;
int StandByFlag = 0;

int VoltagemodeFlag=0;
int VoltPowerOnflag = 0;

int BYPASS_CONTACT_CNT=0;

struct STRU_STATE_FLAG StateFlag;
float GardVoltTarVal = 0;
float InvCurUVRmsF = 0;
float GardVoltIntissued = 0;
float sa=0,sb=0,sc=0;
int BY_Flag=0,BY_Flag1=0;
//int TempInitVal = 50;
float TempMaxValue = 0;

const unsigned int Tempera_Volt[171]={
3498,3489,3480,3470,3460,3450,3439,3428,3417,3405,
3393,3381,3368,3354,3341,3327,3312,3298,3282,3267,
3251,3234,3218,3201,3183,3165,3147,3129,3110,3091,
3071,3052,3032,3011,2991,2970,2949,2928,2907,2885,
2863,2841,2819,2797,2775,2753,2730,2708,2685,2663,
2640,2618,2595,2573,2550,2528,2506,2484,2462,2440,
2418,2397,2375,2354,2333,2312,2292,2271,2251,2231,
2211,2192,2173,2154,2135,2117,2099,2081,2063,2046,
2029,2012,1996,1979,1963,1948,1932,1917,1903,1888,
1874,1860,1846,1833,1820,1807,1794,1782,1770,1758,
1747,1735,1724,1713,1703,1692,1682,1672,1663,1653,
1644,1635,1626,1617,1609,1601,1593,1585,1577,1569,
1562,1555,1548,1541,1534,1528,1521,1515,1509,1503,
1497,1492,1486,1481,1475,1470,1465,1460,1455,1451,
1446,1441,1437,1433,1429,1425,1421,1417,1413,1409,
1405,1402,1398,1395,1392,1388,1385,1382,1379,1376,
1373,1370,1368,1365,1362,1360,1357,1355,1352,1350,
1348};


#pragma DATA_SECTION(FactorySet,"EBSS2");

STRU_ERR_RECORD ErrorRecord;
struct Stru_APFID MuFaciID;
struct Fan_temp FanTempValue;
struct Stru_UserSetting UserSetting;
struct Stru_UserSetting UserParamRatio				= USER_PARAM_RATIO_3P_MEC_DEFUALT;
struct Stru_FactorySet FactorySet;
struct Stru_FactorySet FactorySetupRatio			= FACTORY_PARAM_RATIO_3P_APF_DEFUALT;
struct VARIZERO VariZeroOffset;
struct VARIZERO VariZeroOffsetRatio					= VARIZEROOFFSETRATIO;

struct VARIZEROVAL VariZeroOffsetVAL;
struct VARIZEROVAL VariZeroOffsetRatioVAL           = ESCVARIZEROOFFSETRATIO;

#pragma DATA_SECTION(CapGroupAct,"EBSS3");
struct CapSwitch CapGroupAct;
CapSwitchEvent CapEventFlag = STATE_EV_WRCTRL;
float xrand;

struct Stru_FactorySet ParamEnviron;
ESCCTRLVALFLAG ESCFlagA,ESCFlagB,ESCFlagC;

