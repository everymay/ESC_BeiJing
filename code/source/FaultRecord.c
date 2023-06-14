#include "F2837xS_device.h"
#include "F2837xS_Examples.h"
#include "VariblesRef.h"
#include "MarcoDefine.h"
#include "DCL.h"
#include "DPLL.h"

// ���ܽ��ͣ�
/* ���Լ�¼����ͣ�����߹���ͣ���������
 * ʵ�ַ�����
 * 1���ⲿRAM���ʮ�����ݣ�ÿ������2000�����Ρ�
 * 2��ͨ��Э��Ϊ���ӻ���ַ��0x01�����ݵ�ַͷ��0x3001���洢��ַ�Σ�0x0000���������ݣ�
 * 3���洢λ�ã�
 */
#pragma	CODE_SECTION(FaultRecordProg ,"ram2func")
//#pragma DATA_SECTION(FaultRecSel,"EBSS3");
struct Stru_FaultRecSel FaultRecSel;
const int FaultRecSelTab[4][8]={{0,1,2,3,6,12,24,48},{0,1,2,3,6,12,24,48},{0,1,2,4,8,16,32,64},{0,1,2,5,10,20,40,80}};		//8k,9.6k,12.8k,16k
const int FFTRecSelTab[4][10]={{1,1,1,1},{1,1,0,1,1,0},{1,0,1,0,1,0,1,0},{1,0,0,1,0,0,1,0,0,1}};		//8k,9.6k,12.8k,16k



void SetWaveRecordMode(Uint16 mode)   //��¼���ϲ��ε�ģʽ
{
	FaultRecSel.RecordChan=FaultRecSel.RecordChanOrg = mode     % 100;	//MDSPP:PP:ѡ��ͬ����¼��
	Uint16 SampRate 			=(mode%10000)/100;						//S:����۲�����
	if(SampRate>=7) SampRate =7;
	FaultRecSel.stopRecordMode	= mode/10000;							//MDSPP:M:1:һֱ¼��.�����Ƿ�ͣ��.0:ͣ������Ϻ��Զ�ֹͣ¼��.�������2000����Ĳ�������
	//0: 1���������� (���ݴӻ��Զ������������16k,19.2k,25.6k,32k)	1: 1/2��(8k,9.6k,12.8k,16k)	2: 6.4k	3: 3.2k	4: 1.6k	5: 0.8k	6: 0.4k	7: 0.2k
	//����FFTģʽ�µ�¼��ͨ��.���ұ�֤FFT��������ADBufPos�ϸ��Ӧ,��֤�����������
	FaultRecSel.RecordCntSet = FaultRecSelTab[ADGROUP_MODE][SampRate];	//�������
	FlashApiDataCopy(SampRate);
}

// ���ܽ��ͣ�
// ���Ϸ���ǰ,һֱ¼��.���Ϸ������ʱ���ټ�¼0.5���ܲ�������,�ϴ���LCD
//RecordMode:¼��ͨ��ѡ��: 0:ֹͣ¼��.1~30:ǰ�汾��30�ֲ�ͬͨ����ϵ�¼����ʽ,¼��ͨ��Ϊ10��.31~60:�°汾��30�ֲ�ͬͨ����ϵ�¼����ʽ,¼��ͨ��Ϊ20��
//           98:����װ���Ƿ��й���,һֱ¼��.99:װ�ù��Ϻ�ֹͣ¼��
//RefreshMode:ˢ�·�ʽ 00:ά����һ��״̬               01:ֹͣˢ�»�����
//                   02:һֱˢ�»�����               03:ˢ��һ�λ�������ֹͣ.
//DispSampRate:������00:FFT�ò�������(�̶�Ϊ6.4k)   02:1/2��(8k,9.6k,12.8k,16k)
//01:1���������� (���ݴӻ��Զ������������16k,19.2k,25.6k,32k)
//03:1/4��(4k,4.8k,6.4k,8k)       04:1/8��(1k,2.4k,3.2k,4k)
int TestChan = 0,TESTPhase=0;

void FaultRecordProg(void)
{
//	float *pWave=DebugInner;
//	int StartDmaFlag=1;
//	int tmp;
	struct Stru_FaultRecSel *pSel=&FaultRecSel;
	//ʣ���¼������>=0 && ( ��fftģʽ && �̶������ݼ��¼����������  ||  fftģʽ && (FFT����1 || FFT����2))
	if(	 (pSel->DelayRec >= 0)&&\
		(	((!pSel->isFFTMode)&&(++pSel->RecordCnt > pSel->RecordCntSet))||\
			(( pSel->isFFTMode)&&(FFTRecSelTab[ADGROUP_MODE][ADBufPos]) ))){          //�������
		pSel->DelayRec--;	//¼��һ����,��������һ
		pSel->RecordCnt=0;

		switch(pSel->RecordChan){				//debugW
//		case 0:StartDmaFlag=0;		//û�й���¼��
//		break;

		case 1:						//FFTģʽ
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

		case 91:					//�ۺ���λ
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

			break;
		}
       	if(++CntForRec >= DEBUG_WAVE_LEN)
       		CntForRec = 0;
    }
}

