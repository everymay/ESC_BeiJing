// ========================================================================
//  This software is licensed for use with Texas Instruments C28x
//  family DSCs.  This license was provided to you prior to installing
//  the software.  You may review this license by consulting a copy of
//  the agreement in the doc directory of this library.
// ------------------------------------------------------------------------
//          Copyright (C) 2014-2015 Texas Instruments, Incorporated.
//                          All Rights Reserved.
// ========================================================================
//
//
//  FILE:    Test_FPU_RFFTF32.c
//
//  TITLE:   DSP2833x Device FIR Test Program.   
//
//
//  DESCRIPTION:
//
//    This program shows how to compute a real FFT and associated spectrum 
//    magnitude, phase.  The input buffer must be aligned to a multiple of the 
//    FFT size if using RFFT_f32.  If you do not wish to align the input buffer
//    then use the RFFT_f32u function. In this case, the section alignment #pragma
//    can be commented.  However, using this function will reduce cycle performance 
//    of the algorithm.
//
//  FUNCTIONS:
//
//    void RFFT_f32 (FFT_REAL *)
//    void RFFT_f32_mag (FFT_REAL *)
//    void RFFT_f32_phase (FFT_REAL *)
//
//    Where RFFT_F32_STRUCT is a structure defined as:
//
//    typedef struct {
//      float  *InBuf;
//      float  *OutBuf;
//      float  *CosSinBuf;
//      float  *MagBuf;
//      float  *PhaseBuf;
//      uint16_t FFTSize;
//      uint16_t FFTStages;
//    } RFFT_F32_STRUCT;
//
//  ASSUMPTIONS:
//
//     * FFTSize must be a power of 2 (32, 64, 128, etc)
//     * FFTSize must be greater or equal to 32
//     * FFTStages must be log2(FFTSize)
//     * InBuf, OutBuf, CosSinBuf are FFTSize in length
//     * MagBuf and PhaseBuf are FFTSize/2+1 in length
//     * MagBuf and PhaseBuf are not used by this function.
//       They are only used by the magitude and phase calculation functions.
//
//  Watch Variables:
//
//      InBuf               Input buffer
//      OutBuf				Output buffer
//      CosSinBuf        	Twiddle factor buffer
//      MagBuf       		Magnitude buffer
//      PhaseBuf            Phase buffer
//
//--------------------------------------------------------------------------
//  Input buffer structure:
//   	InBuf[0] = input[0]
//   	InBuf[1] = input[1]
//   	InBuf[2] = input[2]
//   	InBuf[N/2] = input[N/2]
//   	InBuf[N/2+1] = input[N/2+1]
//   	InBuf[N-3] = input[N-3]
//   	InBuf[N-2] = input[N-2]
//   	InBuf[N-1] = input[N-1]
//
//
//###########################################################################
// $TI Release: C28x Floating Point Unit Library V1.40.00.00 $
// $Release Date: Mar 10, 2014 $
//###########################################################################

#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "CoreCtrl.h"
#include "VariblesRef.h"
#include "stdio.h"
#include "F2837xS_device.h"
#include "math.h"
#include "C28x_FPU_FastRTS.h"
#include "float.h"
#include "fpu_rfft.h"
#include "DCL.h"
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Swi.h>
#include "stdlib.h"
extern const Swi_Handle FFTstart;

//#pragma DATA_SECTION(RFFTin1Buff,"RFFTin");  	//Buffer alignment for the input array,
//#pragma DATA_SECTION(RFFToutBuff,"EBSS1");
//#pragma DATA_SECTION(RFFTF32Coef,"EBSS1");
#pragma DATA_SECTION(GenCorrPrv, "WAVEDATA2")
//#pragma DATA_SECTION(IFFTData, "CLADataLS01")
//#pragma DATA_SECTION(CurrHarmRefA, "CLADataLS01")
//#pragma DATA_SECTION(CurrHarmRefB, "CLADataLS01")
//#pragma DATA_SECTION(CurrHarmRefC, "CLADataLS01")
//#pragma DATA_SECTION(CalibCorr, "CLADataLS01")

//#pragma DATA_SECTION(rfft,"EBSS1");
//#pragma DATA_SECTION(IFFTData,"EBSS1");
//#pragma DATA_SECTION(LimitVal,"EBSS1");
//#pragma DATA_SECTION(LimitDly,"EBSS1");
//#pragma DATA_SECTION(GenCorr,"EBSS1");
//float RFFTin1Buff[RFFT_SIZE];                 	//RFFT_f32u(optional), RFFT_f32(required)
//float RFFToutBuff[RFFT_SIZE];                 	//Output of FFT here if RFFT_STAGES is EVEN
//float RFFTF32Coef[RFFT_SIZE];                 	//Twiddle buffer
const GENERATOR_COEFF_STRUCT GenCorrPrv[GENERATOR_COEFF_LEN]=GENERATOR_COEFF_DEFAULTS_ALL;
//const CALIBRATION_COEFF_STRUCT CalibCorrPrv=COMPENSATE_COEFF_DEFAULTS;
//RFFT_F32_STRUCT 			rfft;
CALIBRATION_COEFF_STRUCT CalibCorr;	//Need to put in the BSS segment. Otherwise, do not initialize
GENERATOR_COEFF_STRUCT GenCorr=GENERATOR_COEFF_DEFAULTS;		//Need to put in the BSS segment. Otherwise, do not initialize
float IFFTData[(3+1)*2][CALIBRATION_COEFF_LEN+1];       //共计3+1个通道.1表示正在更新的通道,ifft异步算法不能同时用此通道.
float InvFFTData[3*2][CALIBRATION_COEFF_LEN+1];
float TestLoadCur[3][GEN_FUNDPOINT];
Uint16 TestADTempBufp;
int16 CLAFFTSrcBufp;
Uint16  LimitDly[3+1][CALIBRATION_COEFF_LEN];
float32 LimitVal[3+1][CALIBRATION_COEFF_LEN];
//float FFTPIin[2][CALIBRATION_COEFF_LEN],FFTPIout[2][CALIBRATION_COEFF_LEN];

