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

/*΢�Ͷ�·����WY*/

#define EEPROM_FACTORY_PARAMETER		0x0		//0x00~0x1FF
#define EEPROM_USER_PREFERENCES			0x200	//0x200~0x2CF
#define EEPROM_RECORD_FLASH				0x2D0	//0x2D0~0x2FF
#define EEPROM_VIRTU_ZEROOFFSET			0x300	//0x300~0x3FF
#define EEPROM_CAPPARAM					0x400	//0x100~0x14F	0x50	��������	�û��Զ����·�(0x67,9000~9037)
#define EEPROM_HARMNOIC_USER			0x500	//
#define EEPROM_HARMNOIC_FACTORY			0x564	//
#define	EEPROM_FACILITYID				0x8D0   //0x8D0~0x8DF
#define EEPROM_RECORD_INDEX				0x8E0	//0x8E0~0x907	0x28	SOE����	SOE��ȡ(0x6A) 40
#define EEPROM_RECORD					0x908	//0x908~0x1FFF	0x16F8	SOE���ݿ�	SOE��ȡ(0x6A) 42*2*��������(70)

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

/*�¼��ڲ�����־�е�ƫ��������WY*/
#define SOE_GP_EVENT 					300

#define SOE_BUFF_GROUP					10			//�ɼ�¼10������ͬʱ����
#define RECORD_NUMBER					30			//�ɼ�¼�¼�����

#define EEPROMLOCK  			1
#define EEPROMUNLOCK 			0
#define DEBUG_WAVE_CHAN_NUM  	16
#define DEBUG_WAVE_GROUP 		(12)
#define DEBUG_WAVE_LEN 			500

#define FIXED_CAP_EN			0				//������������Ͷ��
#define FIXED_IOCAP_NUM 		4				//IO������ʽ���޹�Ͷ�е��ݵ�����.
#define FIXED_CAP_NUM 			4				//�����������޹�Ͷ�е��ݵ�����.
#define COMM_FIXED_CAP 			1				//�����������޹�Ͷ�е��ݵķ�ʽΪͨ��Ͷ��

#define FIRST_RMSNUM            12               //���ټ���RMS����,��������ռ���һ���,�ȷſ��ٵ�,�����������ٵ�
#define SLOW_RMSNUM             14              //���ټ����RMS����
#define HARM_CALU_NUM 			9				//г����ʾ������
#define ALL_RMSNUM  			    (FIRST_RMSNUM+SLOW_RMSNUM+2*HARM_CALU_NUM)              //��RMS����
#define RMS_CALU_SEGMENT            (4)                                                     //4��
#define RMS_CALU_SEGMENT_NUM        (RMS_CALU_SEGMENT+2)                                    //���ÿ��5ms��һ��RMSֵ.���г���20ms/5ms=4��,+2���ڻ���
#define RMS_CALU_BLOCK_NUM          ((CTRLFREQUENCY/POWERGRID_FREQ)/RMS_CALU_SEGMENT)       //ÿ�εĵ�����6400/50/4��=32��,ÿ32����ϳ�һ��ֵ

#define CALIBRATION_DISP_LEN        48

/*��������������WY*/
#define CNT_FAULT_DETECT_NUM            43  //Ҫ���ڴ�CNT_HW_OVER_CUR_A��ʼ������

#define CNT_HW_OVER_CUR_A               0   //
#define CNT_HW_OVER_CUR_B               1   //
#define CNT_HW_OVER_CUR_C               2   //

/*�Ѽ�⵽��A��˲ʱֵ����������WY*/
#define CNT_INS_OVER_CUR_A              3

/*�Ѽ�⵽��B��˲ʱֵ����������WY*/
#define CNT_INS_OVER_CUR_B              4

/*�Ѽ�⵽��C��˲ʱֵ����������WY*/
#define CNT_INS_OVER_CUR_C              5

/*�Ѽ�⵽��A��ֱ�����ݵ�ѹ��ѹ�źŵĴ�����WY*/
#define CNT_DC_NEUTRAL_OVER_VOLT_A      6

/*�Ѽ�⵽��B��ֱ�����ݵ�ѹ��ѹ�źŵĴ�����WY*/
#define CNT_DC_NEUTRAL_OVER_VOLT_B      7

/*�Ѽ�⵽��C��ֱ�����ݵ�ѹ��ѹ�źŵĴ�����WY*/
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

/*�Ѽ�⵽��A����·�ű��̵����պϹ��ϴ�����WY*/
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

#define LCNT_FAULT_DETECT_NUM			1	//Ҫ���ڴ�LCNT_LOAD_OVER_CURR��ʼ������
#define LCNT_LOAD_OVER_CURR 			0	//
////����Ͷ�в���
//#define CAPACITOR_CLOSE_TIME 		6
//#define CAPACITOR_OPEN_TIME 		12
#define zeroOffsetUpLimit 			2200.0f	// ��ƫУ׼���޶���Χ
#define zeroOffsetDownLimit 		2000.0f

//����ģʽ
#define BGMODLE_3P_APF 				0		//MECģʽ,��г��,�޹�,��ƽ��,������ѹ��ģʽ
#define BGMODLE_3P_SVG 				1		//SVGģʽ
#define BGMODLE_3P_VOLT_REGULATOR 	2		//�����ѹ������
#define BGMODLE_3P_TRASFOROM_APF 	3		//MECģʽ,��г��,�޹�,��ƽ��,����ѹ��ģʽ
#define BGMODLE_3P_MEC 				4		//MECģʽ,��г��,�޹�,��ƽ��,��ѹ(�ñջ���ʽ)
#define BGMODLE_1P_VOLT_REGULATOR 	5		//�����ѹ��ģʽ
#define BGMODLE_3P_CURR_SOURCE 		6		//���ű۵���Դģʽ

//THDi��ʾ
//#define DISPLAY_MEC_FLAG_MEC		1		//��ʾ�޹�
//#define DISPLAY_MEC_FLAG_THDI		0		//��ʾTHDi

#define CAP_LOAD_RATIO				(51*1e-6*314)
#define CAP_VOL_POINT				((PWMFREQUENCY/(50*4))*2*3)	//ÿ�ܲ���3/4
#define CAP_VOL_POINT_CUR			((PWMFREQUENCY/(50*4))*2*1)	//ÿ�ܲ���1/4
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
// ��Чֵ�ļ������
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
//������ʹ�ã�����ʱ�������
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
extern Uint32 CntForPowerON;				//�ϵ��һֱ����,����������ʱ�����

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
extern float tmpOffsetValue[18];//��ƫ����
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
// DQ������
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

// ���DQ
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
// ��������
extern float GridResSin ;
extern float GridResCos ;
//extern float PLLResSin ;
//extern float PLLResCos ;
//extern float PLLRes2Sin ;
//extern float PLLRes2Cos ;

// ��������
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
extern float SampleLead;  // 8.6 ������ϵ�� 0816

