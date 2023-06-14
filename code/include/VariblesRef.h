/*
 * VariblesRef.h
 *
 *  Created on: 2013-4-9
 *      Author: Administrator
 */

#ifndef VARIBLESREF_H_
#define VARIBLESREF_H_
#include "CoreCtrl.h"
#include "F2837xS_device.h"

/*微型断路器。WY*/

#define EEPROM_FACTORY_PARAMETER		0x0		//0x00~0x1FF
#define EEPROM_USER_PREFERENCES			0x200	//0x200~0x2CF
#define EEPROM_RECORD_FLASH				0x2D0	//0x2D0~0x2FF
#define EEPROM_VIRTU_ZEROOFFSET			0x300	//0x300~0x3FF
#define EEPROM_CAPPARAM					0x400	//0x100~0x14F	0x50	电容器组	用户自定义下发(0x67,9000~9037)
#define EEPROM_HARMNOIC_USER			0x500	//
#define EEPROM_HARMNOIC_FACTORY			0x564	//
#define	EEPROM_FACILITYID				0x8D0   //0x8D0~0x8DF
#define EEPROM_RECORD_INDEX				0x8E0	//0x8E0~0x907	0x28	SOE索引	SOE读取(0x6A) 40
#define EEPROM_RECORD					0x908	//0x908~0x1FFF	0x16F8	SOE数据块	SOE读取(0x6A) 42*2*故障条数(70)

#define SOE_WRITE_ADDR_GROUP			EEPROM_RECORD_INDEX
#define SOE_READ_ADDR_GROUP				EEPROM_RECORD_INDEX+1
#define SOE_WRITE_ADDR_FAULTFLAG 		EEPROM_RECORD_INDEX+2
#define SOE_READ_ADDR_FAULTFLAG 		EEPROM_RECORD_INDEX+3
#define WAVE_RECORD_START_ADDR      0x2000

#define FLASH_UI_BLOCK_READ_ADDR    EEPROM_RECORD_INDEX + 4
#define FLASH_BLOCK_READ_ADDR       EEPROM_RECORD_INDEX + 5
#define FLASH_BLOCK_WRITE_ADDR      EEPROM_RECORD_INDEX + 6
#define FLASH_RECORD_BLOCK_ADDR     EEPROM_RECORD_INDEX + 7
#define FLASH_RECORD_HANDLER_ADDR   EEPROM_RECORD_INDEX + 8

#define SOE_GP_FAULT 					100
#define SOE_GP_WARNING 					400

/*事件在操作日志中的偏移量。？WY*/
#define SOE_GP_EVENT 					300

#define SOE_BUFF_GROUP					10			//可记录10个故障同时产生
#define RECORD_NUMBER					30			//可记录事件总数

#define EEPROMLOCK  			1
#define EEPROMUNLOCK 			0
#define DEBUG_WAVE_CHAN_NUM  	16
#define DEBUG_WAVE_GROUP 		(12)
#define DEBUG_WAVE_LEN 			500

#define FIXED_CAP_EN			0				//开启电容器组投切
#define FIXED_IOCAP_NUM 		4				//IO开出方式的无功投切电容的组数.
#define FIXED_CAP_NUM 			4				//吉林特纳普无功投切电容的组数.
#define COMM_FIXED_CAP 			1				//吉林特纳普无功投切电容的方式为通信投切

#define FIRST_RMSNUM            12               //快速计算RMS数量,数组申请空间是一起的,先放快速的,紧接着是慢速的
#define SLOW_RMSNUM             14              //慢速计算的RMS数量
#define HARM_CALU_NUM 			9				//谐波显示的数量
#define ALL_RMSNUM  			    (FIRST_RMSNUM+SLOW_RMSNUM+2*HARM_CALU_NUM)              //总RMS数量
#define RMS_CALU_SEGMENT            (4)                                                     //4段
#define RMS_CALU_SEGMENT_NUM        (RMS_CALU_SEGMENT+2)                                    //最快每隔5ms出一个RMS值.故切成了20ms/5ms=4段,+2用于缓冲
#define RMS_CALU_BLOCK_NUM          ((CTRLFREQUENCY/POWERGRID_FREQ)/RMS_CALU_SEGMENT)       //每段的点数是6400/50/4段=32点,每32个点合成一个值

#define CALIBRATION_DISP_LEN        48

/*故障类型总数。WY*/
#define CNT_FAULT_DETECT_NUM            43  //要大于从CNT_HW_OVER_CUR_A开始的数量

#define CNT_HW_OVER_CUR_A               0   //
#define CNT_HW_OVER_CUR_B               1   //
#define CNT_HW_OVER_CUR_C               2   //

/*已检测到的A相瞬时值过流次数？WY*/
#define CNT_INS_OVER_CUR_A              3

/*已检测到的B相瞬时值过流次数？WY*/
#define CNT_INS_OVER_CUR_B              4

/*已检测到的C相瞬时值过流次数？WY*/
#define CNT_INS_OVER_CUR_C              5

/*已检测到的A相直流电容电压过压信号的次数。WY*/
#define CNT_DC_NEUTRAL_OVER_VOLT_A      6

/*已检测到的B相直流电容电压过压信号的次数。WY*/
#define CNT_DC_NEUTRAL_OVER_VOLT_B      7

/*已检测到的C相直流电容电压过压信号的次数。WY*/
#define CNT_DC_NEUTRAL_OVER_VOLT_C      8

#define CNT_RMS_OVER_VOLT_A             9   //
#define CNT_RMS_OVER_VOLT_B             10  //
#define CNT_RMS_OVER_VOLT_C             11  //
#define CNT_CTRL_POWER_OFF              12  //
#define CNT_RMS_UNDER_VOLT_A            13  //
#define CNT_RMS_UNDER_VOLT_B            14  //
#define CNT_RMS_UNDER_VOLT_C            15  //
#define LCNT_LOAD_OVER_CUR_A            16  //
#define LCNT_LOAD_OVER_CUR_B            17  //
#define LCNT_LOAD_OVER_CUR_C            18  //
#define CNT_RMS_OVER_CUR_A              19  //
#define CNT_RMS_OVER_CUR_B              20
#define CNT_RMS_OVER_CUR_C              21
#define CNT_SET_OVER_TEMP_A             22
#define CNT_SET_OVER_TEMP_B             23
#define CNT_SET_OVER_TEMP_C             24
#define CNT_FANGLEI_FAULT               25

/*已检测到的A相旁路磁保继电器闭合故障次数。WY*/
#define CNT_BYPASS_NOCLOSE_A            26

#define CNT_BYPASS_NOCLOSE_B            27
#define CNT_BYPASS_NOCLOSE_C            28
#define CNT_SET_BOARD_OVER_TEMP         29
#define CNT_OVER_FREQ                   30
#define CNT_UNDER_FREQ                  31
#define CNT_CALIB_ZERO_SHIFT            32
#define CNT_PARAM_STORAGE_EEPROM        33
#define CNT_HIGHLOWRELAY_A              34
#define CNT_HIGHLOWRELAY_B              35
#define CNT_HIGHLOWRELAY_C              36
#define CNT_SICFAULT_A                  37
#define CNT_SICFAULT_B                  38
#define CNT_SICFAULT_C                  39
#define CNT_BYRelay_A                   40
#define CNT_BYRelay_B                   41
#define CNT_BYRelay_C                   42

#define LCNT_FAULT_DETECT_NUM			1	//要大于从LCNT_LOAD_OVER_CURR开始的数量
#define LCNT_LOAD_OVER_CURR 			0	//
////电容投切参数
//#define CAPACITOR_CLOSE_TIME 		6
//#define CAPACITOR_OPEN_TIME 		12
#define zeroOffsetUpLimit 			2200.0f	// 零偏校准后，限定范围
#define zeroOffsetDownLimit 		2000.0f

//场景模式
#define BGMODLE_3P_APF 				0		//MEC模式,补谐波,无功,不平衡,不带变压器模式
#define BGMODLE_3P_SVG 				1		//SVG模式
#define BGMODLE_3P_VOLT_REGULATOR 	2		//三相电压调节器
#define BGMODLE_3P_TRASFOROM_APF 	3		//MEC模式,补谐波,无功,不平衡,带变压器模式
#define BGMODLE_3P_MEC 				4		//MEC模式,补谐波,无功,不平衡,电压(用闭环方式)
#define BGMODLE_1P_VOLT_REGULATOR 	5		//单相调压器模式
#define BGMODLE_3P_CURR_SOURCE 		6		//五桥臂电流源模式

//THDi显示
//#define DISPLAY_MEC_FLAG_MEC		1		//显示无功
//#define DISPLAY_MEC_FLAG_THDI		0		//显示THDi

#define CAP_LOAD_RATIO				(51*1e-6*314)
#define CAP_VOL_POINT				((PWMFREQUENCY/(50*4))*2*3)	//每周波的3/4
#define CAP_VOL_POINT_CUR			((PWMFREQUENCY/(50*4))*2*1)	//每周波的1/4
#define CAP_CUR_POINT				(((CTRLFREQUENCY/50)*3)/4)

#define TotalHarmDistorionVoltA 		0
#define TotalHarmDistorionVoltB 		1
#define TotalHarmDistorionVoltC 		2
#define TotalHarmDistorionGridCurrA		3
#define TotalHarmDistorionGridCurrB		4
#define TotalHarmDistorionGridCurrC		5
#define TotalHarmDistorionLoadCurrA		6
#define TotalHarmDistorionLoadCurrB		7
#define TotalHarmDistorionLoadCurrC		8

#define shift(v,shift) (((Uint16)(v) << (shift))&((Uint16)1 << (shift)))
#define SETUP_RATIO_CHANGE(original,ratio) (ratio) ? (float)(original)/(ratio) : (original)
#define GridFundaVoltD (SPLL[0].GridPLLVoltD)
#define GridFundaVoltQ (SPLL[0].GridPLLVoltQ)

//extern float GridPLLVoltD;
//extern float GridPLLVoltQ;
//extern float GridPLLVoltDp;
//extern float GridPLLVoltQp;
//extern float GridPLLVoltDn;
//extern float GridPLLVoltQn;
//extern float GridPLLVoltDpf;
//extern float GridPLLVoltQpf;
//extern float GridPLLVoltDnf;
//extern float GridPLLVoltQnf;
extern float GridFundaVoltND;
extern float GridFundaVoltNQ;
extern float PLLVoltD;
extern float GridVoltTheta;//,VoltThetaB,VoltThetaC;
extern float GridCurrTheta;
extern float VoltTheta2;
extern float VoltCurrCalibrPhase;
extern float LoadCurrCalibrPhase;
extern float GridCurrCalibrPhase;
extern float InvCurrCalibrPhase;

extern float ThreeHarmGridVoltRms;
extern float GridFreq;
extern Uint16 CntFaultDelay[CNT_FAULT_DETECT_NUM];
extern Uint32 CntFaultDelayLong[LCNT_FAULT_DETECT_NUM];
extern int16 VoltPos,VoltPrvPos;
extern int CapVoltPos,CapVoltPosCur;
extern int CurrThisPos,CurrPrvPos;

extern int BypassContactCntA,BypassContactCntB,BypassContactCntC;
extern int FanCTLCnt;
extern int ContactorFeedBackCntA,ContactorFeedBackCntB,ContactorFeedBackCntC;
extern int GridvoltCntA,GridvoltCntB,GridvoltCntC,CURRCTRLCNT,TEMPCTRLCNT;
extern int StandByFlag;
extern int ESCFlagValA,ESCFlagValB,ESCFlagValC,ESCDUTYFlagA,ESCDUTYFlagB,ESCDUTYFlagC;

extern int TESTVALUE;
extern int VoltagemodeFlag;
extern int VoltPowerOnflag;

extern Uint16 MeanHalfPos;
extern float32 testout[];
extern float32 testin[];

extern int Esc_NUM[32];
extern float Esc_CurPhaseA,Esc_CurPhaseB,Esc_CurPhaseC;
extern float Esc_VoltPhaseA,Esc_VoltPhaseB,Esc_VoltPhaseC;
extern int TestdataFlag;