//#pragma CODE_SECTION(IFFTCalculate, "ram2func");
//#pragma CODE_SECTION(FFTSWI, "ram2func");
#pragma CODE_SECTION(FFTDataReduction, "ram2func");
#pragma CODE_SECTION(FFTDataReductionInv, "ram2func");
//#pragma CODE_SECTION(SequenceAutoFFT, "ram2func");



//Uint16 TestIFFTBufp=0;
//float32 StepPhaA=0,StepPhaB=0,StepPhaC=0;
float32 HarmSum,HarmSumA,HarmSumB,HarmSumC;
//#pragma DATA_SECTION(CLAStepPhaA, "CLADataLS01")
//#pragma DATA_SECTION(CLAStepPhaB, "CLADataLS01")
//#pragma DATA_SECTION(CLAStepPhaC, "CLADataLS01")
//#pragma DATA_SECTION(FundwaveA, "CLADataLS01")
//#pragma DATA_SECTION(FundwaveB, "CLADataLS01")
//#pragma DATA_SECTION(FundwaveC, "CLADataLS01")
//#pragma DATA_SECTION(TimeStamp, "CLADataLS01")
//#pragma DATA_SECTION(FFTtimeStamp, "CLADataLS01")
//#pragma DATA_SECTION(mpMagA, "CLADataLS01")
//#pragma DATA_SECTION(mpMagB, "CLADataLS01")
//#pragma DATA_SECTION(mpMagC, "CLADataLS01")
//#pragma DATA_SECTION(mpPhaA, "CLADataLS01")
//#pragma DATA_SECTION(mpPhaB, "CLADataLS01")
//#pragma DATA_SECTION(mpPhaC, "CLADataLS01")
//#pragma DATA_SECTION(CLAwave, "CLADataLS01")
//#pragma DATA_SECTION(fftUsedChanA, "CLADataLS01")
//#pragma DATA_SECTION(fftUsedChanB, "CLADataLS01")
//#pragma DATA_SECTION(fftUsedChanC, "CLADataLS01")
//#pragma DATA_SECTION(fftNextChan, "CLADataLS01")
//
//#pragma DATA_SECTION(fftChan, "CLADataLS01")
//#pragma DATA_SECTION(RefreshIFFTdata, "CLADataLS01")
//#pragma DATA_SECTION(FFTCalcChan, "CLADataLS01")
//
////float32 CLACurrHarmRef[3];
float32 CurrHarmRefA=0,CurrHarmRefB=0,CurrHarmRefC=0;
//float32 CLAStepPhaA=0,CLAStepPhaB=0,CLAStepPhaC=0;
float32 FundwaveA=0,FundwaveB=0,FundwaveC=0,CLAwave[6];
float TimeStamp = 0;
float FFTtimeStamp = 0;
unsigned long fftUsedChanA=0,fftUsedChanB=2,fftUsedChanC=4,fftNextChan=6;
//float32 *mpMagA,*mpMagB,*mpMagC,*mpPhaA,*mpPhaB,*mpPhaC;
//unsigned long offsetphase;
//unsigned long fftUseChan,fftChan;
unsigned long RefreshIFFTdata;
unsigned long FFTCalcChan=0;
void FFTInit(void)
{
	//Initialization timer2, because the FFT computing time is longer, to prevent the DMA interrupt
	//after the SCI, SPI and other low priority interrupt can not respond
//	ConfigCpuTimer(&CpuTimer2, 150, 0.7);		//Set a relatively short time, so once you start the timer, enter
//	ConfigCpuTimer(&CpuTimer1, 150, 0.7);		//Set a relatively short time, so once you start the timer, enter

/*    rfft.FFTSize   = RFFT_SIZE;
    rfft.FFTStages = RFFT_STAGES;   
    rfft.InBuf     = &RFFTin1Buff[0];  //Input buffer
    rfft.OutBuf    = &RFFToutBuff[0];  //Output buffer
    rfft.CosSinBuf = &RFFTF32Coef[0];  //Twiddle factor buffer
//  rfft.MagBuf    = &RFFTmagBuff[0];  //Magnitude buffer

    RFFT_f32_sincostable(&rfft);       //Calculate twiddle factor*/
}

void SequenceAutoFFT(int chan)
{
	switch(chan){
	case 0:
	    if(StateFlag.positionCT)					FFTDataReduction2(1,FFTCalcChan>>1);
		else if(StateFlag.DelayCalculationFlag)		FFTDataReduction2(4,FFTCalcChan>>1);
		break;

	case 1:
        if(StateFlag.positionCT)					FFTDataReduction2(1,FFTCalcChan>>1);
        else if(StateFlag.DelayCalculationFlag)		FFTDataReduction2(4,FFTCalcChan>>1);
		break;

	case 2:
        if(StateFlag.positionCT)					FFTDataReduction2(1,FFTCalcChan>>1);
        else if(StateFlag.DelayCalculationFlag)		FFTDataReduction2(4,FFTCalcChan>>1);
		break;

	}
}