//��������
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
extern Uint16 CapFlagDown;				//��������Ͷ��    ��־λ
extern Uint16 SwitchTimeOn;          	//Ͷ��ʱ
extern Uint16 SwitchTimeOff;          	//�м�ʱ
extern Uint16 TimSwitchOnStart;			//Ͷ��ʱ��ʱ��־λ
extern Uint16 TimSwitchOffStart;		//����ʱ��ʱ��־λ
extern Uint16 AutoSwitchOnCount;		//�Զ�Ͷ����ͳ��
extern Uint16 AutoSwitchOffCount;		//�Զ�������ͳ��
extern Uint16 AutoSwitchOn;				//�Զ�Ͷ��־
extern Uint16 AutoSwitchOff;			//�Զ��б�־
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
extern float MU_LCD_RATIO,MU_MultRatio;			//�豸���в���
extern int MU_ALLCOUNT,PWM_address,APF_ID;
extern Uint16 CapEmergTim;
extern float storageUsed;
extern float GridNeutralCurRms;
extern float harmCompPerc;					//�����˹�Ԥ��г��ϵ��
extern int VolUpCo;							//��ѹ����
extern int VolDownCo;						//��ѹ����
extern int VolUpSurCo;						//��ѹ����Խ�޲���Ŀ��ֵ
extern int VolDownSurCo;					//��ѹ����Խ�޲���Ŀ��ֵ
extern int VolSurTime;						//��ѹԽ�޳���ʱ��

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
extern float VolCor;							//��ѹĿ��ֵ
extern float VolCorU,VolCorV,VolCorW;
extern int RechageskipstopFlag;
extern float ModeSwitchingCurrent;					//ģʽת��������ֵ
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
extern Uint16 CRC(Uint16 *puchMsg, Uint16  usDataLen);		//CRCУ�麯������

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
extern float OutCurMaxLimit;				// ����������޷�
extern float OutCurMaxLimitNeg;
extern float TransformRatioVal;			//��ѹ���ѹ�����ֵ,��TrasformRatioParall����ó�
extern int forwardPoints ;
extern int cntForCurMax;
extern float outCurTempLimit;
extern float outCurSinkTempLimit;
extern float outCurBoardTempLimit;
extern float capTsctr;				//������ֵ
extern int cntForSyncInit;			//ͬ����ʼ��
extern Uint16 cntForSyncErr;
extern int syncState;				//ͬ��״̬��
extern float pwmTsGiven;			//����PI�ĸ���
extern float POINT_CAP1;			//�źŴ��
extern float MAX_CAP1;
extern float MIN_CAP1;
extern float syncErr;				//����PI������
extern float syncErrPrior;
extern Uint16 pwmTBPRD;
extern Uint16 PrvPwmTBPRD;
extern Uint16 RemoteAddress;
extern Uint16 DropLetMarkFlag;
extern float PWM_PERIOD_MAX;
extern float PWM_PERIOD_MIN;
extern float syncErrLimit;			//����ɹ���ʶ
extern int cntForSyncOut;			// ���ͷ�ת��ƽ
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
extern int OverVoltHarm;// ��г�����з�ֵ����λ��У׼
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
//�����жϲ���
extern int SpuriousIn,SpuriousOut;						//����û���õ���IO�ڵļ��������
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
//�����жϲ���
extern int SpuriousIn,SpuriousOut;						//����û���õ���IO�ڵļ��������
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
	Uint16 compenEn 				:1;	//Bit0:�����޹�����              			1:�ر� 0:��
	Uint16 mode						:1;	//Bit1:��������						1:һֱ�����̶����������Զ�����,
										//                                  0:MECװ��������,��ʼ�̶����������Զ�����  2�ֶ�����ģʽ
	Uint16 ctrl						:1;	//Bit2:���ƶ˿�              				1:IO�ڷ�ʽ 0:ͨ�ŷ�ʽ
	Uint16 unblance					:1;	//Bit3:�������ಹ��Ͷ��      			1:�ر� 0:��
	Uint16 handMode					:1;	//Bit4:�ֶ�ģʽ						0:�ر� 1:��
	Uint16 rsvd						:1;	//Bit12~5:����
	Uint16 groupFaltEn				:1;	//Bit13:�����������              			1:�رչ���0:��
	Uint16 commFaltEn				:1;	//Bit14:��������ͨ��ʧ��(����������) 	1:�رչ���0:��
	Uint16 overVoltHarmFaltEn		:1;	//Bit15:��ѹг�����ޱ���           		1:�رչ���0:��
	Uint16 overVoltFaltEn			:1;	//Bit15:��Ƿ��ѹ����           			1:�رչ���0:��
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
	Uint16 CapVarUnseal				:1;	//Bit0:�����޹�����          1:�ر� 0:��
	Uint16 CapVarConditon			:1;	//Bit1:�������� 1:һֱ�����̶����������Զ�����
										//0:MECװ��������,��ʼ�̶����������Զ�����
	Uint16 CommutionChose			:1;	//Bit2:���ƶ˿�              1:IO�ڷ�ʽ 0:ͨ�ŷ�ʽ
	Uint16 DivedPhCapvar			:1; //Bit3:�������ಹ��Ͷ��      1:�ر� 0:��
	Uint16 CapHandSwitch			:1;	//Bit4:�����ֶ�����Ͷ��      1:�ر� 0:��
	Uint16 ProSlect					:1; //Bit5:����ѡ��;0:SVG���� 1:����������
	Uint16 CapOvertempFault			:1;	//Bit6:����������¹���           1:�رչ���0:��
	Uint16 CapFault					:1;	//Bit7:�����������               1�رչ���0:��
	Uint16 CapCommutionFault		:1;	//Bit8:��������ͨ��ʧ��(����������) 1:�رչ���0:��
	Uint16 VolHarmonicLimit			:1;	//Bit9:��ѹг�����ޱ���           1:�رչ���0:��
	Uint16 SlaveIdEnable			:1;	//Bit10:�ӻ�ͨ�ŵ�ַƥ��ʹ��      1:�ر�ƥ�� 0:��
	Uint16 CapCompensation			:3;	//Bit11-13: 0:�ֶ� 1:˳��
	Uint16 rsvd						:2;	//����
} CaGrSwi;
typedef union{
	Uint16 	all;
	CaGrSwi  B;
}Capswitchgroup;
struct CapSwitch{
	Uint16 PhactGroup[24];			//24�����Ͷ��	�·�
	Uint16 Capacity[24];			//24���������	�·�
	Uint16 rsvd1;
	Uint16 SwitchThreshold;			//Ͷ������		�·�
	Uint16 TimSwitchOn;				//��Ͷ��ʱ s		�·�
	Uint16 TimSwitchOff;			//Ͷ����ʱ s		�·�
	Capswitchgroup capgroupswitch;	//���������л�	�·�
	Uint16 CapDivdNum;
	Uint16 SvgPercent;				//���ಹ��λ��/��������
	Uint16 HmiVolProt;
    Uint16 CapComuFaultFlag;        //ͨ�Ź��ϱ�־Ϊ
	Uint16 rsvd2;//9
	Uint16 rsvd3;//A
	Uint16 rsvd4;//B
	Uint16 rsvd5;//C
	Uint16 rsvd6;//D
	Uint16 MasteRversion;			//���汾���·�	�·�
	Uint16 SlaveRversion;			//�Ӱ汾���·�	�·�
	Uint16 CapStateUp[24];			//24�����״̬  	��ȡ
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
	Uint16  FlashApiEnable				:1; //����¼���洢����λ 1:����flash 0:���洢
	Uint16  FlashWriteReady				:1; //Flashд����׼��������־λ 1:����ִ��д���� 0:����ִ��
	Uint16  AutoRecordReadFlag			:1; //����¼���Զ���ȡ��־λ
	Uint16	RemoteControlEn				:1;	//Զ�̿���ʹ��λ
	Uint16  FlashBlock0                 :1; //Block0 ʹ�ñ�־λ
	Uint16  FlashBlock1                 :1; //Block1 ʹ�ñ�־λ

} FLSHSTATE;
typedef union{
	Uint16 	  all;
	FLSHSTATE  B;
}FlshSte;
struct STRU_FAURCD{
	FlshSte	FlashState;
	Uint16	UiBlockRead;			 //�����ֶ���ȡ��λ��
	int16   FlashBlockRead;		 	 //����¼�����ȡλ��
	Uint16  FlashBlockWrite;		 //����¼����洢λ��
	Uint16  RecordBlockNum;			 //����¼����洢����
	Uint16  WaveRecordHandler;       //����¼��������
	Uint16	CRC;
};
extern struct STRU_FAURCD RecordFlash;