extern float DccapVoltA;
extern float DccapVoltB;
extern float DccapVoltC;
extern float GridBPCurrAF;
extern float GridBPCurrBF;
extern float GridBPCurrCF;
extern float GridVoltAF,GridVoltAF_Beta;
extern float GridVoltBF,GridVoltBF_Beta;
extern float GridVoltCF,GridVoltCF_Beta;
extern float LoadVoltUF,LoadVoltUF_Beta;
extern float LoadVoltVF,LoadVoltVF_Beta;
extern float LoadVoltWF,LoadVoltWF_Beta;
extern float VoltInAF,VoltInAF_Beta;
extern float VoltInBF,VoltInBF_Beta;
extern float VoltInCF,VoltInCF_Beta;
extern float VoltOutAF,VoltOutAF_Beta;
extern float VoltOutBF,VoltOutBF_Beta;
extern float VoltOutCF,VoltOutCF_Beta;
extern float GridCurrAF;
extern float GridCurrBF;
extern float GridCurrCF;
extern float GridVoltAFHold;
extern float GridVoltBFHold;
extern float GridVoltCFHold;
extern float GridVoltAlpha;
extern float GridVoltBeta;
extern float GridVoltRms;
extern float GridVoltRms_F;
extern float LoadVoltRms;
extern float LoadVoltRms_F;
extern float GridCurrZA;
extern float GridCurrZB;
extern float GridCurrZC;
extern float GridCurrA,GridCurrB,GridCurrC;
extern float GridCurrHvA,GridCurrHvB,GridCurrHvC;
extern float LoadRealCurHvA,LoadRealCurHvB,LoadRealCurHvC;
extern float GridZeroCur;
extern float GridCurRms;
extern float GridCurBYRms;
extern float GridCurRms_F;
extern float GridCurBYRms_F;
extern float CurrRefRms;
extern float CurrRefRms_F;
extern float GridFundaCurD;
extern float GridFundaCurQ;
extern float GridFundaCurND;
extern float GridFundaCurNQ;
extern float Rec_sa;
extern float Rec_sb;
extern float Rec_sc;
extern float LoadRealCurA;
extern float LoadRealCurB;
extern float LoadRealCurC;
extern float LoadRealCurAF;
extern float LoadRealCurBF;
extern float LoadRealCurCF;
extern float LoadRealCurZ;
extern float LoadCurRms;
extern float LoadCurRms_F;
extern float LoadFundaCurZ;
extern float NegCurCompZ;
extern float LoadFundaCurD;
extern float LoadFundaCurQ;
extern float LoadFundaCurND;
extern float LoadFundaCurNQ;
extern float ApfOutCurZA;
extern float ApfOutCurZB;
extern float ApfOutCurZC;
extern float ApfOutCurA;
extern float ApfOutCurB;
extern float ApfOutCurC;
extern float ApfOutCurAD;
extern float ApfOutCurBD;
extern float ApfOutCurCD;
extern float ApfOutCurZ;
extern float ApfOutCurZ_F;
extern float InvFundaCurD;
extern float InvFundaCurQ;
extern float InvFundaCurND;
extern float InvFundaCurNQ;
extern float GridVoltDA,GridVoltQA,GridVoltDB,GridVoltQB,GridVoltDC,GridVoltQC;
extern float LoadRealCurDA,LoadRealCurQA,LoadRealCurDB,LoadRealCurQB,LoadRealCurDC,LoadRealCurQC;
extern float GridRealCurDA,GridRealCurQA,GridRealCurDB,GridRealCurQB,GridRealCurDC,GridRealCurQC;
extern float IverRealCurDA,IverRealCurQA,IverRealCurDB,IverRealCurQB,IverRealCurDC,IverRealCurQC;
extern float ApfOutCurRms;
extern float ApfOutCurRms_F;
extern float dcVoltF;
extern float PreviousDcVoltF;
extern float PreviousDcVoltFTo2;
extern float UnderVoltageLimit;
extern float dcVoltUp;
extern float dcVoltUpF;
extern float dcVoltDn;
extern float dcVoltDnF;
extern float GridActPower ;
extern float GridReactPower ;
extern float GridApparentPower ;
extern float GridActPowerPhA,GridReactPowerPhA,GridApparentPowerPhA,GridCosFiPhA;
extern float GridActPowerPhB,GridReactPowerPhB,GridApparentPowerPhB,GridCosFiPhB;
extern float GridActPowerPhC,GridReactPowerPhC,GridApparentPowerPhC,GridCosFiPhC;
extern float LoadActPowerPhA,LoadReactPowerPhA,LoadApparentPowerPhA,LoadCosFiPhA;
extern float LoadActPowerPhB,LoadReactPowerPhB,LoadApparentPowerPhB,LoadCosFiPhB;
extern float LoadActPowerPhC,LoadReactPowerPhC,LoadApparentPowerPhC,LoadCosFiPhC;
extern float CSReactPowerave;
extern float GridCosFi;
extern float MinVolRms;
extern float MinLoadCosFi;
extern float LoadCurRmsUnban;
extern float GridCurRmsUnban;
extern float32 TransformVoltH2L,TransformVoltL2H,TransformCurrL2H,TransformCurrH2L;
extern float PrvFreq;
extern Uint16 cntSyncSucc;

//extern float Voltuplimit;
//extern float Voltlowlimit;
//extern float Voltoverlimitcompentarvalue;
//extern float Voltunderlimitcompentarvalue;

extern int i;
//extern int FunContDelayFlag;
extern int ArithFlagA,ArithFlagB,ArithFlagC;
extern int TestArithFlagA,TestArithFlagB,TestArithFlagC;
extern Uint16 GridcurrCnt;

extern int PeakStopFlagA,PeakStopFlagB,PeakStopFlagC;

extern int OverTempCnt,OverBoardCnt,OverLoadCnt,OverVoltCnt,UnderVoltCnt;
//extern int16 MeanInitChan;
//extern int16 MeanPos_F;
//extern int16 CapSrcDataBuff[70];
// 有效值的计算变量
extern float VoltGivenStart;
extern float VoltInA_rms;
extern float VoltInB_rms;
extern float VoltInC_rms;
extern float VoltOutA_rms;
extern float VoltOutB_rms;
extern float VoltOutC_rms;
extern float apfOutCurA_rms;
extern float apfOutCurB_rms;
extern float apfOutCurC_rms;
extern float loadCurA_rms;
extern float loadCurB_rms;
extern float loadCurC_rms;
extern float gridCurA_rms;
extern float gridCurB_rms;
extern float gridCurC_rms;
extern float gridCurrBYAF_rms;
extern float gridCurrBYBF_rms;
extern float gridCurrBYCF_rms;
//extern float gridVoltRMS;
extern int cntGridVoltPhaseSeqFault;
//计数器使用，用于时间计数等
extern int RecordWaveFlag ;
extern int CurrentProperty ;
extern int SubordinateStatus ;
extern int MatchCondition ;
extern int StandbyRecoveryTime ;
extern int Mode_PP ;
extern float AutoJudgeRms_F;
extern float AutoStartCurr;
extern float AutoWaitCurr;
extern float AutoStartMode;
extern float AutoWaitCurr;
extern float AutoModeCurr;
extern float R_SSwitchTime;
extern float StandCosFiLimit;
extern float StandUbanCurPer;
extern int16 LowPowerTimCnt;
extern int CntFor100us ;
extern int CntFor300us;
extern float manualubanlanccurA,manualubanlanccurB,manualubanlanccurC;
extern Uint32 CntForPowerON;				//上电后一直计数,用于万年历时间计算

extern int16 EscStandyDelayCnt;
extern int CTRL24_POWERFlag;

//extern int TempInitVal;
extern float TempMaxValue;

extern int EscStandyDelayCnta1,EscStandyDelayCntb1,EscStandyDelayCntc1,EscStandyDelayCnta2,EscStandyDelayCntb2,EscStandyDelayCntc2;
extern int StateSkipFlagA,StateSkipFlagB,StateSkipFlagC;
extern int GV_RMS_UNDER_PowerOnForRecharge;

//extern float DebugInner[DEBUG_WAVE_GROUP];
extern float debugW[DEBUG_WAVE_GROUP][DEBUG_WAVE_LEN];
extern Uint16 RecordConfigBuff[16];
extern int16 CntForRec;
extern long GridCurrPrvADFlag;
extern long GridCurrPrvAD[3];
extern float tmpOffsetValue[18];//零偏定义
extern const unsigned int Tempera_Volt[171];
extern int TempEnvirProvalue;
extern float ESCTESTDATA1;
extern float ESCTESTDATA2;
extern float ESCTESTDATA3;
extern float ESCTESTDATA4;
extern int adcAutoCount;
extern float dcBusVolt;
extern float dcBusVoltGiven;
extern float kp_Dc_Div;
extern float ki_Dc_Div;
extern float kp_Dc;
extern float ki_Dc;
// DQ电流环
extern float currentRefD;
extern float currentRefQ;
extern float currentRefQa;
extern float currentRefQb;
extern float currentRefQc;
extern float currentRefA;
extern float currentRefB;
extern float currentRefC;
extern float GpVoltA_rmsReciprocal;
extern float GpVoltB_rmsReciprocal;
extern float GpVoltC_rmsReciprocal;
extern float kp_CurA;
extern float kp_CurB;
extern float kp_CurC;
extern float kp_Cur;
extern float ki_Cur;
extern float CurrRefDCorr;
extern float HarmLimtCorr;
extern float CapaReactCorr,CapaReactCorrA,CapaReactCorrB,CapaReactCorrC;
extern float CapaUnbalCorr;

// 输出DQ
extern float reactPowerCompCurQ ;
extern float CapreactPowerCurQ;
extern float reactPowerCompCurQa ;
extern float reactPowerCompCurQb ;
extern float reactPowerCompCurQc ;
extern float ReactPowerCurQPA ;
extern float ReactPowerCurQPB ;
extern float ReactPowerCurQPC ;
extern float CapReactPowerCurQPA ;
extern float CapReactPowerCurQPB ;
extern float CapReactPowerCurQPC ;
extern float ReactLoadCompCurQ;
extern float ReactLoadCompCurA;
extern float ReactLoadCompCurB;
extern float ReactLoadCompCurC;
// 电流计算
extern float GridResSin ;
extern float GridResCos ;
//extern float PLLResSin ;
//extern float PLLResCos ;
//extern float PLLRes2Sin ;
//extern float PLLRes2Cos ;

// 电气启动
//extern volatile int StateEventFlag ;
extern volatile int StateEventFlag_A;
extern volatile int StateEventFlag_B;
extern volatile int StateEventFlag_C;
extern float PIlim_Udc;
extern float PIlim_I;
extern float I_ins_index;
//extern int DisPWM_ins_index;
extern float reactPowerGiven;
//extern float VoltupLIMIT;
//extern float VoltlowLIMIT;
//extern float VoltoverlimitcompentarVALUE;
//extern float VoltunderlimitcompentarVALUE;
extern float VolttargetA,VolttargetB,VolttargetC,VoltThreSholdA,VoltThreSholdB,VoltThreSholdC,Backdiffvalue;

extern float constantCosFai,PFConsin;
extern float PwmVa,PwmVb,PwmVc;
extern float PwmVaN,PwmVbN,PwmVcN;
extern int16 RemoteReactpowerOrder;
extern Uint16 RemoteStateRunStop;
extern Uint16 RemoteStateWait;
extern Uint16 RemoteStateFault;
extern float quarterPeriod;
//extern float Q2D_ratio;
//extern int Q2D_ratio;
extern int PhaseControl;
//extern float DeadAdjust;
extern float LoadCurlimit;
extern float Curr_MinCtrlFAN;
extern float Curr_MaxCtrlFAN;
extern float Temp_MinCtrlFAN;
extern float Temp_MaxCtrlFAN;
extern float FanStartMinDUTY;
extern float RateLoadCurrLimit;
extern int CapacitySelection;

extern float SyncHardwareLead;
extern float SampleLead;  // 8.6 待调试系数 0816

//保护变量
extern float GV_RMS_OVER_RLY_PROT;
extern float GV_INS_OVER;
extern float GV_RMS_OVER;
extern float GV_RMS_UNDER;
extern float GF_OVER;
extern float GF_UNDER;
extern float ESCDCVOLITLIMIT;
extern float VoltFallResponseLimit;
extern float reactPowGivenLimit;
extern float PosReactPowGivenLimit;
extern float NegReactPowGivenLimit;
extern float REACTPOWERLIMIT;
extern float REACTPOWERLIMITPH;
extern float OUTCUR_OVER_INS;
extern float OUTCUR_OVER_INS_NEG;
extern float OUTCUR_OVER_RMS;
extern float OUTCUR_FAULTCONTACT;
extern float OUT_OVER_LOAD;
extern float GV_NEGTIVE_UP;
extern float VoltHarmOver;
extern float DC_ERR_LIMIT;

extern int Duty_cyc_FLAG;
extern int ESCtestVAL1,ESCtestVAL2;

extern float TIMER1,TIMER2,TIMER3,TIMER4,TIMERPERIODVAL1,TIMERPERIODVAL2,PERIODVAL1,PERIODVAL2;
extern int CAPTIEMRFlag;
//extern float ESC_L,CurrAD,CurrentRateA,CurrentRateB,CurrentRateC;

extern float Esczct_CNTA,Esczct_CNTB,Esczct_CNTC;
extern float StepSize;
extern float ESC_FeedForward_DutyA,ESC_FeedForward_DutyB,ESC_FeedForward_DutyC;
extern int ESCLowCurrFlagA,ESCLowCurrFlagB,ESCLowCurrFlagC;
extern float PIOutVoltValueA,PIOutVoltValueB,PIOutVoltValueC;
extern Uint32 *EPwm8RegsCMPA,*EPwm8RegsCMPB,*EPwm7RegsCMPA,*EPwm7RegsCMPB,*EPwm6RegsCMPA,*EPwm6RegsCMPB,*EPwm5RegsCMPA,*EPwm5RegsCMPB,*EPwm4RegsCMPA,*EPwm4RegsCMPB,*EPwm3RegsCMPA,*EPwm3RegsCMPB;
extern Uint32 *EPwm8RegsAQCSFRC,*EPwm7RegsAQCSFRC,*EPwm6RegsAQCSFRC,*EPwm5RegsAQCSFRC,*EPwm4RegsAQCSFRC,*EPwm3RegsAQCSFRC;
extern Uint32 *CLAtest1,*CLAtest2;//,*CLAtest3,*CLAtest4;
extern Uint32 *AdcaResultRegsADCRESULT14,*AdcaResultRegsADCRESULT15,*AdcbResultRegsADCRESULT14,*AdcbResultRegsADCRESULT15,*AdccResultRegsADCRESULT14,*AdccResultRegsADCRESULT15;
extern Uint32 *AdccResultRegsADCRESULT12,*AdccResultRegsADCRESULT13,*AdccResultRegsADCRESULT10,*AdccResultRegsADCRESULT8;
extern Uint32 *AdcbResultRegsADCRESULT12,*AdcbResultRegsADCRESULT13,*AdcbResultRegsADCRESULT10,*AdcbResultRegsADCRESULT8;
extern Uint32 *AdcaResultRegsADCRESULT12,*AdcaResultRegsADCRESULT13,*AdcaResultRegsADCRESULT10,*AdcaResultRegsADCRESULT8;
extern Uint16 SCIBMASTERENABLE;
extern Uint16 SCICMASTERENABLE;
extern Uint16 SCIDMASTERENABLE;
extern Uint16 RandIDH,RandIDL;
extern int16  StartDelayTim;
extern Uint16 FlashStartTim;
extern Uint16 RandTim,RandTimCount;
extern Uint16 IDDelayReportEnable;
extern Uint16 CapFlagDown;				//电容器组投切    标志位
extern Uint16 SwitchTimeOn;          	//投计时
extern Uint16 SwitchTimeOff;          	//切计时
extern Uint16 TimSwitchOnStart;			//投延时计时标志位
extern Uint16 TimSwitchOffStart;		//切延时计时标志位
extern Uint16 AutoSwitchOnCount;		//自动投组数统计
extern Uint16 AutoSwitchOffCount;		//自动切组数统计
extern Uint16 AutoSwitchOn;				//自动投标志
extern Uint16 AutoSwitchOff;			//自动切标志
extern Uint16 FlashRecordTim;
extern Uint16 ReFFTDelayTim;
extern Uint16 CpuHeatDelayTime;
extern float CapStartCapcity;
extern float CapStartLimit;
extern Uint16 CapCompenNum,CapDisperNum;
extern Uint16 DisperACount,DisperBCount,DisperCCount;
extern Uint16 DisperAOffCount,DisperBOffCount,DisperCOffCount;
extern Uint16 CapCount,CapCountA,CapCountB,CapCountC;
//extern Uint16 SCIdealR[3][5],SCIdealT1[256],SCIdealT2[70],SCIdealT3[256];
//extern Uint16 SCI_RA[3][30],SCI_TA1[256],SCI_TA2[70],SCI_TA3[256];
extern Uint16 PIAdjustTim,ChargeVoltage;
extern Uint16 PIAutoJust;
extern Uint16 SendOrderCount;
extern float MU_LCD_RATIO,MU_MultRatio;			//设备运行参数
extern int MU_ALLCOUNT,PWM_address,APF_ID;
extern Uint16 CapEmergTim;
extern float storageUsed;
extern float GridNeutralCurRms;
extern float harmCompPerc;					//界面人工预置谐波系数
extern int VolUpCo;							//电压上限
extern int VolDownCo;						//电压下限
extern int VolUpSurCo;						//电压上限越限补偿目标值
extern int VolDownSurCo;					//电压下限越限补偿目标值
extern int VolSurTime;						//电压越限持续时间