//    float32 *pDstMag=&IFFTData[fftNextChan][0],*pDstPha=&IFFTData[fftNextChan+1][0],Re,Im;    //即将更新的通道
//

//Copy the FFT data to the IFFT of the source data area, the other time IFFT to maintain the data, only the phase change with time
void FFTDataReduction(Uint16 chan){     //4.56us
/*    float32 Buf[2][CALIBRATION_COEFF_LEN];
    float32 *pCMag,*pCPha,*pLimVal,harmSum=0,squre,pha,mag,Re,Im;
    float32 *pSrcEnd=rfft.OutBuf+RFFT_SIZE;
    float32 *pSrc   =rfft.OutBuf;
    Uint32  *pCOrd=CalibCorr.harmOrder;
    float32 *pCLimit=CalibCorr.limit;
    float32 *pCStart=CalibCorr.start;
    float32 *pMagBuf = Buf[0],*pPhaBuf = Buf[1];
    Uint16  *pLimDly;
    Uint16  ord;
    Uint16  i;

    switch(chan){
    case 0: //A Channel
        pLimVal=&LimitVal[0][0];        //限制值非同步更新,问题不大.故不采用滚动缓冲区策略
        pLimDly=&LimitDly[0][0];
        pCMag=CalibCorr.MagA;
        pCPha=CalibCorr.PhaA;
        break;
    case 1: //B
        pLimVal=&LimitVal[1][0];
        pLimDly=&LimitDly[1][0];
        pCMag=CalibCorr.MagB;
        pCPha=CalibCorr.PhaB;
        break;
    case 2: //C
        pLimVal=&LimitVal[2][0];
        pLimDly=&LimitDly[2][0];
        pCMag=CalibCorr.MagC;
        pCPha=CalibCorr.PhaC;
       break;
    default:
        pLimVal=&LimitVal[0][0];
        pLimDly=&LimitDly[0][0];
        pCMag=CalibCorr.MagA;
        break;
    }

    //To calculate the fundamental power
    Re= MAGCORR(*(pSrc    + 1));  //
    Im= MAGCORR(*(pSrcEnd - 1));  //
    squre = sqrtf(Re * Re + Im * Im);                                   //得到幅度
    pha=PhaseLimit(atan2(Im,Re) - FFTtimeStamp + PI100) - PI;           //得到相位,100pi是让PhaseLimit函数>0.然后映射到-pi~pi区间
//  *pDstMag++ = squre;
//  *pDstPha++ = pha;
    *pMagBuf++ = squre;
    *pPhaBuf++ = pha;

    //To calculate the Harmnoic Current5
    for(i=0;i<CALIBRATION_COEFF_LEN;i++){
        ord=(Uint16)(*pCOrd++);
        Re= MAGCORR(*(pSrc    + ord));  //
        Im= MAGCORR(*(pSrcEnd - ord));  //
        squre = sqrtf(Re * Re + Im * Im) * *pCMag++;                    //得到幅度
        pha=PhaseLimit(atan2(Im,Re) - FFTtimeStamp *ord + PI100 + *pCPha++ ) - PI;    //得到相位,100pi是让PhaseLimit函数>0.然后映射到-pi~pi区间

        if(*(pLimDly+i)){                       //Once the limit is started, it will last for some time.
            mag*= *(pLimVal+i) * squre;         //得到限幅后幅值
            (*(pLimDly+i))--;                   //自动限制值限制计数器--
            harmSum += mag;                     //得到限幅后的各次谐波的幅值.limit magtiute After calu Squre
        }else{
            if(squre > *(pCLimit + i)){         //signal harmnoic limit magtiute
                *(pLimVal+i)=(*(pCLimit + i) )/ (squre);    //得到幅度缩小比例
                mag*= *(pLimVal+i) * squre;     //得到幅值
                *(pLimDly+i)=43;                //自动限制值计数器赋初值,3.2k/3/43=每次调用A相的间隔/每次限制时间为43点=0.0403125ms
                harmSum += mag;                 //谐波和
            }else if(squre < *(pCStart + i)){
                mag=0;
            }else{
                mag=squre;
                harmSum+=squre;
            }
        }
//      *pMagIn++ = mag;
//      *pPhaIn++ = pha;
//      *pDstMag++ = DCL_runDF22(&fftmag[chan][i],mag);     //HarmLimtCorr;
//      *pDstPha++ = DCL_runDF22(&fftpha[chan][i],pha);     //
        *pMagBuf++ = mag;
        *pPhaBuf++ = pha;
    }
//    for(i=0;i<CALIBRATION_COEFF_LEN;i++){
//        IFFTData[fftNextChan  ][i] = fftmag[chan][i].x1 = Buf[0][i];
//        IFFTData[fftNextChan+1][i] = fftpha[chan][i].x1 = Buf[1][i]+PI;//然后映射到0~2pi区间
//    }
    DCL_runDF22Group(fftmag[chan],Buf[0],&IFFTData[fftNextChan  ][0],CALIBRATION_COEFF_LEN);     //
    DCL_runDF22Group(fftpha[chan],Buf[1],&IFFTData[fftNextChan+1][0],CALIBRATION_COEFF_LEN);     //

    switch(chan){
    case 0: HarmSumA=sqrtf(harmSum);
    //    CLAStepPhaA = 0;
        break;
    case 1: HarmSumB=sqrtf(harmSum);
    //    CLAStepPhaB = 0;
        break;
    case 2: HarmSumC=sqrtf(harmSum);
    //    CLAStepPhaC = 0;
        break;
    default:
        HarmSumA =0.0f;HarmSumB =0.0f;HarmSumC =0.0f;
        break;
    }

    RefreshIFFTdata=chan+1;
//    xrand=( 30*(1.0f/500))*(TestLoadCurS(2)+TestLoadCurS(2)+TestLoadCurS(2));
 * *
 */
}