struct STRU_CNT_MS{
	int inputStart;
	int inputStop; //ͣ����ť���������µ�ʱ������λ��ms����WY
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
	int VolSurDelay;			//Խ�޳���ʱ��
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
	Uint16 onceTimeAdcAutoAdjust :1; //ADC��ƫУ׼��־λ��0��ʹ�ܣ�1��ʧ�ܡ�WY
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
	Uint16 negCurCompFlag :1;//���򲹳�����
	Uint16 harmCompEn :1;//г����������
	Uint16 reactPrCompFlag :1;//�޹���������
	Uint16 AVCPrCompFlag :1;//AVC��������
	Uint16 VoltcompenFlag :1;//��ѹ��������
	Uint16 VoltageModeFlag :1; //����ѹģʽѡ��0����ѹ��1����ѹ��WY
	Uint16 apfWiringmethod :1;
	Uint16 startingMethod :1;
	Uint16 isHarmCompensateMode :1;//0��г��ģʽ 1����
	Uint16 ManualUbanCurFlag :1;//���������ֶ�����ƽ��ģʽ
	Uint16 positionCT :1;
	Uint16 spwmFlag :1;
	Uint16 DynamicProportionEn :1;
	Uint16 VoltFilterEn :1;
	Uint16 SVPWMEn :1;
	Uint16 InvCurUVRmsReadyFlag :1;
	Uint16 prioritizedModeFlag :2;//Bit10~12�������趨,0,1:�޹�����2:г������3:��ƽ������
	Uint16 modeChoosedFlag :4;// ģʽѡ��
	Uint16 constantQFlag :2;//Bit6~7���޹�����ģʽ,	0:���޹�����  1:���޹�����
//		 				2:�㹦������  3:�ر��޹�����

	/*
	 * ��ѹģʽ��WY
	 * 0����ѹ��
	 * 1����ѹ��
	 * 2������ѹ����������
	 * 3����ƽ�����������
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
//	Uint16 ForceRecordWaveRefresh	:1;		//���沨����ʾˢ��
	Uint16 ForceRMSRefresh :1;//�������������Чֵˢ��
	Uint16 FactoryMode :4;//���Էŵ�
	Uint16 RechargeUdcFullFlag :1;//ֱ�����ݳ���ѳ履
	Uint16 EnTransformRatioMode :1;//��ѹ���ѹ��ʹ��
	Uint16 TransformRatioMode :3;//��ѹ���ѹ�����ģʽ
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

// ����
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
    int16 *GridHVoltA; //A���ѹ���ѹ��WY
    int16 *GridHVoltB; //B���ѹ���ѹ��WY
    int16 *GridHVoltC; //C���ѹ���ѹ��WY

    int16 *GridLVoltA; //A���ѹ���ѹ��WY
    int16 *GridLVoltB; //B���ѹ���ѹ��WY
    int16 *GridLVoltC; //C���ѹ���ѹ��WY

    int16 *GridMainCurA; //A���ѹ�������WY
    int16 *GridMainCurB; //B���ѹ�������WY
    int16 *GridMainCurC; //C���ѹ�������WY

    int16 *GridBypassCurA; //A���ѹ�������WY
    int16 *GridBypassCurB; //B���ѹ�������WY
    int16 *GridBypassCurC; //C���ѹ�������WY

    int16 *ADCUDCA; //A��ֱ�����ݵ�ѹ��WY
    int16 *ADCUDCB; //B��ֱ�����ݵ�ѹ��WY
    int16 *ADCUDCC; //C��ֱ�����ݵ�ѹ��WY
}VirtulADStruVAL;
extern VirtulADStruVAL VirtulADVAL;

typedef struct{                  //������ʹ�õ���ƫֵ,�ͽ����·�����ƫֵ��һ������
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
    int ContactorDelay; //���ڼ����·�ű��̵ּ���״̬����ʱ����λ��ms����Ŀ�ģ������ڶ�μ�⣬��ֹ��⡣WY
    int PllDelay; //�����໷��������ʱ������λ��ms����WY
    int HarmDisDelay;      //ESC����г��������ʱ
    int FaultDelayFlag;
    int StartDelay;
    int SelfJc;
    int CutCurrDelay;
};
extern struct ESC_STRU_CNT_MS ESCCntMs;

struct ESC_STRU_CNT_SEC{
    int ChargingTime; //Ԥ���ʱ������λ��s����WY
    int FunContDelay;
    int R_SSwiTiDelay; //ESC�豸�ӡ����С�״̬ת�롾������״̬�ı�Ҫʱ������λ��s����WY
    int S_RSwiTiDelay; //������ѹǷѹʱ������λ��s����WY
    int RelayDISDelay;   //ESCɽ��³���ֳ�������Ӽ̵�������
    int PRECHARGEDelayBY; //Ԥ���ʱ�����ڶ���·�ű��̵ּ���״̬������ʱ��WY //��ֹ�ϵ������乤���дű���������ը��
    int PRECHARGEDelay; //Ԥ���ʱ�����ڶ���·�ű��̵ּ���״̬������ʱ��WY //��ֹ�ϵ������乤���дű���������ը��
    int HWPowerSupDelay; //�ر�15V��Դ����Ҫ��ʱ������λ��s����WY
    int HWPowerStopDelay;
    int HWPowerFaultDelay;
    int UNCurrDelay1;
    int UNCurrDelay2;
};
extern struct ESC_STRU_CNT_MS ESCCntSec;

/*
 * X����Ʋ�����WY
 * X��ȡֵ����Ϊ{A, B, C}
 * */
typedef struct
{
	struct ESC_STRU_CNT_SEC ESCCntSec;
	struct ESC_STRU_CNT_MS ESCCntMs;
	Uint16 onceTimeStateMachine :1; //����״̬��⡣0���������ɣ�1���ȴ�����С�WY
	Uint16 ByPassContactFlag :1; //��·�ű��̵ּ���״̬��0���Ͽ���1���պϡ��ñ�־λ��ADC-ISR�б�������WY
	Uint16 ContactorFeedBackFlag :1; //��·�ű��̵ּ���״̬��0���Ͽ���1���պϡ��ñ�־λ��ADC-ISR�б�������WY
	Uint16 VoltageModeFlag :1;
	Uint16 FaultskipFlag :1; //������ת��־λ��0��������ת������ͣ��״̬���ϵ�Ĭ�ϣ���1����Ҫ��ת������ͣ��״̬��WY
	Uint16 faultFlag :1; //�豸���ϱ�־λ��0�������ڹ��ϣ��ϵ�Ĭ�ϣ���1�����ڹ��ϡ�WY
	Uint16 resetFlag :1;
	Uint16 stopFlag :1; //�豸ͣ����־λ��0���豸Ӧ������δͣ��״̬��1���豸Ӧ������ͣ��״̬��WY
	Uint16 startFlag :1; //�豸������־λ��0���豸Ӧ������δ����״̬��1���豸Ӧ����������״̬��WY
	Uint16 autoStFlag :4; //�Զ������Ĵ�����WY
	Uint16 realFaultFlag :1;
	Uint16 FunContDelayFlag :1; //����,.WY
	Uint16 PWMcurrDirFlag :1; //����,.WY
	Uint16 PeakStopFlag :1; //����,.WY
	Uint16 onceRunStartFlag :1;
	Uint16 FAULTCONFIRFlag :1; //�����źű�־λ��0�������ڹ����źţ�1�����ڹ����źš�WY
	Uint16 DelayFlag :1; //���ض�����£�ʹ���ж������������WY
	Uint16 StateSkipFlag :1;
	Uint16 RELAYCONTROLFlag :1; //�����̵�����־λ��0��������ɣ�1���ȴ�������WY
	Uint16 TurnOffPeakFaultFlag :1;
	Uint16 HWPowerSupFlag :1;
	Uint16 HWPowerSTOPFlag :1; //Ӳ����Դͣ����־λ��������0��δͣ����1��ͣ����WY
	Uint16 HWPowerFAULTFlag :1; //Ӳ�����ϱ�־λ����0�������ڹ��ϣ�1�����ڹ��ϡ�WY
	Uint16 FaultJudgeFlag :1;
	Uint16 BYFEEDBACKFLAG :1; //��·״̬��־λ����0���쳣��1��������WY

	float VoltIn_rms; //������ѹ��Чֵ��WY
	float gridCur_rms; //����������Чֵ��WY
	float ESC_DutyData; //ռ�ձȡ�WY
	float VoltOut_rms;
	float gridCurrBY_rms; //��·������Чֵ��WY

	int PHASE;
	int StableValue; //������ѹ�ȶ�ֵ��WY

	int VoltThreShold; //��ѹ����ֵ��WY

	int Backdiffvalue;
	int PWM_ins_index; //����ͬ��PWM�����0����·���ƣ�1����·ֱͨ��WY
	int PllDelay;
	int EscWaitRunDelayCnt;
	int EscStandyDelayCnt1; //ESC�豸��������״̬ʱ������·����תΪ��·ֱͨ�ı�Ҫʱ����WY
	int EscStandyDelayCnt2; //ESC�豸��������״̬ʱ������·ֱͨתΪ��·���Ƶı�Ҫʱ����WY
	int Volttarget;//Ŀ��ֵ
} ESCCTRLVALFLAG;
extern ESCCTRLVALFLAG ESCFlagA,ESCFlagB,ESCFlagC;

typedef struct {
	float UNIT_OVER_TEMP ; 				// ɢ��Ƭ�¶�����
	float BOARD_OVER_TEMP ; 			// ɢ��Ƭ�¶�����
	float INDUCE_OVER_TEMP; 			// �翹����Ȧ�¶�����
	float MotherBoardTempterature;		// �����¶�
	float HeatSinkTempterature;			// ɢ��Ƭ�¶�
	float EnvirTemperature;              //�����¶�
	float windColdStartTempterature;	// ɢ��Ƭ���������Ʒ�������¶�
	float windColdStopTempterature;		// ɢ��Ƭ���������Ʒ��ֹͣ�¶�
	float MotherBoardStartTempterature;	// ���ش��������Ʒ�������¶�
	float MotherBoardStopTempterature;	// ���ش��������Ʒ��ֹͣ�¶�
	float ReduceCapacityHeatSink;		// ɢ��Ƭ����ģʽ�¶�
	float BackReduceCapacityHeatSink;	// ����ģʽ�ָ��¶�
	float ReduceCapacityMotherBoard;	// ���ؽ���ģʽ�¶�
	float BackReduceCapacityMotherBoard;// ���ݻָ��ָ��¶�
	Uint16 ColdFunCnt;
	Uint16 windColdCtrMode;				// �������ģʽ1:һֱ���� 0,���¶ȴ���������
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
		Uint16 SlaveID				:8; 		//�ӻ�ID ֻ�дӻ���Ч
		Uint16 SciType 				:4;			//SCI����ѡ��
		Uint16 BaudRate				:4;			//SCI������
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
	Uint16 Month 		:8; 			//bit0~7:��
	Uint16 Year 		:8; 			//bit8~15:��
	Uint16 Hour 		:8; 			//bit0~7:ʱ
	Uint16 Day 			:8; 			//bit8~15:��
	Uint16 Second 		:8; 			//bit0~7:��
	Uint16 Minute 		:8; 			//bit8~15:��
	Uint16 MicroSec; 					//����
}Stru_Time;
extern Stru_Time Time;

typedef struct{
	Stru_Time RecordTime; 						//����ʱ��
	Uint16 FaultNumber;							//������
	Uint16 WaveRecordHandler;					//¼������
	Uint16 reserved1;							//reserved1
	Uint16 reserved2;							//reserved2
	Uint16 IOstate1;							//IO��1
	Uint16 IOstate2;							//IO��2
	int GridVoltRms; 						//������ѹ��Чֵ
	int LoadCurRms_F; 						//���ص���
	int ApfOutCurRms_F; 						//�������
	int dcVoltF; 							//ֱ����ѹ+-
    int HeatSinkTempterature;                   //ɢ�����¶�
	int EnvirTemperature;				        //�����¶�
	Uint16 gridFreq;							//����Ƶ��
	int GridActPower;							//�й�����
	int GridReactPower;							//�޹�����
	int GridCosFi;								//��������
	int dcVoltDnF;	 						//ֱ����ѹ+N
	int storageUsed;							//��������
	int SOETest1; 								//�Զ���1
	int gpVoltA_rms; 						//����A���ѹ��Чֵ
	int gpVoltB_rms; 						//����B���ѹ��Чֵ
	int gpVoltC_rms; 						//����C���ѹ��Чֵ
	int loadCurA_rms; 						//A�ฺ�ص�����Чֵ
	int loadCurB_rms; 						//B�ฺ�ص�����Чֵ
	int loadCurC_rms; 						//C�ฺ�ص�����Чֵ
	int apfOutCurA_rms; 						//A�����������Чֵ
	int apfOutCurB_rms; 						//B�����������Чֵ
	int apfOutCurC_rms; 						//C�����������Чֵ
	int gridCurA_rms; 						//A�����������Чֵ
	int gridCurB_rms; 						//B�����������Чֵ
	int gridCurC_rms; 						//C�����������Чֵ
	int GridVoltHvRms; 						//��ѹ�������ѹ��Чֵ
	int gridCurHv_rms; 						//��ѹ�����������Чֵ
	int LoadCurHvRms_F; 						//��ѹ�ฺ�ص�����Чֵ
	int ApfOutCurHvRms_F; 					//��ѹ�����������Чֵ
	int THDu_Grid; 							//������ѹ������
	int THDi_Grid; 							//��������������
	int THDi_Load; 							//���ص���������
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
	Uint16 VoltPhaseSequen			:1; 		//Bit0��		��ѹ����
	Uint16 PhaseSequenCT			:3;			//Bit1~3��	CT����
	Uint16 DirectionCT				:3;			//Bit4~6��	CT����
	Uint16 DefectCT					:2;			//Bit7~8��	CTȱʧ
    Uint16 LoadingCondition         :1;         //Bit9��         ��������
	Uint16 VoltPhaseSequenFailFlag	:1;			//Bit10��	��ѹ����У��ʧ�ܱ�־
	Uint16 PhaseSequenCTFailFlag	:1;			//Bit11��	CT����У��ʧ�ܱ�־
	Uint16 DirectionCTFailFlag		:1;			//Bit12��	CT����У��ʧ�ܱ�־
	Uint16 OverTimeFlag				:1;			//Bit13��	У����ʱ��־
	Uint16 PhaseSequenResult		:2;			//Bit14~15��	У��ʧ�ܹ��ϱ�־
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
    	Uint16 RunState_A   	      : 4;   	// 0-3λ    A������״̬
        Uint16 RunState_B             : 4;      // 4-7λ    B������״̬
        Uint16 RunState_C             : 4;      // 8-11λ  C������״̬
    	Uint16 AutoRecordReadFlag	  : 1;	    // 12λ ����¼���Զ���ȡ��־λ
     }B;
}Uni_State;
extern Uni_State StateEventFlag1;			//����״̬