extern int StableValueA,StableValueB,StableValueC;

extern float TempData[10];
extern float CTLFANDATA[10];
extern float CURRData[10];
extern float UnCurrData[3];
extern float UNCurDiffData[10];
extern float CurrentUnbalanceRegularVoltage[3];
extern int VoltageOverflow[3];
extern int centreGravity;
extern float centreGravityValue;
extern float currentUnbalance;
extern Uint16 windColdCtr;

extern float dutytmpA1,dutytmpA2,dutytmpB1,dutytmpB2,dutytmpC1,dutytmpC2;
extern float CurrTargetTemper;

extern float dutytmp,dutytmp1,TESEINSGridIn,TESETarget,TESEPIerr,testvalue1,FeedVAl,DEBUGduty,DEBUGData,PhaseValue,TESTRMSGridInVAL,NumeratorValue,DenominatorVAL;
extern float VolttargetCorrA,VolttargetCorrB,VolttargetCorrC;
extern int ConstantCurrInsFlagA,ConstantCurrInsFlagB,ConstantCurrInsFlagC;
extern float VolPIOutPwmVa;
extern float VolPIOutPwmVb;
extern float VolPIOutPwmVc;
extern float PIVAL;
extern float VolCor;							//电压目标值
extern float VolCorU,VolCorV,VolCorW;
extern int RechageskipstopFlag;
extern float ModeSwitchingCurrent;					//模式转换电流阀值
extern float OutputCurrent ;
extern float reactPrCompPerc;
extern float restantReactCurrent ;
extern float ConstantReactivePower;
extern float TargetCurrentUnbalance;
extern float transfRatio,transfRatioVirtu;
extern float gridVoltRatio,gridVoltRatioVirtu;  //
extern float outputCurRatio,outputCurRatioVirtu;
extern float bypassCurrRatio,bypassCurrRatioVirtu;
extern float loadCurRatio;
extern float outputCurRatioCurrA;
extern float outputCurRatioCurrB;
extern float outputCurRatioCurrC;
extern float outputCurBypassCurrA;
extern float outputCurBypassCurrB;
extern float outputCurBypassCurrC;
//extern float gridCurRatio;

extern float dcCapVoltRatio,DcCapVoltRatioVirtu,dcVoltUpRatioVirtu_reciprocal;
extern float loadVoltRatio,loadVoltRatioVirtu;
extern const int pt_tab[];
extern const int pt100_tab[];
extern const int ntc_tab[];
extern const float fre_tab[];
extern const float fre_tab1[];
extern const float fre_tab2[];
extern const int CurTarget_30[];
extern const int CurTarget_50[];
extern const int CurTarget_75[];
extern const float CurTarget[3][21];
extern const Uint16 Convert4bTo5b[20];
extern const Uint16 Convert5bTo4b[32];
extern const Uint16  CRCHi[256],CRCLo[256];
extern Uint16 CRC(Uint16 *puchMsg, Uint16  usDataLen);		//CRC校验函数声明

extern float T1PR,FUNT1PR;
extern float LoadFundaCurZRms,CurFundNgARms,CurFundNgBRms,CurFundNgCRms,CurFundNgA,CurFundNgB,CurFundNgC;
extern float CurrHarmRefZ,ApfOutCurZRms,LoadRealCurZRms;
extern float CurrRefARms,CurrRefBRms,CurrRefCRms;
extern float WholeRmsLimit,WholeZRmsLimit,WholeRmsLimitPH;
extern float ResonProtcABC;
extern float ResonProtcABCRms;
extern float dcVoltDiv2Err,DCtestMirr;
extern float NegCurLimit;
extern float S_RSwitchTime;
extern float OutCurMaxLimit;				// 输出电流总限幅
extern float OutCurMaxLimitNeg;
extern float TransformRatioVal;			//高压侧变压器变比值,由TrasformRatioParall计算得出
extern int forwardPoints ;
extern int cntForCurMax;
extern float outCurTempLimit;
extern float outCurSinkTempLimit;
extern float outCurBoardTempLimit;
extern float capTsctr;				//捕获数值
extern int cntForSyncInit;			//同步初始化
extern Uint16 cntForSyncErr;
extern int syncState;				//同步状态机
extern float pwmTsGiven;			//捕获PI的给定
extern float POINT_CAP1;			//信号错分
extern float MAX_CAP1;
extern float MIN_CAP1;
extern float syncErr;				//捕获PI控制器
extern float syncErrPrior;
extern Uint16 pwmTBPRD;
extern Uint16 PrvPwmTBPRD;
extern Uint16 RemoteAddress;
extern Uint16 DropLetMarkFlag;
extern float PWM_PERIOD_MAX;
extern float PWM_PERIOD_MIN;
extern float syncErrLimit;			//捕获成功标识
extern int cntForSyncOut;			// 发送翻转电平
extern int cntForAutoStIn30;
extern int cntForRepFaultA;
extern int cntForRepFaultB;
extern int cntForRepFaultC;
extern float negCurCompD ;
extern float negCurCompQ ;
extern float negCurCompPerc ;
extern float THD_K ;
extern float THD_B ;
extern float dcBusVtInc;
extern float WholeOutCurRmsLimit ;
extern int debugDispFlag ;
extern int OverVoltHarm;// 对谐波进行幅值和相位的校准
extern int VoltDQSource;
extern float NegaQPowerCalib;
extern float NegaDPowerCalib;
extern float ReactivePowerCurrCalib;
extern float ReactivePowerCurrCalibA;
extern float ReactivePowerCurrCalibB;
extern float ReactivePowerCurrCalibC;
extern float ReactivePowerCurrCalibQ;
extern float NegaDPowerCalibD;
extern float NegaQPowerCalibQ;
extern float UbanCompLoadCurND;
extern float UbanCompLoadCurNQ;
extern float ActivePowerCurrCalib;
extern float ActivePowerCurrCalibPH;
extern float PllCalibrPhase;
extern int16 VoltLeadPoints;
extern int16 TestMU_RATIO;
//假死判断参数
extern int SpuriousIn,SpuriousOut;						//用于没有用到的IO口的假输入输出
extern float VolforwardA,VolforwardB,VolforwardC;
extern float APFReactPowerOut,APFReactPowerOutA,APFReactPowerOutB,APFReactPowerOutC;
extern float VoltDerivative;
extern  Uint16 SequenceJudgeTimes;
extern  Uint16 ReceiveFSM;
extern  Uint16 SendFSM;
extern  Uint16 StorageTypeFlag;
extern  Uint16 SoeRemoteBuf;
extern  Uint32 EleAD[6];
extern  Uint16 GfDebug1;
extern  Uint16 GfDebug2;
extern  Uint16 GfDebug3;
extern  Uint16 GfDebug4;
extern  float SeqJud[3];
extern  float DirJud[3];
extern float *pPwmV1;
extern float *pPwmV2;
extern float *pPwmV3;
extern float *pPwmVN1;
extern float *pPwmVN2;
extern float *pPwmVN3;
extern int Fan_speed_manual;

extern float AllCapCapacityPhA,AllCapCapacityPhB,AllCapCapacityPhC;


extern float RUICA,RUICB,RUICC;
//假死判断参数
extern int SpuriousIn,SpuriousOut;						//用于没有用到的IO口的假输入输出
extern Uint16 RmsCaluBlockCnt;
extern Uint16 RmsCaluCnt;
extern int16 calcuFlag;
extern const int FaultRecSelTab[4][8];
extern Uint16 WaveRecordHandler;
extern float32 RmsSumIn[ALL_RMSNUM],RmsSumOut[ALL_RMSNUM],RmsSumQ[RMS_CALU_SEGMENT_NUM][ALL_RMSNUM];
extern float TotalHarmDistorionGridCurr,TotalHarmDistorionLoadCurr,TotalHarmDistorionVolt;
extern Uint16 IOstate1;
extern Uint16 IOstate2;
extern int16 Choose1,Choose2,Choose3,Choose4,Choose5;
extern float xrand;

extern int BYPASS_CONTACT_CNT;

extern float GardVoltTarVal;
extern float InvCurUVRmsF;
extern float GardVoltIntissued;
extern int BY_Flag,BY_Flag1;

#if FIXED_CAP_EN == 1
extern int CSRPIncount,CSRPcount,CSRPIF;
extern int CSRPswitchC,CSRPswitchT;

typedef struct{
	Uint16 compenEn 				:1;	//Bit0:开启无功补偿              			1:关闭 0:打开
	Uint16 mode						:1;	//Bit1:开启条件						1:一直开启固定电容器组自动补偿,
										//                                  0:MEC装置启动后,开始固定电容器组自动补偿  2手动调试模式
	Uint16 ctrl						:1;	//Bit2:控制端口              				1:IO口方式 0:通信方式
	Uint16 unblance					:1;	//Bit3:开启分相补偿投切      			1:关闭 0:打开
	Uint16 handMode					:1;	//Bit4:手动模式						0:关闭 1:打开
	Uint16 rsvd						:1;	//Bit12~5:保留
	Uint16 groupFaltEn				:1;	//Bit13:电容器组故障              			1:关闭故障0:打开
	Uint16 commFaltEn				:1;	//Bit14:电容器组通信失败(吉林特纳普) 	1:关闭故障0:打开
	Uint16 overVoltHarmFaltEn		:1;	//Bit15:电压谐波超限保护           		1:关闭故障0:打开
	Uint16 overVoltFaltEn			:1;	//Bit15:过欠电压保护           			1:关闭故障0:打开
}Stru_EnCapSwitch;
typedef union{
	Uint16 all;
	Stru_EnCapSwitch B;
}Uni_EnCapSwitch;

extern Uni_EnCapSwitch EnCapSwitch;
extern int CapRunSwitchPower;
extern int CapRunSwitchBack;
#endif

typedef struct{
	Uint16 CapVarUnseal				:1;	//Bit0:开启无功补偿          1:关闭 0:打开
	Uint16 CapVarConditon			:1;	//Bit1:开启条件 1:一直开启固定电容器组自动补偿
										//0:MEC装置启动后,开始固定电容器组自动补偿
	Uint16 CommutionChose			:1;	//Bit2:控制端口              1:IO口方式 0:通信方式
	Uint16 DivedPhCapvar			:1; //Bit3:开启分相补偿投切      1:关闭 0:打开
	Uint16 CapHandSwitch			:1;	//Bit4:开启手动调试投切      1:关闭 0:打开
	Uint16 ProSlect					:1; //Bit5:优先选项;0:SVG优先 1:电容器优先
	Uint16 CapOvertempFault			:1;	//Bit6:电容器组过温故障           1:关闭故障0:打开
	Uint16 CapFault					:1;	//Bit7:电容器组故障               1关闭故障0:打开
	Uint16 CapCommutionFault		:1;	//Bit8:电容器组通信失败(吉林特纳普) 1:关闭故障0:打开
	Uint16 VolHarmonicLimit			:1;	//Bit9:电压谐波超限保护           1:关闭故障0:打开
	Uint16 SlaveIdEnable			:1;	//Bit10:从机通信地址匹配使能      1:关闭匹配 0:打开
	Uint16 CapCompensation			:3;	//Bit11-13: 0:手动 1:顺序
	Uint16 rsvd						:2;	//保留
} CaGrSwi;
typedef union{
	Uint16 	all;
	CaGrSwi  B;
}Capswitchgroup;
struct CapSwitch{
	Uint16 PhactGroup[24];			//24组电容投切	下发
	Uint16 Capacity[24];			//24组电容容量	下发
	Uint16 rsvd1;
	Uint16 SwitchThreshold;			//投切门限		下发
	Uint16 TimSwitchOn;				//切投延时 s		下发
	Uint16 TimSwitchOff;			//投切延时 s		下发
	Capswitchgroup capgroupswitch;	//电容器组切换	下发
	Uint16 CapDivdNum;
	Uint16 SvgPercent;				//分相补偿位置/电容数量
	Uint16 HmiVolProt;
    Uint16 CapComuFaultFlag;        //通信故障标志为
	Uint16 rsvd2;//9
	Uint16 rsvd3;//A
	Uint16 rsvd4;//B
	Uint16 rsvd5;//C
	Uint16 rsvd6;//D
	Uint16 MasteRversion;			//主版本号下发	下发
	Uint16 SlaveRversion;			//子版本号下发	下发
	Uint16 CapStateUp[24];			//24组电容状态  	读取
	Uint16 HarmVolFault;
	Uint16 CRC;
};
extern struct CapSwitch CapGroupAct;
typedef enum
{
	STATE_EV_WRCTRL,
	STATE_EV_FAULT,
	STATE_EV_WRVER,
}CapSwitchEvent;
extern CapSwitchEvent CapEventFlag;