//逆变侧电流的FFT处理
void FFTDataReductionInv(Uint16 Chan){     //4.56us
//    float32 pha,mag;
/*    float32 *pSrc   = rfft.OutBuf;
    float32 *pSrcEnd= rfft.OutBuf + RFFT_SIZE;
    Uint32  *pCOrd = CalibCorr.harmOrder;
    float32 *pDstMag,*pDstPha,Re,Im;
    Uint16  ord;
    Uint16 i;

    switch(Chan){
    case 0: //D,逆变侧谐波校正算法
        pDstMag=&InvFFTData[0][0];
        pDstPha=&InvFFTData[1][0];
        break;
    case 1: //E,逆变侧谐波校正算法
        pDstMag=&InvFFTData[2][0];
        pDstPha=&InvFFTData[3][0];
        break;
    case 2: //F,逆变侧谐波校正算法
        pDstMag=&InvFFTData[4][0];
        pDstPha=&InvFFTData[5][0];
        break;
    default:
        pDstMag=&IFFTData[0][0];
        pDstPha=&IFFTData[1][0];
        break;
    }
    //To calculate the fundamental power
    Re= MAGCORR(*(pSrc    + 1));  //
    Im= MAGCORR(*(pSrcEnd - 1));  //
    *pDstMag++ = sqrtf(Re * Re + Im * Im);
    *pDstPha++ = PhaseLimit(atan2(Im,Re) - FFTtimeStamp + PI100) - PI;

    //To calculate the Harmnoic Current
    for(i=0;i<CALIBRATION_COEFF_LEN;i++){   //1~40次(次数高了也无意义,相位误差太大)的*谐波均纳入到逆变侧电流的监控范围,暂时选择与补偿的谐波相同次数
        ord = *pCOrd++;                     //谐波,选择是奇次还是偶次还是所有谐波
        Re= MAGCORR(*(pSrc    + ord));      //
        Im= MAGCORR(*(pSrcEnd - ord));      //
        *pDstMag++  = sqrtf(Re * Re + Im * Im);          //得到幅度
        *pDstPha++  = PhaseLimit(atan2(Im,Re) - FFTtimeStamp*ord + PI100) - PI; //得到相位,100pi是让PhaseLimit函数>0.然后映射到-pi~pi区间
    }
//    DCL_runPI_Group(FFTPICtrl[0],\
//                    FFTPIin[0],\
//                    FFTPIout[0],CALIBRATION_COEFF_LEN*2);
*/
}

void FFTDataReduction2(Uint16 harm,Uint16 chan){
/*	float32 Re,Im;
	float32 *pSrc	=rfft.OutBuf;
	float32 *pSrcEnd=rfft.OutBuf+RFFT_SIZE;
	Re= MAGCORR(*(pSrc    + harm));
	Im= MAGCORR(*(pSrcEnd - harm));
	VoltTheta2=GridVoltTheta+(PI/2);				//超前90
	if(VoltTheta2 > PI2)	VoltTheta2 -= PI2;
	if(VoltTheta2 < 0)		VoltTheta2 += PI2;
	switch(chan){
	case 0:
		if(StateFlag.CurrACountFlag&&(VoltTheta2<(0.02*harm))&&(VoltTheta2>(-0.02*harm))) 		//负载侧电压正负1°,电网侧电压正负4°,4次谐波正负16°
		{
			StateFlag.CurrACountFlag=0;
			SeqJud[chan]=sqrtf(Re*Re+Im*Im);
			DirJud[chan] = atan2(Im,Re)*(180/PI)+180;
			GfDebug1 = VoltTheta2*1000;
		}
	break;
	case 1:
		if(StateFlag.CurrBCountFlag&&(VoltTheta2<(0.02*harm))&&(VoltTheta2>(-0.02*harm)))
		{
			StateFlag.CurrBCountFlag=0;
			SeqJud[chan]=sqrtf(Re*Re+Im*Im);
			DirJud[chan] = atan2(Im,Re)*(180/PI)+180;
			GfDebug2 = VoltTheta2*1000;
		}
	break;
	case 2:
		if(StateFlag.CurrCCountFlag&&(VoltTheta2<(0.02*harm))&&(VoltTheta2>(-0.02*harm)))
		{
			StateFlag.CurrCCountFlag=0;
			SeqJud[chan]=sqrtf(Re*Re+Im*Im);
			DirJud[chan] = atan2(Im,Re)*(180/PI)+180;
			GfDebug3 = VoltTheta2*1000;
		}
	break;
	}*/
}