//---modbus�����
typedef struct{
	Uint16 VoltTheta;      						//����Ƕ�
	int apfOutCurA_rmsF;      				//����A�����
	int apfOutCurB_rmsF;      				//����B�����
	int apfOutCurC_rmsF;      				//����C�����
	int loadCurA_rmsF;      					//���ز�A�����
	int loadCurB_rmsF;      					//���ز�B�����
	int loadCurC_rmsF;      					//���ز�C�����
	int gridCurA_rmsF;      					//������A�����
	int gridCurB_rmsF;      					//������B�����
	int gridCurC_rmsF;      					//������C�����
	int THDu_GridAF;      					//������ѹA������
	int THDu_GridBF;      					//������ѹB������
	int THDu_GridCF;      					//������ѹC������
	int THDi_LoadAF;      					//���ز����A������
	int THDi_LoadBF;      					//���ز����B������
	int THDi_LoadCF;      					//���ز����C������
	int THDi_GridAF;      					//���������A������
	int THDi_GridBF;      					//���������B������
	int THDi_GridCF;      					//���������C������
	int rvsd1[8];      						//����1
	int HarmonicCorrectionResult;            //г��У�����//0x9090
	Uni_InferRead Correction;					//�Զ�����У��
	int rvsd2[6];      						//����2

	int GridVoltRms;							//ESC�޸�   :ABC�ܵĵ�����ѹ��Чֵ  0x9098
	int GridCurRms;							//ABC�ܵĵ���������Чֵ
	int LoadVoltRms; 						//ABC�ܵĸ��ص�ѹ��Чֵ
	int LoadCurRms; 						//ABC�ܵĸ��صĵ�����Чֵ
	int VoltInA_rms; 						//A�������ѹ��Чֵ
	int VoltInB_rms; 							//B�������ѹ��Чֵ
    int VoltInC_rms;                   //C�������ѹ��Чֵ
	int VoltOutA_rms;			        //A�����������Чֵ
	int VoltOutB_rms;							//B�����������Чֵ
	int VoltOutC_rms;							//C�����������Чֵ
	int gridCurA_rms;						//A�ฺ�ص�ѹ��Чֵ
	int gridCurB_rms;								//B�ฺ�ص�ѹ��Чֵ
	int gridCurC_rms;	 						//C�ฺ�ص�ѹ��Чֵ
	int loadCurA_rms;							//A�ฺ�ص�����Чֵ
	int loadCurB_rms;						//B�ฺ�ص�����Чֵ
	int loadCurC_rms; 						//C�ฺ�ص�����Чֵ
	int HeatSinkTempterature; 						//ɢ�����¶�
	int EnvirTemperature; 						//�����¶�
	int gridFreq;                      //����Ƶ��

	int LoadVoltV_rms;                      //esc������δ��
	int LoadVoltW_rms;                      //C�ฺ�ص�ѹ��Чֵ
	int apfOutCurA_rms; 						//A�����������Чֵ
	int apfOutCurB_rms; 						//B�����������Чֵ
	int apfOutCurC_rms; 						//C�����������Чֵ
	int GardCurA_rms; 						//
	int GardCurB_rms; 						//
	int GardCurC_rms; 						//
	int GridVoltHvRms; 						//��ѹ�������ѹ��Чֵ
	int gridCurHv_rms; 						//��ѹ�����������Чֵ
	int LoadCurHvRms_F; 						//��ѹ�ฺ�ص�����Чֵ
	int ApfOutCurHvRms_F; 					//��ѹ�����������Чֵ
	int THDu_Grid; 							//������ѹ������
	int THDi_Grid; 							//��������������
	int THDi_Load; 							//���ص���������
	int THDu_GridA; 							//������ѹ������A
	int THDu_GridB; 							//������ѹ������B
	int THDu_GridC; 							//������ѹ������C
	int THDi_GridA; 							//��������������A
	int THDi_GridB; 							//��������������B
	int THDi_GridC; 							//��������������C
	int THDi_LoadA; 							//���ص���������A
	int THDi_LoadB; 							//���ص���������B
	int THDi_LoadC; 							//���ص���������C
	int gridCur_rms;							//��������
	int gridCurN_rms; 						//N�����������Чֵ
	int loadCurN_rms; 						//N�ฺ�ص�����Чֵ
	int GridActPowerA;
	int GridActPowerB;
	int GridActPowerC;
	int GridReactPowerA;
	int GridReactPowerB;
	int GridReactPowerC;
	int ApparentPowerA;
	int ApparentPowerB;
	int ApparentPowerC;
	int GridCosFiA;								//������������A
	int GridCosFiB;								//������������B
	int GridCosFiC;								//������������C
	int LoadActPowerA;
	int LoadActPowerB;
	int LoadActPowerC;
	int LoadReactPowerA;
	int LoadReactPowerB;
	int LoadReactPowerC;
	int LoadApparentPowerA;
	int LoadApparentPowerB;
	int LoadApparentPowerC;
	int LoadCosFiA;								//���ع�������A
	int LoadCosFiB;								//���ع�������B
	int LoadCosFiC;								//���ع�������C
	int rvsd3;      							//����2
	Uint16 RecordBlockNum;
	Uint16 StateEventFlag;						//����״̬
	Uint16 StartMode;							//������ʽ
	Uint16 modeChoosedFlag;						//����ģʽ
	Uint16 FaultCharacter1;						//������1
	Uint16 FaultCharacter2;						//������2
	Uint16 FaultCharacter3;						//������3
	Uint16 FaultCharacter4;						//������4
	Uint16 FaultCharacter5;						//������5
//    Uint16 FaultCharacter6;                     //������6
	int16  choose1;
	int16  choose2;
	int16  choose3;
	int16  choose4;
	int16  choose5;
	Uint16 SlaveSoftWare5;                      //�ӻ��汾:����Ƶ��
	Uint16 SlaveSoftWare1;
	Uint16 SlaveSoftWare2;
	Uint16 SlaveSoftWare3;
	Uint16 SlaveSoftWare4;
	Uint16 CommuVersion;
	Uint16 UbanRmsPerc;							//����������ƽ���
	int HV_gpVoltA_rms; 						//��ѹ�����A���ѹ��Чֵ
	int HV_gpVoltB_rms; 						//��ѹ�����B���ѹ��Чֵ
	int HV_gpVoltC_rms; 						//��ѹ�����C���ѹ��Чֵ
	int HC_gridCurA_rms; 					//��ѹ��A�����������Чֵ
	int HC_gridCurB_rms; 					//��ѹ��B�����������Чֵ
	int HC_gridCurC_rms; 					//��ѹ��C�����������Чֵ
	int HC_apfOutCurA_rms; 					//��ѹ��A�����������Чֵ
	int HC_apfOutCurB_rms; 					//��ѹ��B�����������Чֵ
	int HC_apfOutCurC_rms; 					//��ѹ��C�����������Чֵ
	int HC_loadCurA_rms; 					//��ѹ��A�ฺ�ص�����Чֵ
	int HC_loadCurB_rms; 					//��ѹ��B�ฺ�ص�����Чֵ
	int HC_loadCurC_rms; 					//��ѹ��C�ฺ�ص�����Чֵ
}Stru_Information_Structure;
extern Stru_Information_Structure Information_Structure;

typedef struct{
	Stru_Time SoeTime;
	Uint16	  FaultNum;
	int16	  FaultValue1;
	int16	  FaultValue2;
	int16	  FaultValue3;
}STRU_SoeRemote;