typedef struct{
	Uint16  FlashApiEnable				:1; //故障录波存储控制位 1:存入flash 0:不存储
	Uint16  FlashWriteReady				:1; //Flash写操作准备就绪标志位 1:可以执行写操作 0:不可执行
	Uint16  AutoRecordReadFlag			:1; //故障录波自动读取标志位
	Uint16	RemoteControlEn				:1;	//远程控制使能位
	Uint16  FlashBlock0                 :1; //Block0 使用标志位
	Uint16  FlashBlock1                 :1; //Block1 使用标志位

} FLSHSTATE;
typedef union{
	Uint16 	  all;
	FLSHSTATE  B;
}FlshSte;
struct STRU_FAURCD{
	FlshSte	FlashState;
	Uint16	UiBlockRead;			 //界面手动读取块位置
	int16   FlashBlockRead;		 	 //故障录波块读取位置
	Uint16  FlashBlockWrite;		 //故障录波块存储位置
	Uint16  RecordBlockNum;			 //故障录波块存储数量
	Uint16  WaveRecordHandler;       //故障录波索引号
	Uint16	CRC;
};
extern struct STRU_FAURCD RecordFlash;

struct STRU_CNT_MS{
	int inputStart;
	int inputStop; //停机按钮被持续按下的时长（单位：ms）。WY
	int IntTime;
	int LowVolDelay;
	int starting;
	int VolSurCount;
    int VolSurCountU;
    int VolSurCountV;
    int VolSurCountW;
	int contactorOpen;
	int contactorClose;
	int ESwitchbroken;
	int SelfJc;
	int StartDelay;
	int renewdelay;
	int VolSurDelay;			//越限持续时间
	int displayTimingCount;
	int HarmonicSendCount;
	int InitializationDelay;
	int GridRMSOverVolt_RlyProt;
	int PLLSafetyCount;
	int HFOverCurrentCount;
	int SendOrderDelay;
    int WatchdogDelay;
    int PowerOnForRechargeVoltage;
    int WIFIDelay;
    int UNCurrDelay1;
    int UNCurrDelay2;
};
extern struct STRU_CNT_MS CntMs;

struct STRU_CNT_SEC{
	int MeasureDelay;
	int VolComDelay;
	int GridVtIsOk;
	int WaitDelay;
	int SpecCstop;
	int EleHopDelay;
	int CapaSwithCount;
	int prechargeRefuseClose;
	int RespondCount;
	int cntForOutputCur;
	int cntForCharging;
	int cntForContractOpen;
	int cntForContractClose;
	int ForceRecordWaveRefreshCnt;
	int cntForCurMax;
	int OverTimeCount;
	int StopDelay;
	int BypassSwitch;
};
extern struct STRU_CNT_SEC CntSec;

struct STRU_STATE_FLAG
{
	Uint16 UserParamWrite :1;
	Uint16 ManuToProtecWrite :1;
	Uint16 SpecialParamWrite :1;
	Uint16 FactoryParamWrite :1;
	Uint16 HarmonicParamWrite :1;
	Uint16 CapDataParamWrite :1;
	Uint16 FactoryParamRefresh :1;
	Uint16 harmCompPercParamRefresh :1;
	Uint16 HarmonicWaveParamRefresh :1;
	Uint16 autoAdcOffsetFlag :1;
	Uint16 onceTimeAdcAutoAdjust :1; //ADC零偏校准标志位。0，使能；1，失能。WY
	Uint16 onceTimeStateMachine :1;
	Uint16 onceRunStartFlag :1;
	Uint16 realFaultFlag :1;
	Uint16 VolSurTimeFlag :1;
	Uint16 StabilizeFlag :1;
	Uint16 SoeFlag :1;
	Uint16 isStarted :1;
	Uint16 isStopped :1;
	Uint16 isFault :1;
	Uint16 doFaultClear :1;
	Uint16 doStop :1;
	Uint16 doStart :1;
	Uint16 waitFlag :1;
	Uint16 stopwait :1;
	Uint16 doWarningCancelSound :1;
	Uint16 negCurCompFlag :1;//负序补偿开启
	Uint16 harmCompEn :1;//谐波补偿开启
	Uint16 reactPrCompFlag :1;//无功补偿开启
	Uint16 AVCPrCompFlag :1;//AVC补偿开启
	Uint16 VoltcompenFlag :1;//电压补偿开启
	Uint16 VoltageModeFlag :1; //升降压模式选择。0：升压；1：降压。WY
	Uint16 apfWiringmethod :1;
	Uint16 startingMethod :1;
	Uint16 isHarmCompensateMode :1;//0发谐波模式 1补偿
	Uint16 ManualUbanCurFlag :1;//厂家设置手动发不平衡模式
	Uint16 positionCT :1;
	Uint16 spwmFlag :1;
	Uint16 DynamicProportionEn :1;
	Uint16 VoltFilterEn :1;
	Uint16 SVPWMEn :1;
	Uint16 InvCurUVRmsReadyFlag :1;
	Uint16 prioritizedModeFlag :2;//Bit10~12：优先设定,0,1:无功优先2:谐波优先3:不平衡优先
	Uint16 modeChoosedFlag :4;// 模式选择
	Uint16 constantQFlag :2;//Bit6~7：无功补偿模式,	0:恒无功功率  1:恒无功电流
//		 				2:恒功率因数  3:关闭无功补偿

	/*
	 * 电压模式。WY
	 * 0：升压；
	 * 1：降压；
	 * 2：升降压（保留）；
	 * 3：不平衡电流补偿。
	 */
	Uint16 VoltageMode :3; //

//	Uint16 autoStFlag				:4;
	Uint16 startFlag :1;
	Uint16 stopFlag :1;
	Uint16 resetFlag :1;
//	Uint16 faultFlag				:1;
	Uint16 selfJcOkFlag :1;
	Uint16 RespondFlag :5;
	Uint16 LEDRunFlag :1;
	Uint16 resetFaultOnceTime :1;
	Uint16 EEPROMResourceLock :1;
	Uint16 onceOneMinFlag :1;
	Uint16 RxcFlag :1;
	Uint16 SendFlag :1;
	Uint16 NegativeSequenceFlag :1;
	Uint16 CurrACountFlag :1;
	Uint16 CurrBCountFlag :1;
	Uint16 CurrCCountFlag :1;
	Uint16 DelayCalculationFlag :1;
	Uint16 MakeEnergyFlag :1;
//	Uint16 ForceRecordWaveRefresh	:1;		//界面波形显示刷新
	Uint16 ForceRMSRefresh :1;//多机并联均衡有效值刷新
	Uint16 FactoryMode :4;//测试放电
	Uint16 RechargeUdcFullFlag :1;//直流电容充电已冲饱
	Uint16 EnTransformRatioMode :1;//高压侧变压器使能
	Uint16 TransformRatioMode :3;//高压侧变压器组别模式
	Uint16 cntForTHDi :1;
	Uint16 PLLSafetyFlag :1;
	Uint16 HFOverCurrentFlag :1;
	Uint16 startOnecRec :1;//
	Uint16 MainContactOnShadow :1;
	Uint16 MainContactEnanle :1;
	Uint16 dcBusVtIncStart :1;
	Uint16 CapEmergShutFlag :1;
	Uint16 StateInstruction :3;
	Uint16 DirectionJudgeFail :1;
	Uint16 PhaseSequeJudSucceedFlag :1;
	Uint16 DirectionJudFailFlag :1;
	Uint16 ManufacturerParameters :1;
	Uint16 VitruZeroParameters :1;
	Uint16 UserPreferences :1;
	Uint16 SequenceAutoFlag :1;
	Uint16 PowerFactor :2;
	Uint16 SOEFullFlag :1;
	Uint16 IDPreferences :1;
	Uint16 WatchdogFlag :1;
	Uint16 RecordWritting :1;
	Uint16 RemoteWriteHoldFlag :1;
	Uint16 RemoteWriteCoilFlag :1;
	Uint16 CapStateFlag :1;
	Uint16 StartEraseFlag :1;
	Uint16 StartFlashFlag :1;
	Uint16 RecordStates :1;
	Uint16 FlashRecordFlag :1;
	Uint16 FanctrlTestFlag :1;
	Uint16 LowPowerFlag :1;
	Uint16 LowPowerStart :1;
	Uint16 TempSoeRecord :1;
	Uint16 RechageskipstopFlag :1;
};
extern struct STRU_STATE_FLAG StateFlag;

typedef struct{
    Uint16 REC_HF               :1;//bit0
    Uint16 REC_IGBT_EN          :1;//bit1
    Uint16 REC_IGBT_FAULT       :1;//bit2
    Uint16 REC_DC_OVER_VOLT     :1;//bit3
    Uint16 REC_CTRDIR           :1;//bit4
    Uint16 REC_OVER_CURRENT     :1;//bit5
    Uint16 REC_FFT_MODE         :1;//bit6
    Uint16 REC_DC_START         :1;//bit7
    Uint16 REC_StateEventFlag   :4;//bit8-11
    Uint16 REC_12               :1;//bit12
    Uint16 REC_13               :1;//bit13
    Uint16 REC_14               :1;//bit14
    Uint16 REC_15               :1;//bit15
}RECORD;

typedef union{
    Uint16  all;
    RECORD   B;
}STRU_ERR_RECORD;

extern STRU_ERR_RECORD ErrorRecord;

// 采样
typedef struct{
	float LoadCurRatioA;
	float LoadCurRatioB;
	float LoadCurRatioC;
	float dcBusVoltUp;
	float dcBusVoltDn;
	float temperature3;
	float temperature2;
	float temperature1;
	float temperature0;
	float temDspBroad;
	float gridVoltAB;
	float gridVoltBC;
	float gridVoltCA;
	float loadCurrentA;
	float loadCurrentB;
	float loadCurrentC;
	float apfOutputCurA;
	float apfOutputCurB;
	float apfOutputCurC;
	float GridVoltDevcoef;
	float DcBusVUpDevcoef;
	float DcBusVDnDevcoef;
	float ApfOutCuDevcoef;
	float CTOutCurDevcoef;

}VirtulADStru;
extern VirtulADStru VirtulAD;

typedef struct{
    int16 *GridHVoltA; //A相高压侧电压。WY
    int16 *GridHVoltB; //B相高压侧电压。WY
    int16 *GridHVoltC; //C相高压侧电压。WY

    int16 *GridLVoltA; //A相低压侧电压。WY
    int16 *GridLVoltB; //B相低压侧电压。WY
    int16 *GridLVoltC; //C相低压侧电压。WY

    int16 *GridMainCurA; //A相低压侧电流。WY
    int16 *GridMainCurB; //B相低压侧电流。WY
    int16 *GridMainCurC; //C相低压侧电流。WY

    int16 *GridBypassCurA; //A相高压侧电流。WY
    int16 *GridBypassCurB; //B相高压侧电流。WY
    int16 *GridBypassCurC; //C相高压侧电流。WY

    int16 *ADCUDCA; //A相直流电容电压。WY
    int16 *ADCUDCB; //B相直流电容电压。WY
    int16 *ADCUDCC; //C相直流电容电压。WY
}VirtulADStruVAL;
extern VirtulADStruVAL VirtulADVAL;

typedef struct{                  //程序当中使用的零偏值,和界面下发的零偏值差一个倍率
    float gridHVoltA;
    float gridHVoltB;
    float gridHVoltC;

    float gridLVoltA;
    float gridLVoltB;
    float gridLVoltC;

    float gridMainCurA;
    float gridMainCurB;
    float gridMainCurC;

    float gridBypassCurA;
    float gridBypassCurB;
    float gridBypassCurC;

    float aDCUDCA;
    float aDCUDCB;
    float aDCUDCC;
}VirtulADStruval;
extern VirtulADStruval VirtulADval;

struct ESC_STRU_CNT_MS{
    int ContactorDelay; //用于检测旁路磁保持继电器状态的延时（单位：ms）。目的：周期内多次检测，防止误测。WY
    int PllDelay; //给锁相环锁相分配的时长（单位：ms）。WY
    int HarmDisDelay;      //ESC电网谐波畸变延时
    int FaultDelayFlag;
    int StartDelay;
    int SelfJc;
    int CutCurrDelay;
};
extern struct ESC_STRU_CNT_MS ESCCntMs;

struct ESC_STRU_CNT_SEC{
    int ChargingTime; //预充电时长（单位：s）。WY
    int FunContDelay;
    int R_SSwiTiDelay; //ESC设备从【运行】状态转入【待机】状态的必要时长（单位：s）。WY
    int S_RSwiTiDelay; //电网电压欠压时长（单位：s）。WY
    int RelayDISDelay;   //ESC山东鲁软现场单独外加继电器控制
    int PRECHARGEDelayBY; //预充电时，用于对旁路磁保持继电器状态检测的延时。WY //防止上电因运输工程中磁保持误动作而炸机
    int PRECHARGEDelay; //预充电时，用于对主路磁保持继电器状态检测的延时。WY //防止上电因运输工程中磁保持误动作而炸机
    int HWPowerSupDelay; //关闭15V电源所必要的时长（单位：s）。WY
    int HWPowerStopDelay;
    int HWPowerFaultDelay;
    int UNCurrDelay1;
    int UNCurrDelay2;
};
extern struct ESC_STRU_CNT_MS ESCCntSec;

/*
 * X相控制参数。WY
 * X的取值集合为{A, B, C}
 * */