int TEST_CHAN =0;
void FFTSWI(void)     //
{
//	SET_DBG_IO3(1);
//    RFFT_f32u(&rfft);				   //暂时用不对齐的FFT,速度稍微慢,对齐的FFT不能用,应该是段地址分配之类的问题

    switch(FFTCalcChan){
    case 0:case 2: case 4:
//        FFTDataReduction(FFTCalcChan>>1);
//        if(StateFlag.SequenceAutoFlag)
//            SequenceAutoFFT(FFTCalcChan>>1);   //自动寻找CT的方向
        break;
    case 1:case 3: case 5:
//        FFTDataReductionInv(FFTCalcChan>>1);
        break;

    }
    if(++FFTCalcChan >= 6) FFTCalcChan=0;
//	SET_DBG_IO3(0);
}

// INT7.1
void DMA_FFTBufferAlignINT(void)
{
	if(SecondReadLen != 0){
//		SecondDMACH1ConfigAndRun();
	}else{
//	    Swi_post(FFTstart);
	}
//	PieCtrlRegs.PIEACK.all |= PIEACK_GROUP7;
}

//===========================================================================
// Function: void RFFT_f32_sincostable(RFFT_F32_STRUCT *fft)
//===========================================================================
//
// Sin/Cos table contains the following entries:
//
//     +-------------------+
//   0 | COS( 1*2*PI/8)    | Stages 1,2,3
//   1 | SIN( 1*2*PI/8)    |
//   2 |        0.0        |
//   3 |        1.0        |
//     +-------------------+
//   4 | COS( 1*2*PI/16)   |
//   5 | SIN( 1*2*PI/16)   |
//   6 | COS( 2*2*PI/16)   | Stage 4
//   7 | SIN( 2*2*PI/16)   |
//   8 | COS( 3*2*PI/16)   |
//   9 | SIN( 3*2*PI/16)   |
//  10 |        0.0        |
//  11 |        1.0        |
//     +-------------------+
//  12 | COS( 1*2*PI/32)   |
//  13 | SIN( 1*2*PI/32)   |
//  14 | COS( 2*2*PI/32)   |
//  15 | SIN( 2*2*PI/32)   |
//  16 | COS( 3*2*PI/32)   |
//  17 | SIN( 3*2*PI/32)   |
//  18 | COS( 4*2*PI/32)   | Stage 5
//  19 | SIN( 4*2*PI/32)   |
//  20 | COS( 5*2*PI/32)   |
//  21 | SIN( 5*2*PI/32)   |
//  22 | COS( 6*2*PI/32)   |
//  23 | SIN( 6*2*PI/32)   |
//  24 | COS( 7*2*PI/32)   |
//  25 | SIN( 7*2*PI/32)   |
//  26 |        0.0        |
//  27 |        1.0        |
//     +-------------------+
//  28 . COS( N*2*PI/64)   . Stage 6  (N = 1 to 15)
//     . SIN( N*2*PI/64)   .
//     |        0.0        |
//     |        1.0        |
//     +-------------------+
//     . COS( N*2*PI/128)  . Stage 7  (N = 1 to 31)
//     . SIN( N*2*PI/128)  .
//     |        0.0        |
//     |        1.0        |
//     +-------------------+
//     . COS( N*2*PI/256)  . Stage 8  (N = 1 to 63)
//     . SIN( N*2*PI/256)  .
//     |        0.0        |
//     |        1.0        |
//     +-------------------+
//     . COS( N*2*PI/512)  . Stage 9  (N = 1 to 127)
//     . SIN( N*2*PI/512)  .
//     |        0.0        |
//     |        1.0        |
//     +-------------------+
//     . COS( N*2*PI/1024) . Stage 10 (N = 1 to 255)
//     . SIN( N*2*PI/1024) .
//     |        0.0        |
//     |        1.0        |
//     +-------------------+
//
// Note: Table size for 1K FFT Real = 990 entries
//---------------------------------------------------------------------------

void RFFT_f32_sincostable(RFFT_F32_STRUCT *fft)
{
    float  tempPI, temp;
    uint16_t i, j, k, l, N;

    tempPI = 0.7853981633975;   // pi/4
    k = 1;
    l = 0;
    for(i = 3; i <= fft->FFTStages; i++)
    {
        N = 1;
        for(j=1; j <= k; j ++)
        {
            temp = (float)N * tempPI;
            fft->CosSinBuf[l++] = cos(temp);
            fft->CosSinBuf[l++] = sin(temp);
            N++;
        }
        fft->CosSinBuf[l++] = 0.0;
        fft->CosSinBuf[l++] = 1.0;
        k = (k * 2) + 1;
        tempPI = tempPI * 0.5;
    }
}