typedef struct{                 //��̨  ������ 0x03
    int16 VolttargetA;                       //A���ѹĿ��ֵ
    int16 VolttargetB;
    int16 VolttargetC;
    int16 VoltThreSholdA;                    //A���ѹ����ֵ
    int16 VoltThreSholdB;
    int16 VoltThreSholdC;
    int16 Backdiffvalue;                     //��ѹ�ز�ֵ
	int16 GridVoltRms; 						//������ѹ��Чֵ
	int16 GridCurRms;                      //����������Чֵ
	int16 LoadVoltRms;                     //���ص�����Чֵ
    int16 HeatSinkTempterature;                   //ɢ�����¶�
	int16 EnvirTemperature;				        //�����¶�
	int16 gridFreq;							//����Ƶ��
	int16 GridActPower;							//�й�����
	int16 GridReactPower;						//�޹�����
	int16 GridCosFi;								//��������
	int16 dcVoltDnF;	 						//ֱ����ѹ+N
	int16 storageUsed;							//��������
	int16 ApparentPower;						//���ڹ���
	int16 gpVoltA_rms; 						//����A���ѹ��Чֵ
	int16 gpVoltB_rms; 						//����B���ѹ��Чֵ
	int16 gpVoltC_rms; 						//����C���ѹ��Чֵ
	int16 loadCurA_rms; 						//A�ฺ�ص�����Чֵ
	int16 loadCurB_rms; 						//B�ฺ�ص�����Чֵ
	int16 loadCurC_rms; 						//C�ฺ�ص�����Чֵ
	int16 apfOutCurA_rms; 						//A�����������Чֵ
	int16 apfOutCurB_rms; 						//B�����������Чֵ
	int16 apfOutCurC_rms; 						//C�����������Чֵ
	int16 gridCurA_rms; 						//A�����������Чֵ
	int16 gridCurB_rms; 						//B�����������Чֵ
	int16 gridCurC_rms; 						//C�����������Чֵ
	int16 GridVoltHvRms; 						//��ѹ�������ѹ��Чֵ
	int16 gridCurHv_rms; 						//��ѹ�����������Чֵ
	int16 LoadCurHvRms_F; 						//��ѹ�ฺ�ص�����Чֵ
	int16 ApfOutCurHvRms_F; 					//��ѹ�����������Чֵ
	int16 THDu_Grid; 							//������ѹ������
	int16 THDi_Grid; 							//��������������
	int16 THDi_Load; 							//���ص���������
	int16 THDu_GridA; 							//������ѹ������A
	int16 THDu_GridB; 							//������ѹ������B
	int16 THDu_GridC; 							//������ѹ������C
	int16 THDi_GridA; 							//��������������A
	int16 THDi_GridB; 							//��������������B
	int16 THDi_GridC; 							//��������������C
	int16 THDi_LoadA; 							//���ص���������A
	int16 THDi_LoadB; 							//���ص���������B
	int16 THDi_LoadC; 							//���ص���������C
	int16 gridCur_rms;							//��������
	int16 gridCurN_rms; 						//N�����������Чֵ
	int16 loadCurN_rms; 						//N�ฺ�ص�����Чֵ
	int16 GridActPowerA;
	int16 GridActPowerB;
	int16 GridActPowerC;
	int16 GridReactPowerA;
	int16 GridReactPowerB;
	int16 GridReactPowerC;
	int16 ApparentPowerA;
	int16 ApparentPowerB;
	int16 ApparentPowerC;
	int16 GridCosFiA;								//������������A
	int16 GridCosFiB;								//������������B
	int16 GridCosFiC;								//������������C
	int16 LoadActPowerA;
	int16 LoadActPowerB;
	int16 LoadActPowerC;
	int16 LoadReactPowerA;
	int16 LoadReactPowerB;
	int16 LoadReactPowerC;
	int16 LoadApparentPowerA;
	int16 LoadApparentPowerB;
	int16 LoadApparentPowerC;
	int16 LoadCosFiA;								//���ع�������A
	int16 LoadCosFiB;								//���ع�������B
	int16 LoadCosFiC;								//���ع�������C
	int16 UbanRmsPerc;							//����������ƽ���
	int16 HV_gpVoltA_rms; 						//��ѹ�����A���ѹ��Чֵ
	int16 HV_gpVoltB_rms; 						//��ѹ�����B���ѹ��Чֵ
	int16 HV_gpVoltC_rms; 						//��ѹ�����C���ѹ��Чֵ
	int16 HC_gridCurA_rms; 					//��ѹ��A�����������Чֵ
	int16 HC_gridCurB_rms; 					//��ѹ��B�����������Чֵ
	int16 HC_gridCurC_rms; 					//��ѹ��C�����������Чֵ
	int16 HC_apfOutCurA_rms; 					//��ѹ��A�����������Чֵ
	int16 HC_apfOutCurB_rms; 					//��ѹ��B�����������Чֵ
	int16 HC_apfOutCurC_rms; 					//��ѹ��C�����������Чֵ
	int16 HC_loadCurA_rms; 					//��ѹ��A�ฺ�ص�����Чֵ
	int16 HC_loadCurB_rms; 					//��ѹ��B�ฺ�ص�����Чֵ
	int16 HC_loadCurC_rms; 					//��ѹ��C�ฺ�ص�����Чֵ
	Uint16 StateEventFlag;						//����״̬
	Uint16 StartMode;							//������ʽ
	Uint16 modeChoosedFlag;						//����ģʽ
	Uint16 FaultCharacter1;						//������1
	Uint16 FaultCharacter2;						//������2
	Uint16 FaultCharacter3;						//������3
	Uint16 FaultCharacter4;						//������4
	Uint16 FaultCharacter5;						//������5
	Uint16 CapStateUp[18];
//	Stru_Time RemoteTime;
//	STRU_SoeRemote	SoeRemote[30];
}Stru_RemoteRefresh_Structure;
extern Stru_RemoteRefresh_Structure RemoteRefreshData;

typedef struct{        //��̨  ������ 0x05
    Uint16 RemoteStart;
    Uint16 RemoteStop;
    Uint16 RemoteReset;
}Stru_RemoteCtrl_Structure;
extern Stru_RemoteCtrl_Structure RemoteStateCtrl;

typedef struct{        //��̨  ������ 0x06
//    int16 ReactpowerOrder;
//    int16 Voltuplimit;
//    int16 Voltlowlimit;
//    int16 Voltoverlimitcompentarvalue;
//    int16 Voltunderlimitcompentarvalue;
    int16 VolttargetA;             //A���ѹĿ��ֵ
    int16 VolttargetB;
    int16 VolttargetC;
    int16 VoltThreSholdA;          //A���ѹ����ֵ
    int16 VoltThreSholdB;
    int16 VoltThreSholdC;
    int16 Backdiffvalue;           //�ز�ֵ

}Stru_RemoteInstruct_Structure;
extern Stru_RemoteInstruct_Structure RemoteInstructCtrl;