typedef struct
{
	struct ESC_STRU_CNT_SEC ESCCntSec;
	struct ESC_STRU_CNT_MS ESCCntMs;
	Uint16 onceTimeStateMachine :1; //单次状态检测。0，检测已完成；1，等待检测中。WY
	Uint16 ByPassContactFlag :1; //旁路磁保持继电器状态。0，断开；1，闭合。该标志位在ADC-ISR中被操作。WY
	Uint16 ContactorFeedBackFlag :1; //主路磁保持继电器状态。0，断开；1，闭合。该标志位在ADC-ISR中被操作。WY
	Uint16 VoltageModeFlag :1;
	Uint16 FaultskipFlag :1; //故障跳转标志位。0，无需跳转至故障停机状态（上电默认）；1，需要跳转至故障停机状态。WY
	Uint16 faultFlag :1; //设备故障标志位。0，不存在故障（上电默认）；1，存在故障。WY
	Uint16 resetFlag :1;
	Uint16 stopFlag :1; //设备停机标志位。0，设备应当处于未停机状态；1，设备应当处于停机状态。WY
	Uint16 startFlag :1; //设备启动标志位。0，设备应当处于未运行状态；1，设备应当处于运行状态。WY
	Uint16 autoStFlag :4; //自动重启的次数？WY
	Uint16 realFaultFlag :1;
	Uint16 FunContDelayFlag :1; //保留,.WY
	Uint16 PWMcurrDirFlag :1; //保留,.WY
	Uint16 PeakStopFlag :1; //保留,.WY
	Uint16 onceRunStartFlag :1;
	Uint16 FAULTCONFIRFlag :1; //故障信号标志位。0，不存在故障信号；1，存在故障信号。WY
	Uint16 DelayFlag :1; //在特定情况下，使得判断条件恒成立。WY
	Uint16 StateSkipFlag :1;
	Uint16 RELAYCONTROLFlag :1; //操作继电器标志位。0，操作完成；1，等待操作。WY
	Uint16 TurnOffPeakFaultFlag :1;
	Uint16 HWPowerSupFlag :1;
	Uint16 HWPowerSTOPFlag :1; //硬件电源停机标志位。？？？0，未停机；1，停机。WY
	Uint16 HWPowerFAULTFlag :1; //硬件故障标志位？。0，不存在故障；1，存在故障。WY
	Uint16 FaultJudgeFlag :1;
	Uint16 BYFEEDBACKFLAG :1; //旁路状态标志位？。0，异常；1，正常。WY

	float VoltIn_rms; //电网电压有效值。WY
	float gridCur_rms; //电网电流有效值。WY
	float ESC_DutyData; //占空比。WY
	float VoltOut_rms;
	float gridCurrBY_rms; //旁路电流有效值。WY

	int PHASE;
	int StableValue; //电网电压稳定值。WY

	int VoltThreShold; //电压门限值。WY

	int Backdiffvalue;
	int PWM_ins_index; //用于同步PWM输出？0，主路调制；1，主路直通。WY
	int PllDelay;
	int EscWaitRunDelayCnt;
	int EscStandyDelayCnt1; //ESC设备处于运行状态时。从主路调制转为主路直通的必要时长。WY
	int EscStandyDelayCnt2; //ESC设备处于运行状态时。从主路直通转为主路调制的必要时长。WY
	int Volttarget;//目标值
} ESCCTRLVALFLAG;
extern ESCCTRLVALFLAG ESCFlagA,ESCFlagB,ESCFlagC;

typedef struct {
	float UNIT_OVER_TEMP ; 				// 散热片温度限制
	float BOARD_OVER_TEMP ; 			// 散热片温度限制
	float INDUCE_OVER_TEMP; 			// 电抗器线圈温度限制
	float MotherBoardTempterature;		// 板载温度
	float HeatSinkTempterature;			// 散热片温度
	float EnvirTemperature;              //环境温度
	float windColdStartTempterature;	// 散热片传感器控制风机启动温度
	float windColdStopTempterature;		// 散热片传感器控制风机停止温度
	float MotherBoardStartTempterature;	// 板载传感器控制风机启动温度
	float MotherBoardStopTempterature;	// 板载传感器控制风机停止温度
	float ReduceCapacityHeatSink;		// 散热片降容模式温度
	float BackReduceCapacityHeatSink;	// 降容模式恢复温度
	float ReduceCapacityMotherBoard;	// 板载降容模式温度
	float BackReduceCapacityMotherBoard;// 降容恢复恢复温度
	Uint16 ColdFunCnt;
	Uint16 windColdCtrMode;				// 风机运行模式1:一直开启 0,受温度传感器控制
}STRU_WinCold;
extern STRU_WinCold WindCold;


struct STRU_SCI{
	volatile struct SCI_REGS *pReg;
	const Uint16 *TxDealBuffStart;		//Send Deal buffer start position
	const Uint16 *RxDealBuffStart;		//Receive Deal buffer start position
	const Uint16 *TxBuffStart;			//Send buffer start position
	const Uint16 *RxBuffStart;			//Receive buffer start position
	volatile Uint16 *TxBuffPos;					//Send buffer position
	volatile Uint16 *RxBuffPos;					//Received buffer position
	volatile Uint16 ReceivedNum;
	volatile Uint16 TA_Count;
	volatile Uint16 ROvertime;					//Receive Overtime count
	volatile Uint16 R_OVERTIME;					//Receive Overtime,units:ms
	volatile Uint16 dealTCount;
	volatile Uint16 TxLen;
	Uint16 UartPeripheralPort		:2;	//Peripheral port selection.0:232(Bluetooth),GPIO35,36,Peripheral is SCI-A,
										//	1:485-1(User customization),GPIO62,63,37,Peripheral is SCI-C
										//	2:485-2(Local LCD screen),GPIO14,15,53,Peripheral is SCI-B
	Uint16 baud						:4;	//baud rate 0:9600 1:115200 2~15:null
	Uint16 Baud_State				:1;	//Baud rate auto initialization
	Uint16 UartAddrMachingEn		:1;	//Uart Slave communication address matching enable 1:disable 0:enable
	Uint16 BlockTxMode				:1;	//compatibility mode,The previous code has no mirror sending buffer, the new code (some function code) has dealT as the processing mirror,
										//and the SCIA_TA is the transmit buffer 0:old 1:new
	Uint16 State 					:2;	//state machine
	Uint16 DetctedHead				:1;	//The first character of communication packets is detected
	Uint16 isReceivedData			:1;	//Data received
};
extern struct STRU_SCI SCI1,SCI2,SCI3;

typedef union{
	Uint16 all;
	struct{
		Uint16 SlaveID				:8; 		//从机ID 只有从机有效
		Uint16 SciType 				:4;			//SCI功能选择
		Uint16 BaudRate				:4;			//SCI波特率
	}B;
}INITCSI;

typedef struct{
	Uint16 SendCount;
	Uint16 Send4bCount;
	Uint16 Send5bCount;
	Uint16 Send5bNumber;
	Uint16 Conversion5bCount;
	Uint16 ReceiveCount;
	Uint16 Receive4bCount;
	Uint16 Receive5bCount;
	Uint16 Receive8bCount;
}Stru_SyncInformation;
extern Stru_SyncInformation SyncInformation;

typedef struct{
	Uint16 Sendbuff1	:1;
	Uint16 Sendbuff2	:1;
	Uint16 Sendbuff3	:1;
	Uint16 Sendbuff4	:1;
	Uint16 Sendbuff5	:1;
}Buff;

typedef union{
	Buff B;
	Uint16 all			:5;
}Stru_InformationMemory;
extern Stru_InformationMemory InformationMemory[200];

typedef struct{
	float32 im;
	float32 re;
	float32 rms;
	float32 THD;
}STRU_HarmTHD;
extern STRU_HarmTHD HarmTHD[HARM_CALU_NUM];

//---SOE
typedef struct{
	Uint16 Month 		:8; 			//bit0~7:月
	Uint16 Year 		:8; 			//bit8~15:年
	Uint16 Hour 		:8; 			//bit0~7:时
	Uint16 Day 			:8; 			//bit8~15:日
	Uint16 Second 		:8; 			//bit0~7:秒
	Uint16 Minute 		:8; 			//bit8~15:分
	Uint16 MicroSec; 					//毫秒
}Stru_Time;
extern Stru_Time Time;

typedef struct{
	Stru_Time RecordTime; 						//故障时间
	Uint16 FaultNumber;							//故障码
	Uint16 WaveRecordHandler;					//录波索引
	Uint16 reserved1;							//reserved1
	Uint16 reserved2;							//reserved2
	Uint16 IOstate1;							//IO量1
	Uint16 IOstate2;							//IO量2
	int GridVoltRms; 						//电网电压有效值
	int LoadCurRms_F; 						//负载电流
	int ApfOutCurRms_F; 						//输出电流
	int dcVoltF; 							//直流电压+-
    int HeatSinkTempterature;                   //散热器温度
	int EnvirTemperature;				        //环境温度
	Uint16 gridFreq;							//电网频率
	int GridActPower;							//有功功率
	int GridReactPower;							//无功功率
	int GridCosFi;								//功率因数
	int dcVoltDnF;	 						//直流电压+N
	int storageUsed;							//已用容量
	int SOETest1; 								//自定义1
	int gpVoltA_rms; 						//电网A相电压有效值
	int gpVoltB_rms; 						//电网B相电压有效值
	int gpVoltC_rms; 						//电网C相电压有效值
	int loadCurA_rms; 						//A相负载电流有效值
	int loadCurB_rms; 						//B相负载电流有效值
	int loadCurC_rms; 						//C相负载电流有效值
	int apfOutCurA_rms; 						//A相输出电流有效值
	int apfOutCurB_rms; 						//B相输出电流有效值
	int apfOutCurC_rms; 						//C相输出电流有效值
	int gridCurA_rms; 						//A相电网电流有效值
	int gridCurB_rms; 						//B相电网电流有效值
	int gridCurC_rms; 						//C相电网电流有效值
	int GridVoltHvRms; 						//高压侧电网电压有效值
	int gridCurHv_rms; 						//高压侧电网电流有效值
	int LoadCurHvRms_F; 						//高压侧负载电流有效值
	int ApfOutCurHvRms_F; 					//高压侧输出电流有效值
	int THDu_Grid; 							//电网电压畸变率
	int THDi_Grid; 							//电网电流畸变率
	int THDi_Load; 							//负载电流畸变率
}Stru_IncidentRecord;
extern Stru_IncidentRecord IncidentRecord[SOE_BUFF_GROUP];

typedef struct{
	Uint16 PastFault;
	Uint16 RecordFaultNumber;
	Uint16 CurrentPointerPosition;
	Uint16 WriteLength;
	Uint16 BufferStoragePointer;
	Uint16 BufferWritePointer;
}Stru_SOE;
extern Stru_SOE SOE;

typedef struct{
	Uint16 ChannelPoints;
	Uint16 ChannelNumber;
}Stru_SOERead;
extern Stru_SOERead SOERead;

typedef struct{
	Uint16 VoltPhaseSequen			:1; 		//Bit0：		电压相序
	Uint16 PhaseSequenCT			:3;			//Bit1~3：	CT相序
	Uint16 DirectionCT				:3;			//Bit4~6：	CT方向
	Uint16 DefectCT					:2;			//Bit7~8：	CT缺失
    Uint16 LoadingCondition         :1;         //Bit9：         负载条件
	Uint16 VoltPhaseSequenFailFlag	:1;			//Bit10：	电压相序校正失败标志
	Uint16 PhaseSequenCTFailFlag	:1;			//Bit11：	CT相序校正失败标志
	Uint16 DirectionCTFailFlag		:1;			//Bit12：	CT方向校正失败标志
	Uint16 OverTimeFlag				:1;			//Bit13：	校正超时标志
	Uint16 PhaseSequenResult		:2;			//Bit14~15：	校正失败故障标志
}Stru_InferRead;

typedef union{
	Uint16 all;
	Stru_InferRead B;
}Uni_InferRead;

typedef union
{
    Uint16                       	all;
    struct
    {
    	Uint16 RunState_A   	      : 4;   	// 0-3位    A相运行状态
        Uint16 RunState_B             : 4;      // 4-7位    B相运行状态
        Uint16 RunState_C             : 4;      // 8-11位  C相运行状态
    	Uint16 AutoRecordReadFlag	  : 1;	    // 12位 故障录波自动读取标志位
     }B;
}Uni_State;
extern Uni_State StateEventFlag1;			//工作状态

