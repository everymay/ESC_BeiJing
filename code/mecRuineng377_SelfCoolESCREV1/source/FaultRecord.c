#include "F2837xS_device.h"
#include "F2837xS_Examples.h"
#include "VariblesRef.h"
#include "MarcoDefine.h"
#include "DCL.h"
#include "DPLL.h"

// 功能解释：
/* 可以记录正常停机或者故障停机后的数据
 * 实现方法：
 * 1、外部RAM存放十组数据，每组数据2000个整形。
 * 2、通信协议为（从机地址）0x01（数据地址头）0x3001（存储地址段）0x0000（传输数据）
 * 3、存储位置：
 */
#pragma	CODE_SECTION(FaultRecordProg ,"ram2func")
//#pragma DATA_SECTION(FaultRecSel,"EBSS3");
struct Stru_FaultRecSel FaultRecSel;
const int FaultRecSelTab[4][8]={{0,1,2,3,6,12,24,48},{0,1,2,3,6,12,24,48},{0,1,2,4,8,16,32,64},{0,1,2,5,10,20,40,80}};		//8k,9.6k,12.8k,16k
const int FFTRecSelTab[4][10]={{1,1,1,1},{1,1,0,1,1,0},{1,0,1,0,1,0,1,0},{1,0,0,1,0,0,1,0,0,1}};		//8k,9.6k,12.8k,16k

//extern float ratio, offset;
//extern float debug_Y,debugA_Y,debugB_Y,debugC_Y,debugA_ratio,debugA_offset,debugB_ratio,debugB_offset;
//extern float dbg_reactive,escdebugpll;

void SetWaveRecordMode(Uint16 mode)   //记录故障波形的模式
{
	FaultRecSel.RecordChan=FaultRecSel.RecordChanOrg = mode     % 100;	//MDSPP:PP:选择不同的组录波
	Uint16 SampRate 			=(mode%10000)/100;						//S:界面观察速率
	if(SampRate>=7) SampRate =7;
	FaultRecSel.stopRecordMode	= mode/10000;							//MDSPP:M:1:一直录波.无论是否停机.0:停机或故障后自动停止录波.保留最后2000个点的波形数据
	//0: 1倍采样速率 (根据从机自动按照最高速率16k,19.2k,25.6k,32k)	1: 1/2倍(8k,9.6k,12.8k,16k)	2: 6.4k	3: 3.2k	4: 1.6k	5: 0.8k	6: 0.4k	7: 0.2k
	//设置FFT模式下的录波通道.并且保证FFT采样点与ADBufPos严格对应,保证多机采样精度
	FaultRecSel.RecordCntSet = FaultRecSelTab[ADGROUP_MODE][SampRate];	//采样间隔
	FlashApiDataCopy(SampRate);
}