typedef struct{        //��̨  ������ 0x02
    Uint16 StateRunStop;
    Uint16 StateWait;
    Uint16 StateFault;
    Uint16 rsvd[13];
    Uint16 ESCFastHardwareOverCurFlagA;            // 0λ��ESC����Ӳ����������A
    Uint16 ESCFastHardwareOverCurFlagB;            // 1λ��ESC����Ӳ����������B
    Uint16 ESCFastHardwareOverCurFlagC;            // 2λ��ESC����Ӳ����������C
    Uint16 ESCInsOverCurFlagA;                     // 3λ��ESC˲ʱֵ��������A
    Uint16 ESCInsOverCurFlagB;                     // 4λ, ESC˲ʱֵ��������B
    Uint16 ESCInsOverCurFlagC;                     // 5λ��ESC˲ʱֵ��������C
    Uint16 ESCDcCapVoltOverVoltFlagA;              // 6λ��ESCֱ�����ݵ�ѹ��ѹ����A
    Uint16 ESCDcCapVoltOverVoltFlagB;              // 7λ��ESCֱ�����ݵ�ѹ��ѹ����B
    Uint16 ESCDcCapVoltOverVoltFlagC;              // 8λ��ESCֱ�����ݵ�ѹ��ѹ����C
    Uint16 ESCGridRMSOverVoltFlagA;                // 9λ��ESC������ѹ��Чֵ��ѹ����A
    Uint16 ESCGridRMSOverVoltFlagB;                //10λ��ESC������ѹ��Чֵ��ѹ����B
    Uint16 ESCGridRMSOverVoltFlagC;                //11λ��ESC������ѹ��Чֵ��ѹ����C
    Uint16 ESCPowerFailDetectFlag;                 //12λ��ESC���������
    Uint16 ESCGridRMSUnderVoltFlagA;               //13λ��ESC������ѹ��ЧֵǷѹ����A
    Uint16 ESCGridRMSUnderVoltFlagB;               //14λ��ESC������ѹ��ЧֵǷѹ����B
    Uint16 ESCGridRMSUnderVoltFlagC;               //15λ��ESC������ѹ��ЧֵǷѹ����C
    Uint16 ESCOverLoadFlagA;                       // 0λ��ESC������ع���A
    Uint16 ESCOverLoadFlagB;                       // 1λ��ESC������ع���B
    Uint16 ESCOverLoadFlagC;                       // 2λ��ESC������ع���C
    Uint16 ESCRmsOverCurrentFlagA;                 // 3λ��ESC��Чֵ�����������A
    Uint16 ESCRmsOverCurrentFlagB;                 // 4λ��ESC��Чֵ�����������B
    Uint16 ESCRmsOverCurrentFlagC;                 // 5λ��ESC��Чֵ�����������C
    Uint16 ESCOverTemperatureFlagA;                // 6λ��ESCɢ��Ƭ�¶ȹ��¹���A
    Uint16 ESCOverTemperatureFlagB;                // 7λ�� ESCɢ��Ƭ�¶ȹ��¹���B
    Uint16 ESCOverTemperatureFlagC;                // 8λ��ESCɢ��Ƭ�¶ȹ��¹���C
    Uint16 ESCFangLeiFaultFlag;                    // 9λ��ESC����������
    Uint16 ESCBypassContNoCloseFlagA;              //10λ��ESC��·��բ����A
    Uint16 ESCBypassContNoCloseFlagB;              //11λ��ESC��·��բ����B
    Uint16 ESCBypassContNoCloseFlagC;              //12λ��ESC��·��բ����C
    Uint16 ESCChassisOverTempFlag;                 //13λ��ESC��Ԫ����(����)
    Uint16 rsvd1;                                  //14λ   ����
    Uint16 rsvd2;                                  //15λ������
    Uint16 ESCCalibrarionFailureFlag;              // 0λ��ESC��ƫУ׼�������
    Uint16 ESCResonanceFlautFlag;                  // 1λ��ESCг�����
    Uint16 ESCGridOverFreqenceFlag;                // 2λ��ESC����Ƶ�ʹ�Ƶ����
    Uint16 ESCGridUnderFreqenceFlag;               // 3λ��ESC����Ƶ��ǷƵ����
    Uint16 ESCTieDianReadErrFlag;                  // 4λ��ESC�������
    Uint16 ESCAutoStartMuchFaultFlag;              // 5λ��ESC������Ƶ������
    Uint16 rsvd3;                                  // 6λ������
    Uint16 rsvd4;                                  // 7λ������
    Uint16 rsvd5;                                  // 8λ������
    Uint16 rsvd6;                                  // 9λ������
    Uint16 rsvd7;                                  // 10λ������
    Uint16 rsvd8;                                  // 11λ������
    Uint16 rsvd9;                                  // 12λ������
    Uint16 rsvd10;                                  // 13λ������
    Uint16 rsvd11;                                  // 14λ������
    Uint16 rsvd12;                                  // 15λ������
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
    int16 gridVoltHAOffset;              //������ʾ��ƫУ׼ֵ
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
	Uint16 Theta;								//����Ƕ�
	Uint16 apfOutCurA_rms; 						//A�����������Чֵ
	Uint16 apfOutCurB_rms; 						//B�����������Чֵ
	Uint16 apfOutCurC_rms; 						//C�����������Чֵ
	Uint16 loadCurA_rms; 						//A�ฺ�ص�����Чֵ
	Uint16 loadCurB_rms; 						//B�ฺ�ص�����Чֵ
	Uint16 loadCurC_rms; 						//C�ฺ�ص�����Чֵ
	Uint16 gridCurA_rms; 						//A�����������Чֵ
	Uint16 gridCurB_rms; 						//B�����������Чֵ
	Uint16 gridCurC_rms; 						//C�����������Чֵ
	Uint16 THDu_GridA; 							//������ѹ������A
	Uint16 THDu_GridB; 							//������ѹ������B
	Uint16 THDu_GridC; 							//������ѹ������C
	Uint16 THDi_LoadA; 							//���ص���������A
	Uint16 THDi_LoadB; 							//���ص���������B
	Uint16 THDi_LoadC; 							//���ص���������C
	Uint16 THDi_GridA; 							//��������������A
	Uint16 THDi_GridB; 							//��������������B
	Uint16 THDi_GridC; 							//��������������C
	Uint16 gpVoltA_rms; 						//����A���ѹ��Чֵ
	Uint16 gpVoltB_rms; 						//����B���ѹ��Чֵ
	Uint16 gpVoltC_rms; 						//����C���ѹ��Чֵ
	Uint16 rvsd1[8];      						//����
}Stru_Multiple_Parallel_Value;
extern Stru_Multiple_Parallel_Value Multiple_Parallel_Value;

//����¼��������
struct Stru_FaultRecSel{
	Uint16 RecordMode;              //��������
	Uint16 RecordChan;              //����ͨ��
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
	Uint16 ManualMode							    :1; 		//Bit0��        �ֶ�ģʽ
    Uint16 AutomaticMode                            :1;         //Bit1��        У��ģʽ
	Uint16 VoltageDistortionOverFlag 				:1;			//Bit2:		��ѹ�����ʳ��ޱ�־
	Uint16 PowerGridCurrDistortionOverFlag			:1;			//Bit3:		�������������ʳ��ޱ�־
	Uint16 LoadCurrDistortionOverFlag				:1;			//Bit4:		���ص��������ʳ��ޱ�־
	Uint16 PowerGridCurrSetOverFlag					:1;			//Bit5:		���������趨ֵ���ޱ�־
	Uint16 LoadCurrSetOverFlag						:1;			//Bit6:		���ص����趨ֵ���ޱ�־
	Uint16 AverageCurrent							:1;			//Bit7��	        ����ģʽ
	Uint16 CalibrationFrequency						:4;			//Bit8~11��  	У������
	Uint16 CorrectionRoundNumber					:4;			//Bit12~15�� У������
}Stru_HarmonicInfer;

typedef union{
	Uint16 all;
	Stru_HarmonicInfer B;
}Uni_HarmonicInfer;

typedef struct{
	Uint16 VoltagePhaseSequence		:1; 		//Bit0��		��ѹ����
	Uint16 PhaseSequenceCT			:3;			//Bit1~3��	CT����
	Uint16 DirectionCT				:3;			//Bit4~6��	CT����
	Uint16 DefectCT					:2;			//Bit7~8��  	CTȱʧ0:��ȱʧ,1:C��ȱʧ,2:B��ȱʧ,3:A��ȱʧ
	Uint16 rvsd						:7;			//Bit9��
}Stru_Infer;

typedef union{
	Uint16 all;
	Stru_Infer B;
}Uni_Infer;