//---modbus读点表
typedef struct{
	Uint16 VoltTheta;      						//锁相角度
	int apfOutCurA_rmsF;      				//逆变侧A相电流
	int apfOutCurB_rmsF;      				//逆变侧B相电流
	int apfOutCurC_rmsF;      				//逆变侧C相电流
	int loadCurA_rmsF;      					//负载侧A相电流
	int loadCurB_rmsF;      					//负载侧B相电流
	int loadCurC_rmsF;      					//负载侧C相电流
	int gridCurA_rmsF;      					//电网侧A相电流
	int gridCurB_rmsF;      					//电网侧B相电流
	int gridCurC_rmsF;      					//电网侧C相电流
	int THDu_GridAF;      					//电网电压A畸变率
	int THDu_GridBF;      					//电网电压B畸变率
	int THDu_GridCF;      					//电网电压C畸变率
	int THDi_LoadAF;      					//负载侧电流A畸变率
	int THDi_LoadBF;      					//负载侧电流B畸变率
	int THDi_LoadCF;      					//负载侧电流C畸变率
	int THDi_GridAF;      					//电网侧电流A畸变率
	int THDi_GridBF;      					//电网侧电流B畸变率
	int THDi_GridCF;      					//电网侧电流C畸变率
	int rvsd1[8];      						//备用1
	int HarmonicCorrectionResult;            //谐波校正结果//0x9090
	Uni_InferRead Correction;					//自动相序校正
	int rvsd2[6];      						//备用2

	int GridVoltRms;							//ESC修改   :ABC总的电网电压有效值  0x9098
	int GridCurRms;							//ABC总的电网电流有效值
	int LoadVoltRms; 						//ABC总的负载电压有效值
	int LoadCurRms; 						//ABC总的负载的电流有效值
	int VoltInA_rms; 						//A相电网电压有效值
	int VoltInB_rms; 							//B相电网电压有效值
    int VoltInC_rms;                   //C相电网电压有效值
	int VoltOutA_rms;			        //A相电网电流有效值
	int VoltOutB_rms;							//B相电网电流有效值
	int VoltOutC_rms;							//C相电网电流有效值
	int gridCurA_rms;						//A相负载电压有效值
	int gridCurB_rms;								//B相负载电压有效值
	int gridCurC_rms;	 						//C相负载电压有效值
	int loadCurA_rms;							//A相负载电流有效值
	int loadCurB_rms;						//B相负载电流有效值
	int loadCurC_rms; 						//C相负载电流有效值
	int HeatSinkTempterature; 						//散热器温度
	int EnvirTemperature; 						//环境温度
	int gridFreq;                      //电网频率

	int LoadVoltV_rms;                      //esc中以下未用
	int LoadVoltW_rms;                      //C相负载电压有效值
	int apfOutCurA_rms; 						//A相输出电流有效值
	int apfOutCurB_rms; 						//B相输出电流有效值
	int apfOutCurC_rms; 						//C相输出电流有效值
	int GardCurA_rms; 						//
	int GardCurB_rms; 						//
	int GardCurC_rms; 						//
	int GridVoltHvRms; 						//高压侧电网电压有效值
	int gridCurHv_rms; 						//高压侧电网电流有效值
	int LoadCurHvRms_F; 						//高压侧负载电流有效值
	int ApfOutCurHvRms_F; 					//高压侧输出电流有效值
	int THDu_Grid; 							//电网电压畸变率
	int THDi_Grid; 							//电网电流畸变率
	int THDi_Load; 							//负载电流畸变率
	int THDu_GridA; 							//电网电压畸变率A
	int THDu_GridB; 							//电网电压畸变率B
	int THDu_GridC; 							//电网电压畸变率C
	int THDi_GridA; 							//电网电流畸变率A
	int THDi_GridB; 							//电网电流畸变率B
	int THDi_GridC; 							//电网电流畸变率C
	int THDi_LoadA; 							//负载电流畸变率A
	int THDi_LoadB; 							//负载电流畸变率B
	int THDi_LoadC; 							//负载电流畸变率C
	int gridCur_rms;							//电网电流
	int gridCurN_rms; 						//N相电网电流有效值
	int loadCurN_rms; 						//N相负载电流有效值
	int GridActPowerA;
	int GridActPowerB;
	int GridActPowerC;
	int GridReactPowerA;
	int GridReactPowerB;
	int GridReactPowerC;
	int ApparentPowerA;
	int ApparentPowerB;
	int ApparentPowerC;
	int GridCosFiA;								//电网功率因数A
	int GridCosFiB;								//电网功率因数B
	int GridCosFiC;								//电网功率因数C
	int LoadActPowerA;
	int LoadActPowerB;
	int LoadActPowerC;
	int LoadReactPowerA;
	int LoadReactPowerB;
	int LoadReactPowerC;
	int LoadApparentPowerA;
	int LoadApparentPowerB;
	int LoadApparentPowerC;
	int LoadCosFiA;								//负载功率因数A
	int LoadCosFiB;								//负载功率因数B
	int LoadCosFiC;								//负载功率因数C
	int rvsd3;      							//备用2
	Uint16 RecordBlockNum;
	Uint16 StateEventFlag;						//工作状态
	Uint16 StartMode;							//启动方式
	Uint16 modeChoosedFlag;						//工作模式
	Uint16 FaultCharacter1;						//故障字1
	Uint16 FaultCharacter2;						//故障字2
	Uint16 FaultCharacter3;						//故障字3
	Uint16 FaultCharacter4;						//故障字4
	Uint16 FaultCharacter5;						//故障字5
//    Uint16 FaultCharacter6;                     //故障字6
	int16  choose1;
	int16  choose2;
	int16  choose3;
	int16  choose4;
	int16  choose5;
	Uint16 SlaveSoftWare5;                      //从机版本:开关频率
	Uint16 SlaveSoftWare1;
	Uint16 SlaveSoftWare2;
	Uint16 SlaveSoftWare3;
	Uint16 SlaveSoftWare4;
	Uint16 CommuVersion;
	Uint16 UbanRmsPerc;							//电网电流不平衡度
	int HV_gpVoltA_rms; 						//高压侧电网A相电压有效值
	int HV_gpVoltB_rms; 						//高压侧电网B相电压有效值
	int HV_gpVoltC_rms; 						//高压侧电网C相电压有效值
	int HC_gridCurA_rms; 					//高压侧A相电网电流有效值
	int HC_gridCurB_rms; 					//高压侧B相电网电流有效值
	int HC_gridCurC_rms; 					//高压侧C相电网电流有效值
	int HC_apfOutCurA_rms; 					//高压侧A相输出电流有效值
	int HC_apfOutCurB_rms; 					//高压侧B相输出电流有效值
	int HC_apfOutCurC_rms; 					//高压侧C相输出电流有效值
	int HC_loadCurA_rms; 					//高压侧A相负载电流有效值
	int HC_loadCurB_rms; 					//高压侧B相负载电流有效值
	int HC_loadCurC_rms; 					//高压侧C相负载电流有效值
}Stru_Information_Structure;
extern Stru_Information_Structure Information_Structure;

typedef struct{
	Stru_Time SoeTime;
	Uint16	  FaultNum;
	int16	  FaultValue1;
	int16	  FaultValue2;
	int16	  FaultValue3;
}STRU_SoeRemote;

typedef struct{                 //后台  功能码 0x03
    int16 VolttargetA;                       //A相电压目标值
    int16 VolttargetB;
    int16 VolttargetC;
    int16 VoltThreSholdA;                    //A相电压门限值
    int16 VoltThreSholdB;
    int16 VoltThreSholdC;
    int16 Backdiffvalue;                     //电压回差值
	int16 GridVoltRms; 						//电网电压有效值
	int16 GridCurRms;                      //电网电流有效值
	int16 LoadVoltRms;                     //负载电流有效值
    int16 HeatSinkTempterature;                   //散热器温度
	int16 EnvirTemperature;				        //环境温度
	int16 gridFreq;							//电网频率
	int16 GridActPower;							//有功功率
	int16 GridReactPower;						//无功功率
	int16 GridCosFi;								//功率因数
	int16 dcVoltDnF;	 						//直流电压+N
	int16 storageUsed;							//已用容量
	int16 ApparentPower;						//视在功率
	int16 gpVoltA_rms; 						//电网A相电压有效值
	int16 gpVoltB_rms; 						//电网B相电压有效值
	int16 gpVoltC_rms; 						//电网C相电压有效值
	int16 loadCurA_rms; 						//A相负载电流有效值
	int16 loadCurB_rms; 						//B相负载电流有效值
	int16 loadCurC_rms; 						//C相负载电流有效值
	int16 apfOutCurA_rms; 						//A相输出电流有效值
	int16 apfOutCurB_rms; 						//B相输出电流有效值
	int16 apfOutCurC_rms; 						//C相输出电流有效值
	int16 gridCurA_rms; 						//A相电网电流有效值
	int16 gridCurB_rms; 						//B相电网电流有效值
	int16 gridCurC_rms; 						//C相电网电流有效值
	int16 GridVoltHvRms; 						//高压侧电网电压有效值
	int16 gridCurHv_rms; 						//高压侧电网电流有效值
	int16 LoadCurHvRms_F; 						//高压侧负载电流有效值
	int16 ApfOutCurHvRms_F; 					//高压侧输出电流有效值
	int16 THDu_Grid; 							//电网电压畸变率
	int16 THDi_Grid; 							//电网电流畸变率
	int16 THDi_Load; 							//负载电流畸变率
	int16 THDu_GridA; 							//电网电压畸变率A
	int16 THDu_GridB; 							//电网电压畸变率B
	int16 THDu_GridC; 							//电网电压畸变率C
	int16 THDi_GridA; 							//电网电流畸变率A
	int16 THDi_GridB; 							//电网电流畸变率B
	int16 THDi_GridC; 							//电网电流畸变率C
	int16 THDi_LoadA; 							//负载电流畸变率A
	int16 THDi_LoadB; 							//负载电流畸变率B
	int16 THDi_LoadC; 							//负载电流畸变率C
	int16 gridCur_rms;							//电网电流
	int16 gridCurN_rms; 						//N相电网电流有效值
	int16 loadCurN_rms; 						//N相负载电流有效值
	int16 GridActPowerA;
	int16 GridActPowerB;
	int16 GridActPowerC;
	int16 GridReactPowerA;
	int16 GridReactPowerB;
	int16 GridReactPowerC;
	int16 ApparentPowerA;
	int16 ApparentPowerB;
	int16 ApparentPowerC;
	int16 GridCosFiA;								//电网功率因数A
	int16 GridCosFiB;								//电网功率因数B
	int16 GridCosFiC;								//电网功率因数C
	int16 LoadActPowerA;
	int16 LoadActPowerB;
	int16 LoadActPowerC;
	int16 LoadReactPowerA;
	int16 LoadReactPowerB;
	int16 LoadReactPowerC;
	int16 LoadApparentPowerA;
	int16 LoadApparentPowerB;
	int16 LoadApparentPowerC;
	int16 LoadCosFiA;								//负载功率因数A
	int16 LoadCosFiB;								//负载功率因数B
	int16 LoadCosFiC;								//负载功率因数C
	int16 UbanRmsPerc;							//电网电流不平衡度
	int16 HV_gpVoltA_rms; 						//高压侧电网A相电压有效值
	int16 HV_gpVoltB_rms; 						//高压侧电网B相电压有效值
	int16 HV_gpVoltC_rms; 						//高压侧电网C相电压有效值
	int16 HC_gridCurA_rms; 					//高压侧A相电网电流有效值
	int16 HC_gridCurB_rms; 					//高压侧B相电网电流有效值
	int16 HC_gridCurC_rms; 					//高压侧C相电网电流有效值
	int16 HC_apfOutCurA_rms; 					//高压侧A相输出电流有效值
	int16 HC_apfOutCurB_rms; 					//高压侧B相输出电流有效值
	int16 HC_apfOutCurC_rms; 					//高压侧C相输出电流有效值
	int16 HC_loadCurA_rms; 					//高压侧A相负载电流有效值
	int16 HC_loadCurB_rms; 					//高压侧B相负载电流有效值
	int16 HC_loadCurC_rms; 					//高压侧C相负载电流有效值
	Uint16 StateEventFlag;						//工作状态
	Uint16 StartMode;							//启动方式
	Uint16 modeChoosedFlag;						//工作模式
	Uint16 FaultCharacter1;						//故障字1
	Uint16 FaultCharacter2;						//故障字2
	Uint16 FaultCharacter3;						//故障字3
	Uint16 FaultCharacter4;						//故障字4
	Uint16 FaultCharacter5;						//故障字5
	Uint16 CapStateUp[18];
//	Stru_Time RemoteTime;
//	STRU_SoeRemote	SoeRemote[30];
}Stru_RemoteRefresh_Structure;
extern Stru_RemoteRefresh_Structure RemoteRefreshData;

typedef struct{        //后台  功能码 0x05
    Uint16 RemoteStart;
    Uint16 RemoteStop;
    Uint16 RemoteReset;
}Stru_RemoteCtrl_Structure;
extern Stru_RemoteCtrl_Structure RemoteStateCtrl;

typedef struct{        //后台  功能码 0x06
//    int16 ReactpowerOrder;
//    int16 Voltuplimit;
//    int16 Voltlowlimit;
//    int16 Voltoverlimitcompentarvalue;
//    int16 Voltunderlimitcompentarvalue;
    int16 VolttargetA;             //A相电压目标值
    int16 VolttargetB;
    int16 VolttargetC;
    int16 VoltThreSholdA;          //A相电压门限值
    int16 VoltThreSholdB;
    int16 VoltThreSholdC;
    int16 Backdiffvalue;           //回差值

}Stru_RemoteInstruct_Structure;
extern Stru_RemoteInstruct_Structure RemoteInstructCtrl;

typedef struct{        //后台  功能码 0x02
    Uint16 StateRunStop;
    Uint16 StateWait;
    Uint16 StateFault;
    Uint16 rsvd[13];
    Uint16 ESCFastHardwareOverCurFlagA;            // 0位，ESC快速硬件过流故障A
    Uint16 ESCFastHardwareOverCurFlagB;            // 1位，ESC快速硬件过流故障B
    Uint16 ESCFastHardwareOverCurFlagC;            // 2位，ESC快速硬件过流故障C
    Uint16 ESCInsOverCurFlagA;                     // 3位，ESC瞬时值过流故障A
    Uint16 ESCInsOverCurFlagB;                     // 4位, ESC瞬时值过流故障B
    Uint16 ESCInsOverCurFlagC;                     // 5位，ESC瞬时值过流故障C
    Uint16 ESCDcCapVoltOverVoltFlagA;              // 6位，ESC直流电容电压过压故障A
    Uint16 ESCDcCapVoltOverVoltFlagB;              // 7位，ESC直流电容电压过压故障B
    Uint16 ESCDcCapVoltOverVoltFlagC;              // 8位，ESC直流电容电压过压故障C
    Uint16 ESCGridRMSOverVoltFlagA;                // 9位，ESC电网电压有效值过压故障A
    Uint16 ESCGridRMSOverVoltFlagB;                //10位，ESC电网电压有效值过压故障B
    Uint16 ESCGridRMSOverVoltFlagC;                //11位，ESC电网电压有效值过压故障C
    Uint16 ESCPowerFailDetectFlag;                 //12位，ESC掉电检测故障
    Uint16 ESCGridRMSUnderVoltFlagA;               //13位，ESC电网电压有效值欠压故障A
    Uint16 ESCGridRMSUnderVoltFlagB;               //14位，ESC电网电压有效值欠压故障B
    Uint16 ESCGridRMSUnderVoltFlagC;               //15位，ESC电网电压有效值欠压故障C
    Uint16 ESCOverLoadFlagA;                       // 0位，ESC输出过载故障A
    Uint16 ESCOverLoadFlagB;                       // 1位，ESC输出过载故障B
    Uint16 ESCOverLoadFlagC;                       // 2位，ESC输出过载故障C
    Uint16 ESCRmsOverCurrentFlagA;                 // 3位，ESC有效值输出过流故障A
    Uint16 ESCRmsOverCurrentFlagB;                 // 4位，ESC有效值输出过流故障B
    Uint16 ESCRmsOverCurrentFlagC;                 // 5位，ESC有效值输出过流故障C
    Uint16 ESCOverTemperatureFlagA;                // 6位，ESC散热片温度过温故障A
    Uint16 ESCOverTemperatureFlagB;                // 7位， ESC散热片温度过温故障B
    Uint16 ESCOverTemperatureFlagC;                // 8位，ESC散热片温度过温故障C
    Uint16 ESCFangLeiFaultFlag;                    // 9位，ESC防雷器故障
    Uint16 ESCBypassContNoCloseFlagA;              //10位，ESC旁路合闸故障A
    Uint16 ESCBypassContNoCloseFlagB;              //11位，ESC旁路合闸故障B
    Uint16 ESCBypassContNoCloseFlagC;              //12位，ESC旁路合闸故障C
    Uint16 ESCChassisOverTempFlag;                 //13位，ESC单元过温(机壳)
    Uint16 rsvd1;                                  //14位   备用
    Uint16 rsvd2;                                  //15位，备用
    Uint16 ESCCalibrarionFailureFlag;              // 0位，ESC零偏校准过大故障
    Uint16 ESCResonanceFlautFlag;                  // 1位，ESC谐振故障
    Uint16 ESCGridOverFreqenceFlag;                // 2位，ESC电网频率过频故障
    Uint16 ESCGridUnderFreqenceFlag;               // 3位，ESC电网频率欠频故障
    Uint16 ESCTieDianReadErrFlag;                  // 4位，ESC铁电故障
    Uint16 ESCAutoStartMuchFaultFlag;              // 5位，ESC自启动频繁故障
    Uint16 rsvd3;                                  // 6位，保留
    Uint16 rsvd4;                                  // 7位，保留
    Uint16 rsvd5;                                  // 8位，保留
    Uint16 rsvd6;                                  // 9位，保留
    Uint16 rsvd7;                                  // 10位，保留
    Uint16 rsvd8;                                  // 11位，保留
    Uint16 rsvd9;                                  // 12位，保留
    Uint16 rsvd10;                                  // 13位，保留
    Uint16 rsvd11;                                  // 14位，保留
    Uint16 rsvd12;                                  // 15位，保留
}Stru_RemoteMonitor_Structure;
extern Stru_RemoteMonitor_Structure RemoteStateMonitor;