// 功能解释：
// 故障发生前,一直录波.故障发生后的时候，再记录0.5个周波的数据,上传到LCD
//RecordMode:录波通道选择: 0:停止录波.1~30:前版本的30种不同通道组合的录波方式,录波通道为10组.31~60:新版本的30种不同通道组合的录波方式,录波通道为20组
//           98:无论装置是否有故障,一直录波.99:装置故障后停止录波
//RefreshMode:刷新方式 00:维持上一个状态               01:停止刷新缓冲区
//                   02:一直刷新缓冲区               03:刷新一次缓冲区后停止.
//DispSampRate:采样率00:FFT用采样速率(固定为6.4k)   02:1/2倍(8k,9.6k,12.8k,16k)
//01:1倍采样速率 (根据从机自动按照最高速率16k,19.2k,25.6k,32k)
//03:1/4倍(4k,4.8k,6.4k,8k)       04:1/8倍(1k,2.4k,3.2k,4k)
int TestChan = 0,TESTPhase=0;
//extern float32 testout[];
//extern float32 testin[];
//extern float rtb_Vq[];
//extern Uint16 MeanHalfPos;
//extern float CurrAD,ZCPJudgea,ZCPJudgeb;
//extern float CurrentV,DBG_TBCTR,DBG_Esc_Phase,VolCha;
void FaultRecordProg(void)
{
//	float *pWave=DebugInner;
//	int StartDmaFlag=1;
//	int tmp;
	struct Stru_FaultRecSel *pSel=&FaultRecSel;
	//剩余可录波点数>=0 && ( 非fft模式 && 固定点数据间隔录波条件满足  ||  fft模式 && (FFT条件1 || FFT条件2))
	if(	 (pSel->DelayRec >= 0)&&\
		(	((!pSel->isFFTMode)&&(++pSel->RecordCnt > pSel->RecordCntSet))||\
			(( pSel->isFFTMode)&&(FFTRecSelTab[ADGROUP_MODE][ADBufPos]) ))){          //采样间隔
		pSel->DelayRec--;	//录波一个点,计数器减一
		pSel->RecordCnt=0;

		switch(pSel->RecordChan){				//debugW
//		case 0:StartDmaFlag=0;		//没有故障录波
//		break;

		case 1:						//FFT模式
            debugW[0][CntForRec] = GridVoltAF;//GridVoltBF;//(AdccResultRegs.ADCRESULT12 - VirtulAD_loadCurrentB)*outputCurRatio;//GridVoltBF;//dbg_claPwmReg[0];
            debugW[1][CntForRec] = GridVoltBF;//LoadVoltVF;//LoadVoltUF;//GridVoltBF;//(AdccResultRegs.ADCRESULT14 - VirtulAD_loadCurrentB)*outputCurRatio;//LoadVoltVF;//dbg_claPwmReg[1];//EPwm8Regs.CMPA.bit.CMPA;////GridVoltBF;//dbg_claPwmReg[0];//GridVoltBF;//ZCPJudgea;
            debugW[2][CntForRec] = GridVoltCF;//GridCurrBF;//DEBUGData;//GridCurrAF;//GridVoltCF;//(AdccResultRegs.ADCRESULT13 - VirtulAD_loadCurrentB)*outputCurRatio;//GridCurrBF;//dbg_claPwmReg[2];////GridVoltCF;//dbg_claPwmReg[1];//GridVoltCF;//ZCPJudgeb;
            debugW[3][CntForRec] = LoadVoltUF;//Esc_VoltPhaseA;//testvalue1;//PhaseValue;//LoadVoltUF;//LoadVoltUF;//Esc_VoltPhaseB;//(AdccResultRegs.ADCRESULT15 - VirtulAD_loadCurrentB)*outputCurRatio;//dbg_claPwmReg[3];//GridCurrAF;//LoadVoltUF;//Esc_VoltPhaseA;//GridCurrAF;
            debugW[4][CntForRec] = LoadVoltVF;//GpioDataRegs.GPEDAT.bit.GPIO154;//ESC_DutyDataA;//TESTRMSGridInVAL;//LoadVoltVF;//LoadVoltVF;//Esc_CurPhaseB;//dbg_claWave[0];//GridCurrBF;//LoadVoltVF;//dbg_claWave[0];//CurrAD;
            debugW[5][CntForRec] = LoadVoltWF;//GpioDataRegs.GPCDAT.bit.GPIO78;//EPwm7Regs.CMPB.bit.CMPB;//TESEINSGridIn;//LoadVoltWF;//LoadVoltWF;//SPLL[1].Theta;//dbg_claWave[1];//GridCurrCF;//LoadVoltW0BF;//dbg_claWave[1];//GridCurrCF;
            debugW[6][CntForRec] = GridCurrAF;//GpioDataRegs.GPCDAT.bit.GPIO95;//EPwm7Regs.CMPA.bit.CMPA;//NumeratorValue;//GridCurrAF;//dutytmp1;//Esc_VoltPhaseA;//GridCurrAF;//SPLL_CUR.Theta;//dbg_claWave[2];//LoadVoltUF;//gridCurA_rms;//GridCurrAF;
            debugW[7][CntForRec] = GridCurrBF;//GpioDataRegs.GPEDAT.bit.GPIO156;//EPwm8Regs.CMPB.bit.CMPB;//DenominatorVAL;//GridCurrBF;//TESEINSGridIn;//Esc_VoltPhaseB;//GridCurrBF;//SPLL[1].PllPiOutput;//dbg_claWave[3];//LoadVoltVF;//gridCurB_rms;//GridCurrBF;//dbg_claWave[2];//debugA_ratio*10000;//LoadVoltVF;
            debugW[8][CntForRec] = GridCurrCF;//EPwm8Regs.CMPA.bit.CMPA;//dutytmp;//GridCurrCF;//Esc_VoltPhaseC;//GridCurrCF;//SPLL_CUR.PllPiOutput;//dbg_claWave[4];//T1PRmulESC_DutyDataBaddnegCurCompPerc>>16;//LoadVoltWF;//gridCurC_rms;//GridCurrCF;//dbg_claWave[3];//debugA_offset*10;//ratio;//VolCha;
            debugW[9][CntForRec] = GridBPCurrAF;//GpioDataRegs.GPCDAT.bit.GPIO95;//PWM_ins_indexA;//SPLL[0].Theta;//dutytmp1;//GridCurrAF;//SPLL[0].PllPiOutput;//GridCurrAF;//VoltInBF;//dbg_claWave[5];//T1PRmulESC_DutyDataBsubnegCurCompPerc>>16;//SPLL[0].Theta*100;//gridCurA_rms;//dbg_claPwmReg[4];//;//debugA_Y;//offset;
            debugW[10][CntForRec] = GridBPCurrBF;//GpioDataRegs.GPEDAT.bit.GPIO156;//TESTRMSGridInVAL;//EPwm8Regs.CMPA.all;//TESTVALUE;//ESC_DutyDataB;//GridCurrBF;//SPLL[1].PllPiOutput;//GridCurrBF;//VoltOutBF;//(AdccResultRegs.ADCRESULT8 - VirtulAD_loadCurrentB)*outputCurRatio;//dbg_claPwmReg[4];//EPwm8Regs.CMPA.bit.CMPA;//SPLL[1].Theta*100;//gridCurB_rms;//dbg_claPwmReg[2];//debugB_Y;
            debugW[11][CntForRec] = GridBPCurrCF;//ArithFlag;//EPwm7Regs.CMPB.all;//ArithFlag;//testvalue1;//GridCurrCF;//SPLL[2].PllPiOutput;//GridCurrCF;//(AdccResultRegs.ADCRESULT10 - VirtulAD_loadCurrentB)*outputCurRatio;//dbg_claPwmReg[5];//EPwm8Regs.CMPB.bit.CMPB;//SPLL[2].Theta*100;//idCurC_rms;//dbg_claPwmReg[3];//debugC_Y;

			break;

//		case 1:					//调试故障录波
//		    debugW[0][CntForRec] = GridVoltAF;
//		    debugW[1][CntForRec] = ErrorRecord.all;//CurrHarmRefA;//VoltTheta;//ctrlVoltA
//		    debugW[2][CntForRec] = currentRefD;//CurrHarmRefA;//pllPiOutput;
//		    debugW[3][CntForRec] = dcVoltF;//CurrRefA;
//		    debugW[4][CntForRec] = dcVoltDn;//InvFundaCurQ;//negCurCompQ
//		    debugW[5][CntForRec] = CurrRefA;//LoadFundaCurQ;//dcVoltDiv2Err;//pwmTsGiven;
//		    debugW[6][CntForRec] = CurrRefB;//GridPLLVoltD;//ctrlVoltB;//dcVoltUpF;//AGCVal;
//		    debugW[7][CntForRec] = Rec_sa;//dcVoltDn;//CurrHarmRefA;////LoadFundaCurZ;//LoadRealCurB
//		    debugW[8][CntForRec] = Rec_sb;//GridPLLVoltQ;//CurrHarmRefA;//dcVoltUpF;//UdcBalanceCurr;
//		    debugW[9][CntForRec] = ApfOutCurA;//RmsSum[0];//currentRefD;//VoltSlidA[VoltPos];//VoltPos;
//			//10
//		    debugW[10][CntForRec] = GridVoltTheta;//CurFundA;//VoltSlidA[MeanPos_F];//CurrHarmRefA;
//		    debugW[11][CntForRec] = GridResCos;//PwmVc;//RmsSumQ[0];//dcBusVtIncStart;
//			break;

		case 91:					//售后看相位
            debugW[0][CntForRec] = VolttargetCorrC;//GridVoltAF;//TESEINSGridIn;//VoltInA_rms;//VoltInAF;//GridVoltBF;//(AdccResultRegs.ADCRESULT12 - VirtulAD_loadCurrentB)*outputCurRatio;//GridVoltBF;//dbg_claPwmReg[0];
            debugW[1][CntForRec] = CurrTargetTemper;//GridVoltBF;//GridCurrAF;//TESEINSGridIn;//VoltInAF;//VoltInBF;//VoltOutCF;//LoadVoltVF;//LoadVoltUF;//GridVoltBF;//(AdccResultRegs.ADCRESULT14 - VirtulAD_loadCurrentB)*outputCurRatio;//LoadVoltVF;//dbg_claPwmReg[1];//EPwm8Regs.CMPA.bit.CMPA;////GridVoltBF;//dbg_claPwmReg[0];//GridVoltBF;//ZCPJudgea;
            debugW[2][CntForRec] = gridCurC_rms;//GridVoltCF;//NumeratorValue;//GridCurrAF;//VoltInCF;//GridCurrBF;//DEBUGData;//GridCurrAF;//GridVoltCF;//(AdccResultRegs.ADCRESULT13 - VirtulAD_loadCurrentB)*outputCurRatio;//GridCurrBF;//dbg_claPwmReg[2];////GridVoltCF;//dbg_claPwmReg[1];//GridVoltCF;//ZCPJudgeb;
            debugW[3][CntForRec] = ESCFlagC.ESC_DutyData;//Esc_VoltPhaseA;//PhaseValue;//TESEINSGridIn;//VoltInBF;//VoltOutAF;//ESCFlagC.PWM_ins_index;//Esc_VoltPhaseA;//testvalue1;//PhaseValue;//LoadVoltUF;//LoadVoltUF;//Esc_VoltPhaseB;//(AdccResultRegs.ADCRESULT15 - VirtulAD_loadCurrentB)*outputCurRatio;//dbg_claPwmReg[3];//GridCurrAF;//LoadVoltUF;//Esc_VoltPhaseA;//GridCurrAF;
            debugW[4][CntForRec] = ConstantCurr[2].CorrPI.i10;//GridRealCurErrA;//LoadVoltVF;//ArithFlagA;//VoltOutBF;//VoltOutBF;//EPwm5Regs.CMPA.bit.CMPA;//GpioDataRegs.GPEDAT.bit.GPIO154;//ESC_DutyDataA;//TESTRMSGridInVAL;//LoadVoltVF;//LoadVoltVF;//Esc_CurPhaseB;//dbg_claWave[0];//GridCurrBF;//LoadVoltVF;//dbg_claWave[0];//CurrAD;
            debugW[5][CntForRec] = GpioDataRegs.GPDDAT.bit.GPIO97;//ESCFlagC.Volttarget*(1+VolttargetCorrC);//dbg_reactive;//LoadVoltWF;//dutytmp1;//ESCFlagA.ESCCntMs.HarmDisDelay;//dutytmp1;//GridCurrBF;//EPwm3Regs.CMPA.bit.CMPA;//VoltOutCF;//EPwm5Regs.CMPB.bit.CMPB;//GpioDataRegs.GPCDAT.bit.GPIO78;//EPwm7Regs.CMPB.bit.CMPB;//TESEINSGridIn;//LoadVoltWF;//LoadVoltWF;//SPLL[1].Theta;//dbg_claWave[1];//GridCurrCF;//LoadVoltW0BF;//dbg_claWave[1];//GridCurrCF;
            debugW[6][CntForRec] = gridCurrBYCF_rms;//GridCurrAF;//TESTRMSGridInVAL;//ESCFlagA.ESC_DutyData;//ArithFlag;//VoltInCF;//EPwm3Regs.CMPB.bit.CMPB;//GridCurrAF;//EPwm6Regs.CMPA.bit.CMPA;//GpioDataRegs.GPCDAT.bit.GPIO95;//EPwm7Regs.CMPA.bit.CMPA;//NumeratorValue;//GridCurrAF;//dutytmp1;//Esc_VoltPhaseA;//GridCurrAF;//SPLL_CUR.Theta;//dbg_claWave[2];//LoadVoltUF;//gridCurA_rms;//GridCurrAF;
            debugW[7][CntForRec] = GridCurrCF;//TestArithFlagA;//VoltInA_rms;//ESCFlagA.ESC_DutyData;//VoltOutCF;//EPwm4Regs.CMPA.bit.CMPA;//GridCurrBF;//EPwm6Regs.CMPB.bit.CMPB;//GpioDataRegs.GPEDAT.bit.GPIO156;//EPwm8Regs.CMPB.bit.CMPB;//DenominatorVAL;//GridCurrBF;//TESEINSGridIn;//Esc_VoltPhaseB;//GridCurrBF;//SPLL[1].PllPiOutput;//dbg_claWave[3];//LoadVoltVF;//gridCurB_rms;//GridCurrBF;//dbg_claWave[2];//debugA_ratio*10000;//LoadVoltVF;
            debugW[8][CntForRec] = VoltInCF;//GridCurrCF;//VoltInAF;//TESETarget;//GpioDataRegs.GPEDAT.bit.GPIO156;//GridCurrCF;//EPwm4Regs.CMPB.bit.CMPB;//GridCurrCF;//StateEventFlag_B;//ESCFlagC.ESC_DutyData;//StateEventFlag;//EPwm8Regs.CMPA.bit.CMPA;//dutytmp;//GridCurrCF;//Esc_VoltPhaseC;//GridCurrCF;//SPLL_CUR.PllPiOutput;//dbg_claWave[4];//T1PRmulESC_DutyDataBaddnegCurCompPerc>>16;//LoadVoltWF;//gridCurC_rms;//GridCurrCF;//dbg_claWave[3];//debugA_offset*10;//ratio;//VolCha;
            debugW[9][CntForRec] = ConstantCurr[2].state;//ESCFlagA.ESC_DutyData;//GridBPCurrAF;//dutytmp;//ArithVAL;//GpioDataRegs.GPEDAT.bit.GPIO156;//GridBPCurrAF;//GpioDataRegs.GPFDAT.bit.GPIO164;//GridBPCurrAF;//GpioDataRegs.GPCDAT.bit.GPIO93;//PWM_ins_indexA;//SPLL[0].Theta;//dutytmp1;//GridCurrAF;//SPLL[0].PllPiOutput;//GridCurrAF;//VoltInBF;//dbg_claWave[5];//T1PRmulESC_DutyDataBsubnegCurCompPerc>>16;//SPLL[0].Theta*100;//gridCurA_rms;//dbg_claPwmReg[4];//;//debugA_Y;//offset;
            debugW[10][CntForRec] = VoltOutCF;//ESCFlagB.ESC_DutyData;//GridBPCurrBF;//testvalue1;//DenominatorVAL;//ArithVal;//GpioDataRegs.GPFDAT.bit.GPIO164;//GridBPCurrBF;//GpioDataRegs.GPCDAT.bit.GPIO92 ;//GpioDataRegs.GPFDAT.bit.GPIO164;//GridBPCurrBF;//StateEventFlag_C;//TESTRMSGridInVAL;//EPwm8Regs.CMPA.all;//TESTVALUE;//ESC_DutyDataB;//GridCurrBF;//SPLL[1].PllPiOutput;//GridCurrBF;//VoltOutBF;//(AdccResultRegs.ADCRESULT8 - VirtulAD_loadCurrentB)*outputCurRatio;//dbg_claPwmReg[4];//EPwm8Regs.CMPA.bit.CMPA;//SPLL[1].Theta*100;//gridCurB_rms;//dbg_claPwmReg[2];//debugB_Y;
            debugW[11][CntForRec] = GridBPCurrCF;//ESCFlagC.ESC_DutyData;//GridBPCurrCF;//ESCFlagA.ESC_DutyData;//ESCFlagA.ESCCntMs.HarmDisDelay;//TESEPIerr;//TESETarget;//GpioDataRegs.GPCDAT.bit.GPIO93;//ESCFlagA.ESC_DutyData;//GridBPCurrCF;//ESCFlagB.stopFlag;//GpioDataRegs.GPEDAT.bit.GPIO153;//GridBPCurrCF;//ArithFlag;//EPwm7Regs.CMPB.all;//ArithFlag;//testvalue1;//GridCurrCF;//SPLL[2].PllPiOutput;//GridCurrCF;//(AdccResultRegs.ADCRESULT10 - VirtulAD_loadCurrentB)*outputCurRatio;//dbg_claPwmReg[5];//EPwm8Regs.CMPB.bit.CMPB;//SPLL[2].Theta*100;//idCurC_rms;//dbg_claPwmReg[3];//debugC_Y;

//            debugW[0][CntForRec]    = fftpha[0][1].x1;
//            debugW[1][CntForRec]    = fftpha[0][2].x1;
//            debugW[2][CntForRec]    = fftpha[0][3].x1;
//            debugW[3][CntForRec]    = LoadRealCurA;
//            debugW[4][CntForRec]    = CurrHarmRefA;
//            debugW[5][CntForRec]    = CurrRefA;
//            debugW[6][CntForRec]    = InvFFTData[0][1];
//            debugW[7][CntForRec]    = InvFFTData[1][1];//ADBufPos;//fftNextChan;//HarmLimtCorr;//Rec_sc;
//            debugW[8][CntForRec]    = InvFFTData[0][2];//TimeStamp;
//            debugW[9][CntForRec]    = InvFFTData[1][2];//CLAStepPhaA;
//            debugW[10][CntForRec]   = InvFFTData[0][3];//ApfOutCurA;//RefreshIFFTdata;//dcVoltUp;
//            debugW[11][CntForRec]   = InvFFTData[1][3];//ADBufPos;//ErrorRecord.all;

			break;

//        case 92:
//            debugW[0][CntForRec]    = GridVoltAF;
//            debugW[1][CntForRec]    = currentRefD;
//            debugW[2][CntForRec]    = UdcPICtrl.i6;//GridVoltCF;
//            debugW[3][CntForRec]    = UdcPICtrl.i10;//StateFlag.dcBusVtIncStart;
//            debugW[4][CntForRec]    = dcVoltDn;
//            debugW[5][CntForRec]    = dcVoltUp;
//            debugW[6][CntForRec]    = dcVoltF;
//            debugW[7][CntForRec]    = CurrRefA;//StateEventFlag;
//            debugW[8][CntForRec]    = testVolforwardA;
//            debugW[9][CntForRec]    = ApfOutCurA;
//            debugW[10][CntForRec]   = Rec_sa;
//            debugW[11][CntForRec]   = ApfOutCurC;
//
//            break;

			/*
		case 93:					//
			*pWave++ = GridVoltAF;
			*pWave++ = GridVoltBF;
			*pWave++ = GridVoltCF;
			*pWave++ = ApfOutCurA;
			*pWave++ = ApfOutCurB;
			*pWave++ = ApfOutCurC;
			*pWave++ = CurrRefA;
			*pWave++ = CurrRefB;
			*pWave++ = CurrRefC;
			*pWave++ = CtrlVoltA;

			*pWave++ = CtrlVoltB;
			*pWave++ = CtrlVoltC;
			*pWave++ = VolFundA;
			*pWave++ = VolFundB;
			*pWave++ = VolFundC;
			*pWave++ = PwmVa;
			*pWave++ = PwmVb;
			*pWave++ = PwmVc;
			*pWave++ = ThreeResSin;//spwmFlag;
			*pWave   = dcBusVtIncStart;
			break;
		case 94:					//
			*pWave++ = GridVoltAF;
			*pWave++ = GridVoltBF;
			*pWave++ = GridVoltCF;
			*pWave++ = ApfOutCurA;
			*pWave++ = ApfOutCurB;
			*pWave++ = ApfOutCurC;
			*pWave++ = CurrRefA;
			*pWave++ = CurrRefB;
			*pWave++ = CurrRefC;
			*pWave++ = PllPiOutput;

			*pWave++ = Theta;
			*pWave++ = VoltTheta;
			*pWave++ = PLLResSin;
			*pWave++ = PLLResCos;
			*pWave++ = gridFreq;
			*pWave++ = PrvFreq;
			*pWave++ = capTsctr;
			*pWave++ = cntForSyncOut;
			*pWave++ = syncState;
			*pWave   = syncErr;
			break;
		case 95:					//
			*pWave++ = GridVoltAF;
			*pWave++ = GridVoltBF;
			*pWave++ = GridVoltCF;
			*pWave++ = capTsctr;
			*pWave++ = cntForSyncOut;
			*pWave++ = syncState;
			*pWave++ = syncErr;
			*pWave++ = pwmTsGiven;
			*pWave++ = syncErrLimit;
			*pWave++ = LED_SYNC;

			*pWave++ = EPwm1Regs.TBPRD ;
			*pWave++ = cntForSyncErr;
			*pWave++ = SyncHardwareLead;
			*pWave++ = T1PR;
			*pWave++ = MAX_CAP1;
			*pWave++ = MU_ALLCOUNT;
			*pWave++ = OUT_SYNC_SINGLE;
			*pWave++ = syncErrPrior;
			*pWave++ = syncErr;
			*pWave   = cntSyncSucc;
			break;
		case 96:					//
			*pWave++ = GridVoltAF;
			*pWave++ = GridVoltBF;
			*pWave++ = GridVoltCF;
			*pWave++ = VoltSlidA[VoltPos];
			*pWave++ = VoltSlidB[VoltPos];
			*pWave++ = VoltSlidC[VoltPos];
			*pWave++ = FundwaveA;
			*pWave++ = FundwaveB;
			*pWave++ = FundwaveC;
			*pWave++ = CurrHarmRefA;

			*pWave++ = CurrHarmRefB;
			*pWave++ = CurrHarmRefC;
			*pWave++ = HarmSumA;
			*pWave++ = HarmSumB;
			*pWave++ = HarmSumC;
			*pWave++ = dcVoltUpF;
			*pWave++ = dcVoltDnF;
			*pWave++ = HarmLimtCorr;
			*pWave++ = CapaUnbalCorr;
			*pWave   = CurrRefA;
			break;
		case 97:					//
			*pWave++ = GridVoltAF;
			*pWave++ = VoltSlidA[VoltPos];
			*pWave++ = PllPiOutput;
			*pWave++ = currentRefD;
			*pWave++ = CurrRefA;
			*pWave++ = CurrRefB;
			*pWave++ = CurrRefC;
			*pWave++ = ApfOutCurA;
			*pWave++ = ApfOutCurB;
			*pWave++ = ApfOutCurC;

			*pWave++ = CurrHarmRefA;
			*pWave++ = dcVoltUp;
			*pWave++ = dcVoltDn;
			*pWave++ = PwmVa;
			*pWave++ = PwmVb;
			*pWave++ = PwmVc;
			*pWave++ = StateEventFlag;
			*pWave++ = GpioDataRegs.GPADAT.bit.GPIO31;	//直流硬件过压
			*pWave++ = GpioDataRegs.GPADAT.bit.GPIO10;	//IGBT_FAULT_ACK1
			*pWave   = GpioDataRegs.GPADAT.bit.GPIO11;	//IGBTEN
			break;
		case 98:
			*pWave++ = GridVoltAF;
			*pWave++ = GridVoltBF;
			*pWave++ = GridVoltCF;
			*pWave++ = *pGridVolt1;
			*pWave++ = *pGridVolt2;
			*pWave++ = *pGridVolt3;
			*pWave++ = VolforwardA;
			*pWave++ = VolforwardB;
			*pWave++ = UdcBalanceCurr;
			*pWave++ = currentRefD;

			*pWave++ = ApfOutCurA;
			*pWave++ = ApfOutCurB;
			*pWave++ = PwmVa;
			*pWave++ = PwmVb;
			*pWave++ = PwmVc;
			*pWave++ = GpioDataRegs.GPADAT.bit.GPIO31<<3 | GpioDataRegs.GPADAT.bit.GPIO10<<2 | GpioDataRegs.GPADAT.bit.GPIO12<<1 | GpioDataRegs.GPADAT.bit.GPIO13;
			*pWave++ = dcVoltUpF;
			*pWave++ = dcVoltDnF;
			*pWave++ = dcVoltUp;
			*pWave   = dcVoltDn;
			break;
			*/
//		default: StartDmaFlag=0;break;
		}
       	if(++CntForRec >= DEBUG_WAVE_LEN)
       		CntForRec = 0;
  //     	if(StartDmaFlag)
  //     		StartDMACH2();
    }
}
/*
//}
		case 1:
			*pWave++ = GridVoltAF;
			*pWave++ = ApfOutCurA;
			*pWave++ = ApfOutCurB;
			*pWave++ = ApfOutCurC;
			*pWave++ = PwmVa;
			*pWave++ = PwmVb;
			*pWave++ = PwmVc;
			*pWave++ = CurrRefA;
			*pWave++ = CurrRefB;
			*pWave   = CurrRefC;
			break;
		case 2:
			*pWave++ = GridVoltAF;
			*pWave++ = PLLResSin;//CurrRefA;
			*pWave++ = ApfOutCurA;
			*pWave++ = LoadRealCurA;//gpVoltA_rms;
			*pWave++ = GridActPower;//GridCurrA;
			*pWave++ = GridReactPower;//LoadRealCurA;
			*pWave++ = GridPLLVoltD;//loadCurA_rms;
			*pWave++ = GridPLLVoltQ;//LoadRealCurB;
			*pWave++ = GridFundaCurD;//gridCurA_rms;
			*pWave   = GridFundaCurQ;//apfOutCurA_rms;
			break;
		case 3:
			*pWave++ = GridVoltAF;
			*pWave++ = FundwaveA;
			*pWave++ = FundwaveB;
			*pWave++ = FundwaveC;
			*pWave++ = CurrHarmRefA;
			*pWave++ = CurrHarmRefB;
			*pWave++ = CurrHarmRefC;
			*pWave++ = HarmSumA;
			*pWave++ = HarmSumB;
			*pWave   = HarmSumC;
			break;
		case 4:
			*pWave++ = GridVoltAF;
			*pWave++ = GridVoltBF;
			*pWave++ = GridVoltCF;
			*pWave++ = VoltSlidA[VoltPos];
			*pWave++ = VoltSlidB[VoltPos];
			*pWave++ = VoltSlidC[VoltPos];
			*pWave++ = dcVoltUpF;
			*pWave++ = dcVoltDnF;
			*pWave++ = gridFreq;
			*pWave   = PllPiOutput;
			break;
		case 5:
			*pWave++ = GridVoltAF;
			*pWave++ = negCurCompD;
			*pWave++ = negCurCompQ;
			*pWave++ = LoadFundaCurZ;
			*pWave++ = currentRefD;
			*pWave++ = currentRefQ;
			*pWave++ = InvFundaCurD;
			*pWave++ = InvFundaCurQ;
			*pWave++ = InvFundaCurND;
			*pWave   = InvFundaCurNQ;
			break;
		case 9:
			StepPhaA+=PI2/(6400/50);
			PhaseLimitF(StepPhaA);
			StepPhaB+=PI2/(6400/(5*50));
			PhaseLimitF(StepPhaB);

			xPhaseTmp=PhaseLimitI( (StepPhaA )*(PI2_SINE_LOOKTABLE/PI2) );
			AF= pSineLookTab(xPhaseTmp) ;	//matrix realA*realB-imagA*imagB
			xPhaseTmp=PhaseLimitI( (StepPhaB )*(PI2_SINE_LOOKTABLE/PI2) );
			BF= pSineLookTab(xPhaseTmp) ;

			*pWave++ = AF*10;
			*pWave++ = BF*10;
			*pWave++ = AF*100;
			*pWave++ = BF*100;
			*pWave++ = AF*1000;

			*pWave++ = BF*10000;
			*pWave++ = CntForRec;
			*pWave++ = AF*100;
			*pWave++ = AF*100+BF*60;
			*pWave++ = (AF+BF)*100;

			*pWave++ = (AF-BF*0.5)*100;
			break;
		default: StartDmaFlag=0;
		}
       	if(++CntForRec >= DEBUG_WAVE_LEN)
       		CntForRec = 0;
       	if(StartDmaFlag)
       		StartDMACH2();
    }
} }
//}
 */
// ---------------------- NO MORE --------------------------- //