struct Stru_FactorySet{
//ԭ��������
	int PhaseControl;               //ESC��λ����                       //Q2D_ratio;					//1 QD����ϵ��
	int LoadCurlimit;               //ESC���ص�������              //int D2Q_ratio;					//2 ����У��
	int SyncHardwareLead;			//3 ͬ����ǰ����
	int SampleLead;					//4 ������ǰ����
	int T1PR;						//5 ��������
	int reactPowGivenLimit;			//6 �����޹��޷� //δʹ��
	int Curr_MinCtrlFAN;			//7��С���ؿ��Ʒ������
	int Curr_MaxCtrlFAN;		    //8����ؿ��Ʒ��ȫ��
	int Temp_MinCtrlFAN;			//9��С�¶ȿ��Ʒ������
	int Temp_MaxCtrlFAN;			//10����¶ȿ��Ʒ��ȫ��
	int FanStartMinDUTY;			//11���������Сռ�ձ�
	int RateLoadCurrLimit;		    //12���������
	int CapacitySelection;		    //13����ѡ��
	int dcCapVoltRatio;				//14ESCֱ�����ݵ�ѹ����
	int loadVoltRatio;				//15ESC���ص�ѹ����
	int gridVoltRatio;				//16 ������ѹ����
	int outputCurRatio;				//17 �����������
	int loadCurRatio;				//18
	int AutoStartMode;				//19
	int SPWM;						//20 spwm��־λ
	int RecordMode;					//21 ����¼����־λ
	int isHarmCompensateMode;		//22 BW�˲���־λ
	int bypassCurrRatio;		    //23 ESC��·����ϵ��
	int DC_ERR_LIMIT;				//24 �е���Ʊ���
	int PIlim_Udc;					//25 ֱ����ѹ����
	int PIlim_I;					//26 PWM�ڻ���������
	int phaseErrLimit;				//27 ��������޷�
	int kp_Pll;						//28  �������
	int ki_Pll;						//29 �������
	int kp_Dc_Div;					//30 �е���Ʊ���
	int ki_Dc_Div;					//31 �е���ƻ���
	int kp_Dc;						//32 ֱ����ѹ����
	int ki_Dc;						//33 ֱ����ѹ����
	int kp_Cur;						//34 ����������
	int ki_Cur;						//35 ����������
	int GV_RMS_UNDER;				//36
	int GV_RMS_OVER;				//37
	int GF_OVER;					//38������Ƶ
	int GF_UNDER;					//39����ǷƵ
	int ESCDCVOLITLIMIT;			//40ESCֱ�����ݵ�ѹ����ֵ
	int VoltFallResponseLimit;		//41ESC��ѹ���������Ӧ��ֵ
	int OUTCUR_OVER_INS;			//42˲ʱ��������
	int OUTCUR_OVER_RMS;			//43������Чֵ����
	int OUT_OVER_LOAD;				//44�����������
	int OutCurMaxLimit;				//45��������޷�
	int UNIT_OVER_TEMP;				//46ɢ��������
	int reactPrCompPerc;            //47�޹�����ϵ��
	int harmCompPerc;               //48ESC���Թ̶�ռ�ձ�
	int negCurCompPerc;             //49ESC IGBT��������ʱ��
	int ESCTESTDATA1;
	int ESCTESTDATA2;
	int ESCTESTDATA3;
	int ESCTESTDATA4;
	int TempEnvirProvalue;
	int THD_K;
	int THD_B;
	int VoltProport;			//��ѹ����P
	int VoltIntegral;			//��ѹ����I
	int VoltDerivative;			//ESC PI�޷�ֵ
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
	int R_SSwitchTime;          //ESC ����ת����ʱ��
	int S_RSwitchTime;          //ESC ����ת����ʱ��
	int WholeOutCurRmsLimit;
	int StandCosFiLimit;	//
	int StandUbanCurPer;	//
	Uint16 BOARD_OVER_TEMP;		//��Ԫ����
	//��������
	Uint16 BG_MODEL;			//��������
//	Uint16 LED_MODEL;
	INITCSI INIT_SCIA_BAUD;		//����A������
	Uint16 CommunicationVersion;//communicationVersion
	INITCSI INIT_SCIB_BAUD;		//����B������
	INITCSI INIT_SCIC_BAUD;		//����C������
	Uni_Infer Infer;	//�Զ�����У��
	Uni_HarmonicInfer HarmonicInfer;	//�Զ�г��У��
	Uint16 VoltageDistortionValue;				//��ѹ�������趨ֵ
	Uint16 PowerGridCurrDistortionValue;		//����������������趨ֵ
	Uint16 LoadCurrDistortionValue;				//���ز�����������趨ֵ
	Uint16 PowerGridCurrSetPoint;				//����������趨ֵ
	Uint16 LoadCurrSetPoint;					//���ز�����趨ֵ
	Uint16 CompensatingHarmonicQuantity;
//��������
	Uint16 StandbyRecoveryTime;
	Uint16 AutoCurr;							//�ز����
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

struct VARIZEROVAL{         //�����·�����ƫֵ
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
	Uint16 apfWiringmethod		:1; 		//Bit0�����߷�ʽ,0:���������� 1:����������
	Uint16 startingMethod		:1;			//Bit1��������ʽ,0:�Զ�ģʽ 1:�ֶ�ģʽ
	Uint16 positionCT			:1;			//Bit2��CTλ��,0:������ 1:���ز�
	Uint16 harmCompEn			:1;			//Bit3��г������ģʽ,0:�ر� 1:��
	Uint16 negCurCompFlag		:1;			//Bit4����ƽ�ⲹ��ģʽ,0:�ر� 1:��
	Uint16 AVCMode				:1;			//Bit5��AVCģʽ,0:�ر� 1:��
	Uint16 constantQFlag		:2;			//Bit6~7���޹�����ģʽ,	0:���޹�����  1:���޹�����
											//		 				2:�㹦������  3:�ر��޹�����
	Uint16 VoltageMode			:3;			//Bit8~10��ESC��ѹ����ģʽ,	0:��ѹ  1:��ѹ 2:����ѹ����ģʽ 3:��ƽ�ⲹ��ģʽ 4:�޹�����ģʽ
	Uint16 prioritizedModeFlag	:2;			//Bit11~12�������趨,0,1:�޹�����2:г������3:��ƽ������
											//					4:���ٲ���5:�Զ�����6:��ѹ����
	Uint16 StandbyModeFlag		:1;			//Bit13������ģʽ������־
	Uint16 TransformerSection	:2;			//Bit14~15����ѹ���������
}Stru_WordMode;
typedef union{
	Uint16 all;
	Stru_WordMode B;
}Uni_WordMode;

//extern Uni_WordMode WordMode;

struct Stru_UserSetting{		//eeprom start:0x00.length
	Uni_WordMode WordMode;
	int TargetCurrentUnbalance;		//��ƽ���Ŀ��ֵ
	int transfRatio;			//CT���
	int GV_RMS_UNDER;			 //������ЧֵǷѹ
	int GV_RMS_OVER;			 //������Чֵ��ѹ
    int Backdiffvalue;           //�ز�ֵ
	int ConstantReactivePower;  //���޹�����ָ��
	int TransformRatioParall;
	int VolttargetA;             //A���ѹĿ��ֵ
	int VolttargetB;             //B���ѹĿ��ֵ
	int VolttargetC;             //C���ѹĿ��ֵ
	int VoltThreSholdA;          //A���ѹ����ֵ
	int VoltThreSholdB;          //B���ѹ����ֵ
	int VoltThreSholdC;          //C���ѹ����ֵ
	int AutoWaitCurr;
	int Mode_PP;
	Uint16 CRC;
};
extern struct Stru_UserSetting UserSetting;
extern struct Stru_UserSetting UserParamRatio;

struct Stru_APFID{		//eeprom start:0x8D0.length
	Uint16 RandIDH;				//��Ԫƥ���ַ��
	Uint16 RandIDL;             //��Ԫƥ���ַ��
	Uint16 APF_ID;				//��Ԫ��ַ
	Uint16 MU_UNIT_NUM;			//��Ԫ����
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



// ���ƺ���������
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

//DSP2837x_Cla.c �����ĺ���
extern void CLA_configClaMemory(void);
extern void CLA_initCpu1Cla1(void);
#endif /* VARIBLESREF_H_ */