typedef struct{
	int16 gridVoltABOffset;
	int16 gridVoltBCOffset;
	int16 gridVoltCAOffset;
	int16 apfOutputCurAOffset;
	int16 apfOutputCurBOffset;
	int16 apfOutputCurCOffset;
    int16 loadCurrentAOffset;
    int16 loadCurrentBOffset;
    int16 loadCurrentCOffset;
    int16 dcBusVoltUpOffset;
    int16 dcBusVoltDnOffset;
    int16 temperature1Offset;
    int16 temperature2Offset;
    int16 temDspBroadOffset;
    int16 GridVoltDevcoef;
    int16 DcBusVUpDevcoef;
    int16 DcBusVDnDevcoef;
    int16 ApfOutCuDevcoef;
    int16 CTOutCurDevcoef;
}Stru_Virtu_ZeroOffset;
extern Stru_Virtu_ZeroOffset VirtuZeroOffset;

typedef struct{
    int16 gridVoltHAOffset;              //界面显示零偏校准值
    int16 gridVoltHBOffset;
    int16 gridVoltHCOffset;
    int16 gridVoltLAOffset;
    int16 gridVoltLBOffset;
    int16 gridVoltLCOffset;
    int16 gridMainIAOffset;
    int16 gridMainIBOffset;
    int16 gridMainICOffset;
    int16 gridBypassIAOffset;
    int16 gridBypassIBOffset;
    int16 gridBypassICOffset;
    int16 aDCUDCA;
    int16 aDCUDCB;
    int16 aDCUDCC;

}Stru_Virtu_ZeroOffSETVAL;
extern Stru_Virtu_ZeroOffSETVAL VirtuZeroOffSETVAL;

typedef struct{
	Uint16 Theta;								//锁相角度
	Uint16 apfOutCurA_rms; 						//A相输出电流有效值
	Uint16 apfOutCurB_rms; 						//B相输出电流有效值
	Uint16 apfOutCurC_rms; 						//C相输出电流有效值
	Uint16 loadCurA_rms; 						//A相负载电流有效值
	Uint16 loadCurB_rms; 						//B相负载电流有效值
	Uint16 loadCurC_rms; 						//C相负载电流有效值
	Uint16 gridCurA_rms; 						//A相电网电流有效值
	Uint16 gridCurB_rms; 						//B相电网电流有效值
	Uint16 gridCurC_rms; 						//C相电网电流有效值
	Uint16 THDu_GridA; 							//电网电压畸变率A
	Uint16 THDu_GridB; 							//电网电压畸变率B
	Uint16 THDu_GridC; 							//电网电压畸变率C
	Uint16 THDi_LoadA; 							//负载电流畸变率A
	Uint16 THDi_LoadB; 							//负载电流畸变率B
	Uint16 THDi_LoadC; 							//负载电流畸变率C
	Uint16 THDi_GridA; 							//电网电流畸变率A
	Uint16 THDi_GridB; 							//电网电流畸变率B
	Uint16 THDi_GridC; 							//电网电流畸变率C
	Uint16 gpVoltA_rms; 						//电网A相电压有效值
	Uint16 gpVoltB_rms; 						//电网B相电压有效值
	Uint16 gpVoltC_rms; 						//电网C相电压有效值
	Uint16 rvsd1[8];      						//备用
}Stru_Multiple_Parallel_Value;
extern Stru_Multiple_Parallel_Value Multiple_Parallel_Value;

//故障录波控制用
struct Stru_FaultRecSel{
	Uint16 RecordMode;              //故障类型
	Uint16 RecordChan;              //故障通道
	Uint16 RecordChanOrg;
	int16  DelayRec;
	Uint16 RecordCntSet		:8;
	Uint16 RecordCnt		:8;
	Uint16 RefreshMode		:1;
	Uint16 isFFTMode		:1;
	Uint16 stopRecordMode	:2;
	Uint16 ForceRecordWaveRefresh :1;
};
extern struct Stru_FaultRecSel FaultRecSel;

typedef struct{
	Uint16  harmCompPerc2;
	Uint16  harmCompPerc3;
	Uint16  harmCompPerc4;
	Uint16  harmCompPerc5;
	Uint16  harmCompPerc6;
	Uint16  harmCompPerc7;
	Uint16  harmCompPerc8;
	Uint16  harmCompPerc9;
	Uint16  harmCompPerc10;
	Uint16  harmCompPerc11;
	Uint16  harmCompPerc12;
	Uint16  harmCompPerc13;
	Uint16  harmCompPerc14;
	Uint16  harmCompPerc15;
	Uint16  harmCompPerc16;
	Uint16  harmCompPerc17;
	Uint16  harmCompPerc18;
	Uint16  harmCompPerc19;
	Uint16  harmCompPerc20;
	Uint16  harmCompPerc21;
	Uint16  harmCompPerc22;
	Uint16  harmCompPerc23;
	Uint16  harmCompPerc24;
	Uint16  harmCompPerc25;
	Uint16  harmCompPerc26;
	Uint16  harmCompPerc27;
	Uint16  harmCompPerc28;
	Uint16  harmCompPerc29;
	Uint16  harmCompPerc30;
	Uint16  harmCompPerc31;
	Uint16  harmCompPerc32;
	Uint16  harmCompPerc33;
	Uint16  harmCompPerc34;
	Uint16  harmCompPerc35;
	Uint16  harmCompPerc36;
	Uint16  harmCompPerc37;
	Uint16  harmCompPerc38;
	Uint16  harmCompPerc39;
	Uint16  harmCompPerc40;
	Uint16  harmCompPerc41;
	Uint16  harmCompPerc42;
	Uint16  harmCompPerc43;
	Uint16  harmCompPerc44;
	Uint16  harmCompPerc45;
	Uint16  harmCompPerc46;
	Uint16  harmCompPerc47;
	Uint16  harmCompPerc48;
	Uint16  harmCompPerc49;
	Uint16  CRC;
}Stru_harmCompPercParameter;
extern Stru_harmCompPercParameter harmCompPercParameter;

typedef struct{
	Uint16  MagA;
	Uint16  MagB;
	Uint16  MagC;
	Uint16  PhaA;
	Uint16  PhaB;
	Uint16  PhaC;
	Uint16  limit;
	Uint16  start;
}Stru_Wave;

typedef struct{
	Stru_Wave 	Wave[CALIBRATION_DISP_LEN];
	Uint16  	CRC;
}Stru_Harmonic;
extern Stru_Harmonic Harmonic;

typedef struct{
	Uint16 ManualMode							    :1; 		//Bit0：        手动模式
    Uint16 AutomaticMode                            :1;         //Bit1：        校正模式
	Uint16 VoltageDistortionOverFlag 				:1;			//Bit2:		电压畸变率超限标志
	Uint16 PowerGridCurrDistortionOverFlag			:1;			//Bit3:		电网电流畸变率超限标志
	Uint16 LoadCurrDistortionOverFlag				:1;			//Bit4:		负载电流畸变率超限标志
	Uint16 PowerGridCurrSetOverFlag					:1;			//Bit5:		电网电流设定值超限标志
	Uint16 LoadCurrSetOverFlag						:1;			//Bit6:		负载电流设定值超限标志
	Uint16 AverageCurrent							:1;			//Bit7：	        均流模式
	Uint16 CalibrationFrequency						:4;			//Bit8~11：  	校正次数
	Uint16 CorrectionRoundNumber					:4;			//Bit12~15： 校正轮数
}Stru_HarmonicInfer;

typedef union{
	Uint16 all;
	Stru_HarmonicInfer B;
}Uni_HarmonicInfer;

typedef struct{
	Uint16 VoltagePhaseSequence		:1; 		//Bit0：		电压相序
	Uint16 PhaseSequenceCT			:3;			//Bit1~3：	CT相序
	Uint16 DirectionCT				:3;			//Bit4~6：	CT方向
	Uint16 DefectCT					:2;			//Bit7~8：  	CT缺失0:不缺失,1:C相缺失,2:B相缺失,3:A相缺失
	Uint16 rvsd						:7;			//Bit9：
}Stru_Infer;

typedef union{
	Uint16 all;
	Stru_Infer B;
}Uni_Infer;

struct Stru_FactorySet{
//原厂家设置
	int PhaseControl;               //ESC相位控制                       //Q2D_ratio;					//1 QD解耦系数
	int LoadCurlimit;               //ESC负载电流限制              //int D2Q_ratio;					//2 死区校正
	int SyncHardwareLead;			//3 同步超前点数
	int SampleLead;					//4 采样超前点数
	int T1PR;						//5 开关周期
	int reactPowGivenLimit;			//6 给定无功限幅 //未使用
	int Curr_MinCtrlFAN;			//7最小负载控制风机启动
	int Curr_MaxCtrlFAN;		    //8最大负载控制风机全速
	int Temp_MinCtrlFAN;			//9最小温度控制风机启动
	int Temp_MaxCtrlFAN;			//10最大温度控制风机全速
	int FanStartMinDUTY;			//11风机启动最小占空比
	int RateLoadCurrLimit;		    //12额定电流限制
	int CapacitySelection;		    //13容量选择
	int dcCapVoltRatio;				//14ESC直流电容电压采样
	int loadVoltRatio;				//15ESC负载电压采样
	int gridVoltRatio;				//16 电网电压采样
	int outputCurRatio;				//17 输出电流采样
	int loadCurRatio;				//18
	int AutoStartMode;				//19
	int SPWM;						//20 spwm标志位
	int RecordMode;					//21 测试录波标志位
	int isHarmCompensateMode;		//22 BW滤波标志位
	int bypassCurrRatio;		    //23 ESC旁路电流系数
	int DC_ERR_LIMIT;				//24 中点控制饱和
	int PIlim_Udc;					//25 直流电压饱和
	int PIlim_I;					//26 PWM内环电流饱和
	int phaseErrLimit;				//27 锁相误差限幅
	int kp_Pll;						//28  锁相比例
	int ki_Pll;						//29 锁相积分
	int kp_Dc_Div;					//30 中点控制比例
	int ki_Dc_Div;					//31 中点控制积分
	int kp_Dc;						//32 直流电压比例
	int ki_Dc;						//33 直流电压积分
	int kp_Cur;						//34 电流环比例
	int ki_Cur;						//35 电流环积分
	int GV_RMS_UNDER;				//36
	int GV_RMS_OVER;				//37
	int GF_OVER;					//38电网过频
	int GF_UNDER;					//39电网欠频
	int ESCDCVOLITLIMIT;			//40ESC直流电容电压限制值
	int VoltFallResponseLimit;		//41ESC电压跌落快速响应限值
	int OUTCUR_OVER_INS;			//42瞬时电流过流
	int OUTCUR_OVER_RMS;			//43电流有效值过流
	int OUT_OVER_LOAD;				//44输出电流过载
	int OutCurMaxLimit;				//45输出电流限幅
	int UNIT_OVER_TEMP;				//46散热器过温
	int reactPrCompPerc;            //47无功比例系数
	int harmCompPerc;               //48ESC测试固定占空比
	int negCurCompPerc;             //49ESC IGBT测试死区时间
	int ESCTESTDATA1;
	int ESCTESTDATA2;
	int ESCTESTDATA3;
	int ESCTESTDATA4;
	int TempEnvirProvalue;
	int THD_K;
	int THD_B;
	int VoltProport;			//电压补偿P
	int VoltIntegral;			//电压补偿I
	int VoltDerivative;			//ESC PI限幅值
	int VoltDQSource;
	int NegaQPowerCalib;
	int NegaDPowerCalib;
	int ReactivePowerCurrCalib;
	int ActivePowerCurrCalib;
	int TestMU_RATIO;
	int VoltLeadPoints;
	int windColdCtr;
	int PllCalibrPhase;
	int debugDispFlag;
	int R_SSwitchTime;          //ESC 运行转待机时间
	int S_RSwitchTime;          //ESC 待机转运行时间
	int WholeOutCurRmsLimit;
	int StandCosFiLimit;	//
	int StandUbanCurPer;	//
	Uint16 BOARD_OVER_TEMP;		//单元过温
	//场景设置
	Uint16 BG_MODEL;			//场景设置
//	Uint16 LED_MODEL;
	INITCSI INIT_SCIA_BAUD;		//蓝牙A波特率
	Uint16 CommunicationVersion;//communicationVersion
	INITCSI INIT_SCIB_BAUD;		//蓝牙B波特率
	INITCSI INIT_SCIC_BAUD;		//蓝牙C波特率
	Uni_Infer Infer;	//自动相序校正
	Uni_HarmonicInfer HarmonicInfer;	//自动谐波校正
	Uint16 VoltageDistortionValue;				//电压畸变率设定值
	Uint16 PowerGridCurrDistortionValue;		//电网侧电流畸变率设定值
	Uint16 LoadCurrDistortionValue;				//负载侧电流畸变率设定值
	Uint16 PowerGridCurrSetPoint;				//电网侧电流设定值
	Uint16 LoadCurrSetPoint;					//负载侧电流设定值
	Uint16 CompensatingHarmonicQuantity;
//新增设置
	Uint16 StandbyRecoveryTime;
	Uint16 AutoCurr;							//回差电流
	Uint16 StandbyMode;
	Uint16 VolSurTime;
	Uint16 ModeSwitchingCurrent;
    Uint16 FaultRecord;
    Uint16 rvsd;
	Uint16 CRC;
};
extern struct Stru_FactorySet FactorySet;
extern struct Stru_FactorySet FactorySetupRatio;
extern struct Stru_FactorySet ParamEnviron;