/*void TestFFT(void){
	Uint16 i;
	// Clear input buffers:
		for(i=0; i < RFFT_SIZE; i++)
		{
			RFFTin1Buff[i] = 1.0f;
		}

		// Generate sample waveforms:
		Rad = 0.0f;
		RadStep = PI2/RFFT_SIZE;// 0.04908738521234051935097880286374;//0.1963495408494f;
		for(i=0; i < RFFT_SIZE; i++)
		{
			RFFTin1Buff[i]   = sin(Rad+D2R(0));// + cos(Rad*2.3567); //Real input signal
			//FFTImag_in[i] = cos(Rad*8.345) + sin(Rad*5.789);
			Rad +=  RadStep;
		}

	    rfft.FFTSize   = RFFT_SIZE;
	    rfft.FFTStages = RFFT_STAGES;
	    rfft.InBuf     = &RFFTin1Buff[0];  //Input buffer
	    rfft.OutBuf    = &RFFToutBuff[0];  //Output buffer
	    rfft.CosSinBuf = &RFFTF32Coef[0];  //Twiddle factor buffer
//	    rfft.MagBuf    = &RFFTmagBuff[0];  //Magnitude buffer

	    RFFT_f32_sincostable(&rfft);       //Calculate twiddle factor

	    for (i=0; i < RFFT_SIZE; i++)
	    {
	      	RFFToutBuff[i] = 0;			   //Clean up output buffer
	    }

//	    for (i=0; i < RFFT_SIZE/2; i++)
//	    {
//	     	RFFTmagBuff[i] = 0;		       //Clean up magnitude buffer
//	    }

	    RFFT_f32u(&rfft);				   //Calculate real FFT
//	    RFFT_f32_mag(&rfft);				//Calculate magnitude

}*/

/*  switch(Dbg_ADTempBufp){
    case 0:
        ProfileOutTime(Global1);
        ProfileInTime(Global4);
        MemProfileData(Global1,PROFILE_CHAN_AD0FFT);
        break;
    case 2:
        ProfileOutTime(Global2);
        ProfileInTime(Global5);
        MemProfileData(Global2,PROFILE_CHAN_AD2FFT);
        break;
    case 4:
        ProfileOutTime(Global3);
        ProfileInTime(Global6);
        MemProfileData(Global3,PROFILE_CHAN_AD4FFT);
        break;
    }
 */

//#if CLA_IFFT_EN == 0
//void IFFTCalculate(void){       //5.98us
//  float32 *pCPha=CalibCorr.Pha;
//  Uint32  *pCOrd=CalibCorr.harmOrder;
//
//  float32 *pDstReA=&IFFTData[0][0];
//  float32 *pDstImA=&IFFTData[1][0];
//  float32 *pDstReB=&IFFTData[2][0];
//  float32 *pDstImB=&IFFTData[3][0];
//  float32 *pDstReC=&IFFTData[4][0];
//  float32 *pDstImC=&IFFTData[5][0];
//  int16 xPhaseA,xPhaseB,xPhaseC;
//  float32 waveA=0,waveB=0,waveC=0;                            //Generating inverter side compensation current
//  Uint16 i;
//
////    ProfileGetVar(LOCALVAR);
////    ProfileInTime(LOCALVAR);
//  StepPhaA+=IFFT_PHASE_STEP;
//  PhaseLimit(StepPhaA);
//  StepPhaB+=IFFT_PHASE_STEP;
//  PhaseLimit(StepPhaB);
//  StepPhaC+=IFFT_PHASE_STEP;
//  PhaseLimit(StepPhaC);
////To calculate the fundamental power
//  xPhaseA=PhaseLimitI( (/**pCPha + */StepPhaA )*(PI2_SINE_LOOKTABLE/PI2) );
//  xPhaseB=PhaseLimitI( (/**pCPha + */StepPhaB )*(PI2_SINE_LOOKTABLE/PI2) );
//  xPhaseC=PhaseLimitI( (/**pCPha + */StepPhaC )*(PI2_SINE_LOOKTABLE/PI2) );
//
//  FundwaveA= ( pCoseLookTab(xPhaseA) * *(pDstReA++)-\
//               pSineLookTab(xPhaseA) * *(pDstImA++)); //matrix realA*realB-imagA*imagB
//  FundwaveB= ( pCoseLookTab(xPhaseB) * *(pDstReB++)-\
//               pSineLookTab(xPhaseB) * *(pDstImB++));
//  FundwaveC= ( pCoseLookTab(xPhaseC) * *(pDstReC++)-\
//               pSineLookTab(xPhaseC) * *(pDstImC++));
////To calculate the Harmnoic Current
//  for(i=0;i<CALIBRATION_COEFF_LEN;i++){
//      xPhaseA=PhaseLimitI( (*pCPha   + *pCOrd   * StepPhaA )*(PI2_SINE_LOOKTABLE/PI2) );
//      xPhaseB=PhaseLimitI( (*pCPha   + *pCOrd   * StepPhaB )*(PI2_SINE_LOOKTABLE/PI2) );
//      xPhaseC=PhaseLimitI( (*pCPha++ + *pCOrd++ * StepPhaC )*(PI2_SINE_LOOKTABLE/PI2) );
//
////        Testwave[5][i][TestIFFTBufp]=xPhaseA;
//      waveA+= ( pCoseLookTab(xPhaseA) * *(pDstReA++)-\
//                pSineLookTab(xPhaseA) * *(pDstImA++));    //matrix realA*realB-imagA*imagB
//      waveB+= ( pCoseLookTab(xPhaseB) * *(pDstReB++)-\
//                pSineLookTab(xPhaseB) * *(pDstImB++));
//      waveC+= ( pCoseLookTab(xPhaseC) * *(pDstReC++)-\
//                pSineLookTab(xPhaseC) * *(pDstImC++));
////        pCPha++;pCOrd++;
//  }
////    if(++TestIFFTBufp>=WAVERECORDLEN)
////        TestIFFTBufp=0;
//  CurrHarmRefA=waveA;
//  CurrHarmRefB=waveB;
//  CurrHarmRefC=waveC;
//
////    ProfileOutTime(LOCALVAR);
////    MemProfileData(LOCALVAR,PROFILE_CHAN_IFFT);
//}
//#endif

//int FFTDataReduction(Uint16 Chan){     //4.56us
//  float32 *pCMag,*pLimVal,harmSum=0,squre;
//  float32 *pSrc   =rfft.OutBuf;
//  float32 *pSrcEnd=rfft.OutBuf+RFFT_SIZE;
//  Uint32  *pCOrd=CalibCorr.harmOrder;
//  float32 *pCLimit=CalibCorr.limit;
//  float32 *pCStart=CalibCorr.start;
//  float32 *pDstRe,*pDstIm,Re,Im;
//  Uint16  *pLimDly;
//  Uint16  ord;
//
//  Uint16 i;
//  switch(Chan){
//  case 0: //A Channel
//      pDstRe=&IFFTData[0][0];
//      pDstIm=&IFFTData[1][0];
//      pLimVal=&LimitVal[0][0];
//      pLimDly=&LimitDly[0][0];
//      pCMag=CalibCorr.MagA;
//      break;
//  case 1: //B
//      pDstRe=&IFFTData[2][0];
//      pDstIm=&IFFTData[3][0];
//      pLimVal=&LimitVal[1][0];
//      pLimDly=&LimitDly[1][0];
//      pCMag=CalibCorr.MagB;
//      break;
//    case 2: //C
//        pDstRe=&IFFTData[4][0];
//        pDstIm=&IFFTData[5][0];
//        pLimVal=&LimitVal[2][0];
//        pLimDly=&LimitDly[2][0];
//        pCMag=CalibCorr.MagC;
//        break;
//    case 3: //D,逆变侧谐波校正算法
//        pDstRe=&IFFTData[4][0];
//        pDstIm=&IFFTData[5][0];
//        pLimVal=&LimitVal[2][0];
//        pLimDly=&LimitDly[2][0];
//        pCMag=CalibCorr.MagC;
//        return 0;
//
//  default:
//      pDstRe=&IFFTData[0][0];
//      pDstIm=&IFFTData[1][0];
//      pLimVal=&LimitVal[0][0];
//      pLimDly=&LimitDly[0][0];
//      pCMag=CalibCorr.MagA;
//      break;
//  }
//  //To calculate the fundamental power
//  *pDstRe++= MAGCORR(*(pSrc    + 1));//
//  *pDstIm++= MAGCORR(*(pSrcEnd - 1));//
//
//  //To calculate the Harmnoic Current5
//  for(i=0;i<CALIBRATION_COEFF_LEN;i++){
//      ord=*pCOrd++;
//      Re= *pCMag   * MAGCORR(*(pSrc    + ord));//
//      Im= *pCMag++ * MAGCORR(*(pSrcEnd - ord));//
//      if(*(pLimDly+i)){                   //Once the limit is started, it will last for some time.
//          Re*= *(pLimVal+i);
//          Im*= *(pLimVal+i);
//          (*(pLimDly+i))--;
//          harmSum+=(Re * Re + Im * Im);   //limit magtiute After calu Squre
//      }else{
//          squre=(Re * Re + Im * Im);
//          if(squre > *(pCLimit + i)){     //signal harmnoic limit magtiute
//              *(pLimVal+i)=sqrt(*(pCLimit + i) )/ sqrt(squre);
//              Re*= *(pLimVal+i);
//              Im*= *(pLimVal+i);
//              *(pLimDly+i)=10;
//              harmSum+=(Re * Re + Im * Im);
//          }else if(squre < *(pCStart + i)){
//              Re=0;  Im=0;
//          }else{
//              harmSum+=squre;
//          }
//      }
//      *pDstRe++=Re;//*HarmLimtCorr;
//      *pDstIm++=Im;//*HarmLimtCorr;
//  }
//
//  switch(Chan){
//  case 0: HarmSumA=sqrt(harmSum);
//      break;
//  case 1: HarmSumB=sqrt(harmSum);
//      break;
//  case 2: HarmSumC=sqrt(harmSum);
//      break;
//  default:
//      HarmSumA =0.0f;HarmSumB =0.0f;HarmSumC =0.0f;
//      break;
//  }
//  SetFFTChan();   //
//}