struct VARIZERO{
    int16 gridVoltAB;
    int16 gridVoltBC;
    int16 gridVoltCA;
    int16 apfOutputCurA;
    int16 apfOutputCurB;
    int16 apfOutputCurC;
    int16 loadCurrentA;
    int16 loadCurrentB;
    int16 loadCurrentC;
    int16 dcBusVoltUp;
    int16 dcBusVoltDn;
    int16 temperature1;
    int16 temperature2;
    int16 temDspBroad;
    int16 GridVoltDevcoef;
    int16 DcBusVUpDevcoef;
    int16 DcBusVDnDevcoef;
    int16 ApfOutCuDevcoef;
    int16 CTOutCurDevcoef;
    Uint16 CRC;
};
extern struct VARIZERO VariZeroOffset;
extern struct VARIZERO VariZeroOffsetRatio;

struct VARIZEROVAL{         //界面下发的零偏值
    int16 gridHVoltA;
    int16 gridHVoltB;
    int16 gridHVoltC;
    int16 gridLVoltA;
    int16 gridLVoltB;
    int16 gridLVoltC;
    int16 gridMainCurA;
    int16 gridMainCurB;
    int16 gridMainCurC;
    int16 gridBypassCurA;
    int16 gridBypassCurB;
    int16 gridBypassCurC;
    Uint16 CRC;
};
extern struct VARIZEROVAL VariZeroOffsetVAL;
extern struct VARIZEROVAL VariZeroOffsetRatioVAL;


typedef struct{
	Uint16 apfWiringmethod		:1; 		//Bit0：接线方式,0:三相四线制 1:三相三线制
	Uint16 startingMethod		:1;			//Bit1：启动方式,0:自动模式 1:手动模式
	Uint16 positionCT			:1;			//Bit2：CT位置,0:电网侧 1:负载侧
	Uint16 harmCompEn			:1;			//Bit3：谐波补偿模式,0:关闭 1:打开
	Uint16 negCurCompFlag		:1;			//Bit4：不平衡补偿模式,0:关闭 1:打开
	Uint16 AVCMode				:1;			//Bit5：AVC模式,0:关闭 1:打开
	Uint16 constantQFlag		:2;			//Bit6~7：无功补偿模式,	0:恒无功功率  1:恒无功电流
											//		 				2:恒功率因数  3:关闭无功补偿
	Uint16 VoltageMode			:3;			//Bit8~10：ESC电压补偿模式,	0:升压  1:降压 2:升降压补偿模式 3:不平衡补偿模式 4:无功补偿模式
	Uint16 prioritizedModeFlag	:2;			//Bit11~12：优先设定,0,1:无功优先2:谐波优先3:不平衡优先
											//					4:快速补偿5:自动补偿6:电压优先
	Uint16 StandbyModeFlag		:1;			//Bit13：待机模式开启标志
	Uint16 TransformerSection	:2;			//Bit14~15：变压器连接组别
}Stru_WordMode;
typedef union{
	Uint16 all;
	Stru_WordMode B;
}Uni_WordMode;

//extern Uni_WordMode WordMode;

struct Stru_UserSetting{		//eeprom start:0x00.length
	Uni_WordMode WordMode;
	int TargetCurrentUnbalance;		//不平衡度目标值
	int transfRatio;			//CT变比
	int GV_RMS_UNDER;			 //电网有效值欠压
	int GV_RMS_OVER;			 //电网有效值过压
    int Backdiffvalue;           //回差值
	int ConstantReactivePower;  //恒无功功率指令
	int TransformRatioParall;
	int VolttargetA;             //A相电压目标值
	int VolttargetB;             //B相电压目标值
	int VolttargetC;             //C相电压目标值
	int VoltThreSholdA;          //A相电压门限值
	int VoltThreSholdB;          //B相电压门限值
	int VoltThreSholdC;          //C相电压门限值
	int AutoWaitCurr;
	int Mode_PP;
	Uint16 CRC;
};
extern struct Stru_UserSetting UserSetting;
extern struct Stru_UserSetting UserParamRatio;

struct Stru_APFID{		//eeprom start:0x8D0.length
	Uint16 RandIDH;				//单元匹配地址高
	Uint16 RandIDL;             //单元匹配地址低
	Uint16 APF_ID;				//单元地址
	Uint16 MU_UNIT_NUM;			//单元总数
	Uint16 CRC;
};
extern struct Stru_APFID MuFaciID;

struct Fan_temp{
    int Temp_A1;
    int Temp_A2;
    int Temp_A3;
    int Temp_B1;
    int Temp_B2;
    int Temp_B3;
    int Temp_C1;
    int Temp_C2;
    int Temp_C3;
    int Temp_IN;
};
extern struct Fan_temp FanTempValue;



// 控制函数声明区
inline float Max(float a,float b,float c)
{
	float max = a > b ? a:b;
	return (max > c ? max:c);
}

inline float Min(float a,float b,float c)
{
	float min = a < b ? a:b;
	return (min < c ? min:c);
}
extern void Delayus(int16 i);
extern void Timer111usIntTask(void);
//void MainLoop(void);
extern void  CapacitorSwithingCtr(void);
extern void InitParameters(void);
extern void FunAutoControl(void);
extern void InitBeforeReadROM(void);
extern void InitAfterReadROM(void);
extern void AdRegeditOffset(void);
extern void ZeroOffsetJudgment(void);
//extern void PLL(void);
extern void DischargingMode(void);
extern void Pi_Adjust_Sync(void);
extern void Receive_Signe(void);
extern void Sync_OUT(char OUT);
extern void Signe_Head(char Should, Uint16 Count);
extern void Signe_Switch(void);
//extern void Conversion4bTo5b(Uint16 Number);
extern void Conversion8bTo5b(Uint16 Number);
extern void Conversion16bTo5b(Uint16 Number);
extern void Get_Order(void);
extern void Send_Order(Uint16 addr,Uint16 Number);
extern void SigPhDQCal(void);
extern void SigPhDQinput(void);
extern void ReactivePowerGiven(void);
//extern void DisablePWM(void);
//extern void EnablePWM(void);
//extern void FaultClearPWM(void);
extern void FaultDetectInMainLoop(void);
//extern void FaultDetectInRunning(int isRunState);
extern void FaultDetectInInt(void);
extern void FaultFastDetectInInt(void);
extern void FaultDetInRunning(void);
extern void FaultReset(void);
extern void GridPhaseSequTest(Uint16 i);
int Monthnum(unsigned char y,unsigned char m);
//extern void TimerCounter(void);
extern void InitSci(struct STRU_SCI *pStu,Uint16 baud);
extern void InitSciGpio(void);
extern void UartAllRun(void);
extern void THDiCal(void);
extern void OutsideIsrProg(void);
extern void FaultRecordProg(void);
extern void AutoStartInFault(void);
extern void ExecuteOpenContact_A(void);
extern void ExecuteOpenContact_B(void);
extern void ExecuteOpenContact_C(void);
extern void OverTempLimitCur(void);
extern void InitGpio_NewBoard(void);
extern void OutputGpioInit_NEWBOARD(void);
extern void SetResetExecute(void);
extern void initStartVar(void);
extern int16 FaultDetectLong(Uint16 SoeName,Uint16 counterName,Uint32 FaultDelayTime);
extern int16 StandbyDetectLong(Uint16 SoeName,Uint16 counterName,Uint32 StandbyDelayTime);
extern int16 StandbyDetect(Uint16 SoeName,Uint16 counterName,Uint32 StandbyDelayTime);
extern int16 FaultDetect(Uint16 SoeName,Uint16 counterName,Uint16 FaultDelayTime,Uint16 ESCPHASE);
extern void FirstFaultStop(Uint16 ESCPHASE);
extern int16 THREEPHASEFaultDetect(Uint16 SoeName,Uint16 counterName,Uint16 FaultDelayTime);
extern int16 PowerFailFaultDetect(Uint16 SoeName,Uint16 counterName,Uint16 FaultDelayTime);
extern int16 FaultDetectExtCnt(Uint16 SoeName,Uint16 extCnt,Uint16 FaultDelayTime);
extern void SetFaultDelayCounter(Uint16 counterName,Uint16 mode);
extern void FaultStopExecute(void);
extern void Monitor_Message(void);
extern void RemoteParamerRefresh(void);
extern void RemoteParamerRefresh2(void);
extern int16 UserParamReading(void);
extern int16 FactoryParamReading(void);
extern int16 CapParamReading(void);
extern int16 FaultParamReading(void);
extern int16 UserHarmnoicReading(void);
extern int16 HarmnoicParamReading(void);
extern int16 CommunicationParamReading(void);
extern int16 ManufacturerParametersReading(void);
extern int16 UserPreferencesReading(void);
extern int16 VirtuPreferencesReading(void);
extern int16 FlashPreferencesReading(void);
extern int16 IDPreferencesReading(void);
extern void ParamEnvironRefresh(void);
extern void ManufacturerParametersRefresh(void);
extern void VirtuPreferencesRefresh(void);
extern void IDParametersRefresh(void);
extern void CapParamRefresh(void);
extern void VariZeroOffsetRefresh(void);
extern void UserPreferencesRefresh(void);
extern void BlindingBluetoothConnectionLED(void);
extern void SlaveStateInstruction(void);
extern void SetWaveRecordMode(Uint16 mode);
extern void HarmnoicCorrection(void);
extern void InitCapGroupAct(void);
extern void PowerReactStateRefresh(void);
extern void FlashApiDataCopy(Uint16 Samprate);
extern void FlashDataRead(void);
extern void WriteFlashConfig(Uint16 FaultNum);
extern int  EraseSector();
extern void StartEraseSector(void);
//extern int  EraseSector(int block);
extern void SoeRemote(Uint16 faultNumber);
extern void RemoteWriteControl(Uint16 usAddress);
extern void SPITXINTB_ISR(void);
extern void PLLrun(void);
extern void SigPhDQComput(void);
extern void InitAdc(void);
extern void InitGpio_NewBoard(void);
extern void InitUserVar(void);
extern void CapacitorSwithingCtr(void);
extern void CapacitorSwithingExc(void);
extern void Funda3sTo2rTransf(void);
extern void FirstRmsCalc(void);
extern void SlowRmsCalc(void);
extern void RmsCalcIn(void);
extern void RMSLimit(void);
extern void VoltageCompensation(void);
extern void ReactivePowerGiven(void);
extern float CurrLimitPri(float in);
extern float CurrLimitZPri(float in);
extern void MainContactConfirm(Uint16 VAL);
extern void Fan_Cnt(void);
//extern void MeanAllInit(void);
extern void RMSAllInit(void);
extern float32 MeanInit(Uint16 chan);
extern void InitParameters(void);
extern void FaultDetectInInt(void);
//extern void FaultDetectInRunning(void);
extern void FaultDetectInMainLoop(void);
extern void FaultReset(void);
//extern void GridPhaseSequTest(void);
extern void FaultRecordProg(void);
extern void OutputGpioInit_NEWBOARD(void);
extern void SetStartCtrl(void);
extern void CapcitorCommAct(void);
extern void FunAutoControl(void);
extern void OutdoorMEC_EnergySaving_Ctr(void);
extern void DischargingMode(void);
extern void Delayus(int16 i);
extern void OverTempLimitCur(void);
extern void AdRegeditOffset(void);
extern void ZeroOffsetJudgment(void);
extern void THDiCal(void);
extern void AutoStartInFault(void);
extern void Pi_Adjust_Sync(void);
extern void InitScia(int16 baud);
extern void InitScib(void);
extern void InitScic(void);
extern void SciA_Run(void);
extern void SciB_Run(void);
extern void SciC_Run(void);
extern void Sci_Run(void);
//extern void SciDeal_Run(void);
extern void ReadSpiPara(void);
extern void InitSciGpio(void);
extern Uint16 SpiWrite(Uint16 data);
extern void   InitSpiGpio(void);
extern void   FM_Write_Enable(void);
extern void   FM_Write_Disable(void);
extern Uint16 FM_Read_Status();
extern void   FM_Write_Status(Uint16 data);
extern Uint16 FM_Read_Byte(Uint16 address);
extern void   FM_Write_Byte(Uint16 address,Uint16 data);
extern void   FM_DatWrite(int addr, int length, int *ptr);
extern void   FM_DatRead(int addr, int length, int *ptr);
extern void   FM_DatWordWrite(int addr,int data);
extern void InitEPwm(void);
extern void InitEPwmGpio(void);
extern void Epwm1t2();
extern void InitCtrlParam(int mode);
extern void AdjustFulkPRcoeff(void);
extern void InitMainCtrlParam(void);
extern void PLL(void);
//extern void DisablePWM(void);
extern void DisablePWMA(void);
extern void DisablePWMB(void);
extern void DisablePWMC(void);
extern void FaultClearPWMA(void);
extern void FaultClearPWMB(void);
extern void FaultClearPWMC(void);
//extern void EnablePWM(void);
extern void EnablePWMA(void);
extern void EnablePWMB(void);
extern void EnablePWMC(void);
extern void PWMsector(void);
//extern void profileDisp(void);
//extern void InitXIntrupt(void);
extern void FFTInit(void);
//extern void FFTDataReduction(Uint16 Chan);
extern void TestWaveGenerator(void);
extern float TempCompFuntion(float TempData[], int VAL);
extern float CURRCompFuntion(float CURRData[], int VAL);
extern float DUTYMUXFuntion(float CTLFANDATA[], int VAL);
extern float UNCurrMAXCompFuntion(float UnCurrData[], int VAL);
extern float UNCurrMINCompFuntion(float UnCurrData[], int VAL);
extern float UNCurDiffMAXFuntion(float UNCurDiffData[], int VAL);

extern int Tempera_cal( int volt);
extern void FANAutoCtlFunction(void);

//extern int16 ZCPJudge(float VoltIn,float VoltOut,float Esc_VoltPhase,float CurrIn,Uint16 TBCTR);

extern void StateFeedBackJudge(void);

extern void FORCEPWM(void);
extern void ESCfilterMemCopy(Uint16 num);

//DSP2837x_Cla.c 包含的函数
extern void CLA_configClaMemory(void);
extern void CLA_initCpu1Cla1(void);
#endif /* VARIBLESREF_H_ */