/*
//#define PROFILE_CYCLE

#ifdef PROFILE_CYCLE

#define PROFILE_BUFF 350
#define PROFILE_CHAR_FFT    "  FFT"
#define PROFILE_CHAR_IFFT   " IFFT"
#define PROFILE_CHAR_EPWM4  "EPWM4"
#define PROFILE_CHAR_DMA    "  DMA"
#define PROFILE_CHAR_ADDMA  "ADDMA"
#define PROFILE_CHAR_AD0FFT "AD0FT"
#define PROFILE_CHAR_AD2FFT "AD2FT"
#define PROFILE_CHAR_AD4FFT "AD4FT"
#define PROFILE_CHAR_AD6FFT "AD6FT"
#define PROFILE_CHAR_AD7FFT "AD7FT"
#define PROFILE_CHAR_AD8FFT "AD8FT"
#define PROFILE_CHAR_ADINT  "ADINT"
#define PROFILE_CHAN_FFT    0
#define PROFILE_CHAN_IFFT   1
#define PROFILE_CHAN_EPWM4  2
#define PROFILE_CHAN_DMA    3
#define PROFILE_CHAN_ADDMA  4
#define PROFILE_CHAN_AD0FFT 5
#define PROFILE_CHAN_AD2FFT 6
#define PROFILE_CHAN_AD4FFT 7
#define PROFILE_CHAN_AD6FFT 8
#define PROFILE_CHAN_AD7FFT 9
#define PROFILE_CHAN_AD8FFT 10
#define PROFILE_CHAN_ADINT  11

#define PROFILE_TEXT_DEFAULTS { PROFILE_CHAR_FFT,   PROFILE_CHAR_IFFT,  PROFILE_CHAR_EPWM4, PROFILE_CHAR_DMA,\
                                PROFILE_CHAR_ADDMA, PROFILE_CHAR_AD0FFT,PROFILE_CHAR_AD2FFT,PROFILE_CHAR_AD4FFT,\
                                PROFILE_CHAR_AD6FFT,PROFILE_CHAR_AD7FFT,PROFILE_CHAR_AD8FFT,PROFILE_CHAR_ADINT}
extern Uint32 ProfileDat[PROFILE_BUFF][5];
extern char *ProfileText[12];               //Up to record 12 channel
extern Uint16 ProfilePos,ProfileDispPos;
extern Uint32 profileInTimeGlobal1,profileOutTimeGlobal1,*pProfileDstGlobal1;
extern Uint32 profileInTimeGlobal2,profileOutTimeGlobal2,*pProfileDstGlobal2;

#define ProfileGetVar(Var)  Uint32 profileInTime##Var,profileOutTime##Var,*pProfileDst##Var //每个函数中如果要分段显示,则每段需要调用不同的以下函数
#define ProfileInTime(Var)   profileInTime##Var  =ReadCpuTimer0Counter()    //同上
#define ProfileOutTime(Var)  profileOutTime##Var =ReadCpuTimer0Counter()
#define LOCALVAR 1
//timer是减计数
#define MemProfileData(Var,chan)\
        DINT;\
        if(++ProfilePos>=PROFILE_BUFF)\
            ProfilePos=0;\
        pProfileDst##Var=&ProfileDat[ProfilePos][0];\
        EINT;\
        *pProfileDst##Var++=chan;\
        *pProfileDst##Var++=profileInTime##Var>=profileOutTime##Var\
                    ?profileInTime##Var-profileOutTime##Var :(Uint32)0xffffffff-profileOutTime##Var+profileInTime##Var;\
        *pProfileDst##Var++=profileInTime##Var;\
        *pProfileDst##Var++=profileOutTime##Var;\
        *pProfileDst##Var=ProfilePos;
//      ProfileDat[ProfilePos][0]=chan;\
//      ProfileDat[ProfilePos][1]=profileInTime##Var>=profileOutTime##Var\
//          ?profileInTime##Var-profileOutTime##Var :(Uint32)0xffffffff-profileOutTime##Var+profileInTime##Var;\
//      ProfileDat[ProfilePos][2]=profileInTime##Var;\
//      ProfileDat[ProfilePos][3]=profileOutTime##Var;\
        EINT;


#else
#define ProfileGetVar(Var)
#define ProfileInTime(Var)
#define ProfileOutTime(Var)
#define MemProfileData(Var,chan)

#endif

#ifdef PROFILE_CYCLE
    #pragma CODE_SECTION(profileDisp, "ram2func");
    #pragma DATA_SECTION(ProfileDat,"WAVEDATA1");
    Uint32 ProfileDat[PROFILE_BUFF][5];
    char *ProfileText[12]=PROFILE_TEXT_DEFAULTS;
    Uint16 ProfilePos=0,ProfileDispPos=0;
    //Uint32 profileInTimeGlobal1=0,profileOutTimeGlobal1=0,*pProfileDstGlobal1=&ProfileDat[0][0];
    //Uint32 profileInTimeGlobal2=0,profileOutTimeGlobal2=0,*pProfileDstGlobal2=&ProfileDat[0][0];
    ProfileGetVar(Global1);

void profileDisp(void){
    while(ProfileDispPos!=ProfilePos){\
    if(++ProfileDispPos>=PROFILE_BUFF)
        ProfileDispPos=0;\
    printf("%s,D,%u,I,%x%x,O,%x%x,@%u\n",ProfileText[ ProfileDat[ProfileDispPos][0]],(Uint16)ProfileDat[ProfileDispPos][1],\
            (Uint16)(ProfileDat[ProfileDispPos][2]>>16),(Uint16)ProfileDat[ProfileDispPos][2],\
            (Uint16)(ProfileDat[ProfileDispPos][3]>>16),(Uint16)ProfileDat[ProfileDispPos][3],\
            (Uint16)(ProfileDat[ProfileDispPos][4]));\
    }
}
#endif
*/
//void InitXIntrupt(void){
//  XIntruptRegs.XNMICR.bit.SELECT=0;   //Timer 1 connected To INT13
////    StartCpuTimer1();
//}
//DMA Software bit (CHx.CONTROL.PERINTFRC) only	//閸氼垰濮〥MA鐎瑰本鍨氶弫鐗堝祦閹碱剛些閸氬骸鎯庨崝鈺癷mer2
//===========================================================================
// End of File
//===========================================================================

