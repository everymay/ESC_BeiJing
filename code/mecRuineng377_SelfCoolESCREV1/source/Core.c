/*
 * Core.c
 *
 *  Created on: 2016-3-3
 *      Author: HR
 */
#include "fdacoefs.h"
#include "F2837xS_device.h"
#include "VariblesRef.h"
#include "math.h"
#include "C28x_FPU_FastRTS.h"
#include "DCL.h"
#include "F2837xS_Examples.h"
#include "MarcoDefine.h"
#include "CoreCtrl.h"
#include "DSP28x_Project.h"
#include "mbport.h"
#include "Modbus.h"
#include "VarMean.h"
#include "DPLL.h"

#pragma CODE_SECTION(clock5ms, "ram2func");
#pragma CODE_SECTION(clock1ms, "ram2func");
//#pragma	CODE_SECTION(ReactivePowerGiven ,"ram2func")
#pragma CODE_SECTION(Funda3sTo2rTransf, "ram2func");
#pragma CODE_SECTION(FirstRmsCalc, "ram2func");
#pragma CODE_SECTION(SlowRmsCalc, "ram2func");
#pragma CODE_SECTION(RmsCalcIn, "ram2func");
//#pragma CODE_SECTION(MeanInit, "ram2func");
#pragma CODE_SECTION(RMSLimit, "ram2func");
#pragma CODE_SECTION(CurrLimitPri, "ram2func");
#pragma CODE_SECTION(CurrLimitZPri, "ram2func");
#pragma CODE_SECTION(GetCapaReactCorr, "ram2func");
#pragma CODE_SECTION(GetHarmLimitCorr, "ram2func");
#pragma CODE_SECTION(GetCapaUnbalCorr, "ram2func");
//#pragma CODE_SECTION(VoltageCompensation, "ram2func");
#pragma CODE_SECTION(RunPIRE, "ram2func");
#pragma CODE_SECTION(Max3, "ram2func");
#pragma CODE_SECTION(SigPhDQComput, "ram2func");


extern Event_Handle Event_SetStartSM;
extern Event_Handle Event_CapSwitchSM;
extern Event_Handle Event_Outside;

float32 CtrlVoltA=0,CtrlVoltB=0,CtrlVoltC=0;
float32 CurFundA=0,CurFundB=0,CurFundC=0,CurFundStandbyA=0,CurFundStandbyB=0,CurFundStandbyC=0;
float32 VolFundA=0,VolFundB=0,VolFundC=0;
//float32 FundwaveA=0,FundwaveB=0,FundwaveC=0;
float32 CurrRefA=0,CurrRefB=0,CurrRefC=0;
float32 CurrRefStandbyA=0,CurrRefStandbyB=0,CurrRefStandbyC=0;
float32 curFundNg;
float currRef_D = 0;
float CtrlVoltUV = 0;

#pragma DATA_SECTION(FlvSlot,"EBSS3");
#pragma DATA_SECTION(RmsSumIn,"EBSS3");
#pragma DATA_SECTION(RmsSumOut,"EBSS3");
//#pragma DATA_SECTION(RmsSumQ,"EBSS3");
float32 FlvSlot[FLV_NUM];

float32 RmsSumIn[ALL_RMSNUM], RmsSumOut[ALL_RMSNUM], RmsSumQ[RMS_CALU_SEGMENT_NUM][ALL_RMSNUM];

float32 UnUsedStoge=0;
//#pragma DATA_SECTION(DQbufIn,"EBSS3");
float DQbufIn[MEANPOINT_QUARTER][MEANPOINT_QUARTER_CHAN];

#define CLARKE2AlphaBeta(A,B,C,Z) 	\
				Z = ((A) + (B) + (C))*S1DIV3;\
				zA = (A)-(Z);\
				zB = (B)-(Z);\
				zC = (C)-(Z);\
				alpha = (SQRT2* SQRT_2DIV3) * (zA - 0.5f*zB - 0.5f*zC);\
			 	beta  = (SQRT2*SQRT2_DIV2) * (zB - zC)



inline float PARK2D(float alpha,float beta,float resSin,float resCos){
 	return (beta *resSin + alpha*resCos)*(SQRT2_DIV2);
}

inline float PARK2Q(float alpha,float beta ,float resSin,float resCos){
 	return (beta *resCos-alpha*resSin)*(SQRT2_DIV2);
}




inline float * PARK2DQ(float *p,float resSin,float resCos,float alpha,float beta)
{
    *p++=(beta *resSin + alpha*resCos)*(SQRT2_DIV2);    //D
    *p++=(alpha*resSin - beta *resCos)*(SQRT2_DIV2);    //Q
    return p;
}

float32 testout[FIFO_DQ_DATA_SIZE];

//6.4k����Ƶ��
void SigPhDQCal(void)
{
    float32 * pIn,*pfifo;
    int i;
    float32 theta,resSin,resCos,DQCalTheta;

    for(i=0;i<FIFO_DQ_BLOCK_NUM;i++){                   //�������ݻ������۶��ٵ�,�����һ����,��ΪDQ�����ƽ�ȵ�
        if(!cbIsEmpty(&BlockFifo)){                     //�ж�fifo�ǿ�
            pfifo = testout;                            //�õ�fifo�����ָ��
            pIn = rtb_Vq;                               //�õ�MEAN������ָ��

            cbRead(&BlockFifo,pfifo);                   //fifo�ж�

            DQCalTheta  = *pfifo++;                                 //0�õ��������ݵ���λ
            *pIn++      = *pfifo++;                                 //1�õ�ֱ����ѹ

            theta=DQCalTheta - (PI*1/180);//VoltCurrCalibrPhase;    //�������ѹ�˲����������λ��ʱУ��
            if(theta < 0)   theta += PI2;                           //2*PI limiting
            sincosf(PhaseLimit(theta         ),&resSin,&resCos);
            pIn= PARK2DQ(pIn,resSin,resCos,*pfifo++,*pfifo++);      //2,3
            sincosf(PhaseLimit(theta-(PI*2/3)),&resSin,&resCos);
            pIn= PARK2DQ(pIn,resSin,resCos,*pfifo++,*pfifo++);      //4,5
            sincosf(PhaseLimit(theta+(PI*2/3)),&resSin,&resCos);
            pIn= PARK2DQ(pIn,resSin,resCos,*pfifo++,*pfifo++);      //6,7

            theta=DQCalTheta - LoadCurrCalibrPhase;                 //���ز������˲����������λ��ʱУ��
            if(theta < 0)   theta += PI2;                           //2*PI limiting
            sincosf(PhaseLimit(theta         ),&resSin,&resCos);
            pIn= PARK2DQ(pIn, resSin,resCos,*pfifo++,*pfifo++);     //8,9
            sincosf(PhaseLimit(theta-(PI*2/3)),&resSin,&resCos);
            pIn= PARK2DQ(pIn, resSin,resCos,*pfifo++,*pfifo++);     //10,11
            sincosf(PhaseLimit(theta+(PI*2/3)),&resSin,&resCos);
            pIn= PARK2DQ(pIn, resSin,resCos,*pfifo++,*pfifo++);     //12,13

            theta=DQCalTheta - GridCurrCalibrPhase;                 //����������˲����������λ��ʱУ��
            if(theta < 0)   theta += PI2;                           //2*PI limiting
            sincosf(PhaseLimit(theta         ),&resSin,&resCos);
            pIn= PARK2DQ(pIn, resSin,resCos,*pfifo++,*pfifo++);     //14,15
            sincosf(PhaseLimit(theta-(PI*2/3)),&resSin,&resCos);
            pIn= PARK2DQ(pIn, resSin,resCos,*pfifo++,*pfifo++);     //16,17
            sincosf(PhaseLimit(theta+(PI*2/3)),&resSin,&resCos);
            pIn= PARK2DQ(pIn, resSin,resCos,*pfifo++,*pfifo++);     //18,19


            theta =DQCalTheta - InvCurrCalibrPhase;                 //��������˲����������λ��ʱУ��
            if(theta < 0)   theta += PI2;                           //2*PI limiting
            sincosf(PhaseLimit(theta         ),&resSin,&resCos);
            pIn= PARK2DQ(pIn, resSin,resCos,*pfifo++,*pfifo++);     //20,21
            sincosf(PhaseLimit(theta-(PI*2/3)),&resSin,&resCos);
            pIn= PARK2DQ(pIn, resSin,resCos,*pfifo++,*pfifo++);     //22,23
            sincosf(PhaseLimit(theta+(PI*2/3)),&resSin,&resCos);
            pIn= PARK2DQ(pIn, resSin,resCos,*pfifo++,*pfifo++);     //24,25

            float32 zA,zB,zC,alpha,beta;

            //Fundamental zero sequence current needs to be done after the operation of the fundamental wave FFT
            LoadFundaCurZ  =(FundwaveA + FundwaveB + FundwaveC)*S1DIV3; //The Fundamental zero sequence

            //The positive and negative d,q,zero axis of Load current
            theta = DQCalTheta;      //��������˲����������λ��ʱУ��
            if(theta < 0)   theta += PI2;                           //2*PI limiting
            sincosf(PhaseLimit(theta         ),&resSin,&resCos);

            CLARKE2AlphaBeta(testout[8],testout[10],testout[12],LoadRealCurZ);
            pIn= PARK2DQ(pIn,  resSin, resCos,alpha,beta);         //26,27,LoadFundaCurD,LoadFundaCurQ
            pIn= PARK2DQ(pIn, -resSin, resCos,alpha,beta);         //28,29,LoadFundaCurND,LoadFundaCurNQ

            //The positive and negative d,q,zero axis of Load current ,for decoupling control
            CLARKE2AlphaBeta(testout[20],testout[22],testout[24],ApfOutCurZ);
            pIn= PARK2DQ(pIn,  resSin, resCos,alpha,beta);         //30,31,InvFundaCurD,InvFundaCurQ
            pIn= PARK2DQ(pIn, -resSin, resCos,alpha,beta);         //32,33,InvFundaCurND,InvFundaCurNQ

            //The positive and negative d,q,zero axis of grid Current
            CLARKE2AlphaBeta(testout[14],testout[16],testout[18],GridZeroCur);
            pIn= PARK2DQ(pIn, resSin,resCos,alpha,beta);          //34,35,GridFundaCurD,GridFundaCurQ

            varMean_step();                     //mean�㷨.21.5us@24channel
        }else{break;}
    }

    float *pOut=VarMeanOut;             //�õ����һ�����������

    dcVoltDiv2Err       =*pOut++;       //1
    GridVoltDA          =*pOut++;       //2
    GridVoltQA          =*pOut++;       //3
    GridVoltDB          =*pOut++;       //4
    GridVoltQB          =*pOut++;       //5
    GridVoltDC          =*pOut++;       //6
    GridVoltQC          =*pOut++;       //7

    LoadRealCurDA       =*pOut++;       //8
    LoadRealCurQA       =*pOut++;
    LoadRealCurDB       =*pOut++;
    LoadRealCurQB       =*pOut++;
    LoadRealCurDC       =*pOut++;
    LoadRealCurQC       =*pOut++;

    GridRealCurDA       =*pOut++;       //14
    GridRealCurQA       =*pOut++;
    GridRealCurDB       =*pOut++;
    GridRealCurQB       =*pOut++;
    GridRealCurDC       =*pOut++;
    GridRealCurQC       =*pOut++;

    IverRealCurDA       =*pOut++;       //20
    IverRealCurQA       =*pOut++;
    IverRealCurDB       =*pOut++;
    IverRealCurQB       =*pOut++;
    IverRealCurDC       =*pOut++;
    IverRealCurQC       =*pOut++;

    LoadFundaCurD       =*pOut++;       //26
    LoadFundaCurQ       =*pOut++;
    LoadFundaCurND      =*pOut++;
    LoadFundaCurNQ      =*pOut++;

    InvFundaCurD        =*pOut++;       //30
    InvFundaCurQ        =*pOut++;
    InvFundaCurND       =*pOut++;
    InvFundaCurNQ       =*pOut++;

    GridFundaCurD       =*pOut++;       //34
    GridFundaCurQ       =*pOut++;       //35
}


float testin[FIFO_DQ_DATA_SIZE];

void SigPhDQinput(void)
{
    float32 * pIn=testin;
    if(++CurrThisPos >= MEANPOINT_QUARTER)      CurrThisPos = 0;
    if(++CurrPrvPos >= MEANPOINT_QUARTER)       CurrPrvPos = 0;
    float *pThisSrc     = DQbufIn[CurrThisPos];
    float *pPrvSrc      = DQbufIn[CurrPrvPos];
    float *pCapAlpha    = 0;//VoltSlid[VoltPos];
    float *pCapBeta     = 0;//VoltSlid[CapVoltPos];

    *pIn++ = SPLL[0].Theta;                    //1,���浱ǰʱ�̵�����ֵ
    *pIn++ = DCtestMirr = dcVoltUpF - dcVoltDnF;         //2

    *pIn++= *pCapAlpha++;                   //3,������ѹA��alpha
    *pIn++= *pCapBeta++;                    //4,������ѹA��beta
    *pIn++= *pCapAlpha++;
    *pIn++= *pCapBeta++;
    *pIn++= *pCapAlpha++;
    *pIn++= *pCapBeta++;

    *pIn++= *pThisSrc++ = LoadRealCurA;     //9
    *pIn++= *pPrvSrc++;                     //10
    *pIn++= *pThisSrc++ = LoadRealCurB;
    *pIn++= *pPrvSrc++;
    *pIn++= *pThisSrc++ = LoadRealCurC;
    *pIn++= *pPrvSrc++;

    *pIn++= *pThisSrc++ = GridCurrA;        //15,�Ž�1/4������,�Ҹ�fifo
    *pIn++= *pPrvSrc++;                     //16��1/4��������ʱ��ȡ����fifo
    *pIn++= *pThisSrc++ = GridCurrB;
    *pIn++= *pPrvSrc++;
    *pIn++= *pThisSrc++ = GridCurrC;
    *pIn++= *pPrvSrc++;

    *pIn++= *pThisSrc++ = ApfOutCurAD;      //21
    *pIn++= *pPrvSrc++;                     //
    *pIn++= *pThisSrc++ = ApfOutCurBD;
    *pIn++= *pPrvSrc++;
    *pIn++= *pThisSrc++ = ApfOutCurCD;
    *pIn++= *pPrvSrc++;

    cbWrite(&BlockFifo,testin);        //д��fifo
}


void SigPhDQComput(void)
{
    Uint16 temp0 = (Uint16)(Esc_VoltPhaseA*PLL_STEP);           //float convert to int
    temp0 %=  SINE_LOOKTABLE_LEN;
    float resSinA=pSineLookTab(temp0);
    float resCosA=pCoseLookTab(temp0);

    temp0 = (Uint16)(Esc_VoltPhaseB*PLL_STEP);                  //float convert to int
    temp0 %=  SINE_LOOKTABLE_LEN;
    float resSinB=pSineLookTab(temp0);
    float resCosB=pCoseLookTab(temp0);

    temp0 = (Uint16)(Esc_VoltPhaseC*PLL_STEP);                  //float convert to int
    temp0 %=  SINE_LOOKTABLE_LEN;
    float resSinC=pSineLookTab(temp0);
    float resCosC=pCoseLookTab(temp0);

    GridRealCurQA2   =-DCL_runDF22(&SinglePhaseDispFilter[0],    PARK2D(GridCurrAF,GridCurrAF_Beta,   resSinA,resCosA));
    GridRealCurQB2   =-DCL_runDF22(&SinglePhaseDispFilter[1],    PARK2D(GridCurrBF,GridCurrBF_Beta,   resSinB,resCosB));
    GridRealCurQC2   =-DCL_runDF22(&SinglePhaseDispFilter[2],    PARK2D(GridCurrCF,GridCurrCF_Beta,   resSinC,resCosC));

    GridRealCurDA2   = DCL_runDF22(&SinglePhaseDispFilter[3],    PARK2Q(GridCurrAF,GridCurrAF_Beta,   resSinA,resCosA));
    GridRealCurDB2   = DCL_runDF22(&SinglePhaseDispFilter[4],    PARK2Q(GridCurrBF,GridCurrBF_Beta,   resSinB,resCosB));
    GridRealCurDC2   = DCL_runDF22(&SinglePhaseDispFilter[5],    PARK2Q(GridCurrCF,GridCurrCF_Beta,   resSinC,resCosC));

    GridRealCurErrA =DCL_runPI(&GridCurrPICtrlA, GridRealCurQA2 , 0);
    GridRealCurErrB =DCL_runPI(&GridCurrPICtrlB, GridRealCurQB2 , 0);
    GridRealCurErrC =DCL_runPI(&GridCurrPICtrlC, GridRealCurQC2 , 0);
}


#define THDI_RMS_SQRT3CALU(chan)\
    pH->im      = (*p++)*(POWERGRID_FREQ*CTRLSTEP_ALGORITHM);\
    pH->re      = (*p++)*(POWERGRID_FREQ*CTRLSTEP_ALGORITHM);\
    pH++->rms   = *p*(POWERGRID_FREQ*CTRLSTEP_ALGORITHM);\
    (chan)= SQRT3*sqrtf(*p++ *(POWERGRID_FREQ*CTRLSTEP_ALGORITHM))

#define THDI_RMS_CALU(chan)\
    pH->im      = (*p++)*(POWERGRID_FREQ*CTRLSTEP_ALGORITHM);\
    pH->re      = (*p++)*(POWERGRID_FREQ*CTRLSTEP_ALGORITHM);\
    pH++->rms   = *p*(POWERGRID_FREQ*CTRLSTEP_ALGORITHM);\
    (chan)= sqrtf(*p++ *(POWERGRID_FREQ*CTRLSTEP_ALGORITHM))

#define THDI_RMS_INPUT(chan)\
    *p++ += (chan) * GridResSin;\
    *p++ += (chan) * GridResCos;\
    *p++ += (chan) * (chan)
/*
 * RmsSumIn[x]�д�ŵ�������Ҫ����ͨ����RMS_CALU_BLOCK_NUM����ļ����,����������ʱ��ʡ������.��Ϊ����û��Ҫÿ�������
 * ����Ҫһ�������.�ɱ���8�������,���1��.��Ϊ��˵�Ҫ������û��ô��.ÿ���ܲ�����16�μ���.
  �������������������������������������������������������������������������������Щ������������������������������������������Щ������������������������������Щ���������������������������������������
  ��ApfOutCurAD[0]^2                      ��ApfOutCurBD[0]^2     ��    ...        ��LoadRealCurZ[0]^2  ��
  ��     +[...n+1]                        ��                     ��               ��                   ��
  ��ApfOutCurAD[RMS_CALU_SEGMENT-1]^2     ��                     ��               ��                   ��
  �������������������������������������������������������������������������������ة������������������������������������������ة������������������������������ة���������������������������������������
*/

void RmsCalcIn(void)          //����AD����,�ɵ������ݷ��ڶ�ά���鵱��
{
    Uint16 i;
    float *p=RmsSumIn,*pDst;              //���׵�ַ

    *p++ += VoltInAF * VoltInAF;          //1
    *p++ += VoltInBF * VoltInBF;
    *p++ += VoltInCF * VoltInCF;
    *p++ += VoltOutAF * VoltOutAF;        //4
    *p++ += VoltOutBF * VoltOutBF;
    *p++ += VoltOutCF * VoltOutCF;
    *p++ += GridCurrAF * GridCurrAF;      //7
    *p++ += GridCurrBF * GridCurrBF;
    *p++ += GridCurrCF * GridCurrCF;
    *p++ += GridBPCurrAF * GridBPCurrAF;      //10
    *p++ += GridBPCurrBF * GridBPCurrBF;
    *p++ += GridBPCurrCF * GridBPCurrCF;


// ������ѹ����������������������ص���������
//    THDI_RMS_INPUT(GridVoltAF);     //x[n]^2+x[n-1]^2+...+x[N-1]^2
//    THDI_RMS_INPUT(GridVoltBF);
//    THDI_RMS_INPUT(GridVoltCF);
//    THDI_RMS_INPUT(LoadVoltUF);
//    THDI_RMS_INPUT(LoadVoltVF);
//    THDI_RMS_INPUT(LoadVoltWF);
//    THDI_RMS_INPUT(GridCurrAF);
//    THDI_RMS_INPUT(GridCurrBF);
//    THDI_RMS_INPUT(GridCurrCF);

    *p++ += LoadFundaCurZ*LoadFundaCurZ;        //35    //���ֵ��DQģ�����
    *p++ += CurFundNgA*CurFundNgA;              //36
    *p++ += CurFundNgB*CurFundNgB;
    *p++ += CurFundNgC*CurFundNgC;
    float temp=CurrHarmRefA +CurrHarmRefB +CurrHarmRefC;
    *p++ += temp*temp;
    *p++ += ApfOutCurZ*ApfOutCurZ;
    *p++ += GridZeroCur*GridZeroCur;            //41
    *p++ += LoadRealCurZ*LoadRealCurZ;          //42

    //�μ���,������
    if( (++RmsCaluBlockCnt >= RMS_CALU_BLOCK_NUM) || (StateFlag.ForceRMSRefresh) )  //jcl:ForceRMSRefresh�߼��ø�
    {
        RmsCaluBlockCnt=0;                      //ÿ�εĵ�������,һ������
        p=RmsSumIn;                             //ָ��ÿ�εĵ���֮�͵��׵�ַ
        pDst=RmsSumQ[RmsCaluCnt];               //����ÿһ�ε�����
        for(i=0;i<ALL_RMSNUM;i++){
            *pDst++ = *p;                       //��RmsSum��һ��������RMS_CALU_SEGMENT_SIZE_NUM�������ĺ�,������RmsSumQ��
            *p++=0;                             //RmsSum����,����RmsSumIn��������һ��֮��.
        }
        if(++RmsCaluCnt>=RMS_CALU_SEGMENT_NUM) RmsCaluCnt=0;     //ѭ����������������
    }
}

/*���ڲ��û��������㷨������Чֵ��ԭ���������Ϊĳ��ԭ�����κ����,�����ڵ�����������һֱ���ڶ���������.������װ�õĽ�׳��,�ʲ�����.
  �±�ΪRmsSumQ[��][��]�Ľ���
  RmsSumQ[x][0:RMS_CALU_SEGMENT]ÿ�д�һ·ģ��ͨ��������,������RmsSumIn[x].
  �˻�����Ϊѭ��������.Ϊ��֤���ݲ����첽�̵߳�д�뵼�����ݱ�ˢ�µ�,�ʻ���������3��.
  ��ЧֵRMS=(RmsSumQ[x][0:RMS_CALU_SEGMENT-1])�ĺͳ���(POWERGRID_FREQ*CTRLSTEP_ALGORITHM)
  �������������������������������������������������������������������Щ������������������������������������������������������������������Щ������������Щ�������������������������������������������������������������������������������������������������
  ��RmsSumIn[0]t(0)                 ��RmsSumIn[1]t(0)                  �� ...  ��RmsSumIn[ALL_RMSNUM-1]t(0)                      ��
  ��                                ��                                 ��      ��                                                ��
  �������������������������������������������������������������������੤�����������������������������������������������������������������੤�����������੤������������������������������������������������������������������������������������������������
  ��RmsSumIn[0]t(1)                 ��RmsSumIn[1]t(1)                  �� ...  ��RmsSumIn[ALL_RMSNUM-1]t(1)                      ��
  ��                                ��                                 ��      ��                                                ��
  �������������������������������������������������������������������੤�����������������������������������������������������������������੤�����������੤������������������������������������������������������������������������������������������������
  ��    ...                         ��                                 ��      ��                                                ��
  ��                                ��                                 ��      ��                                                ��
  �������������������������������������������������������������������੤�����������������������������������������������������������������੤�����������੤������������������������������������������������������������������������������������������������
  ��RmsSumIn[0]t(RMS_CALU_SEGMENT)  ��RmsSumIn[1]t(RMS_CALU_SEGMENT)   �� ...  ��RmsSumIn[ALL_RMSNUM-1]t(RMS_CALU_SEGMENT)       ��
  ��                                ��                                 ��      ��                                                ��
  �������������������������������������������������������������������ة������������������������������������������������������������������ة������������ة�������������������������������������������������������������������������������������������������
 */

void SlowRmsCalc(void)         //��������Чֵ,�ǽ��������ڷ�Ϊ�Ķ�,����1/4����ȡֵ,���뽫�������ڵ�ֵ��ȡ��,���ܽ�����Чֵ����.
{
    STRU_HarmTHD *pH=(STRU_HarmTHD*) HarmTHD;    //�ڲɼ�һ�����ܲ�(CTRLFREQUENCY/POWERGRID_FREQ)����ʱ,ָ��HarmTHD�׵�ַ,����calcuFlag����һ�ּ���
    float *pSrc,*p;
    Uint16 i,j;
    int16 addr;

    if((RmsCaluCnt==0) || (StateFlag.ForceRMSRefresh))          //ÿ��20msˢ��һ��
    {
        p = &RmsSumOut[FIRST_RMSNUM];                           //���RMS��������ַ
        for(i=ALL_RMSNUM-FIRST_RMSNUM;i;i--)                    //��FIRST_RMSNUM~ALL_RMSNUM·���
            *p++=0;                                             //RMS�����������

        addr=RmsCaluCnt;
        for(i=0;i<(RMS_CALU_SEGMENT);i++){                      //ÿͨ����RMS_CALU_SEGMENT�����ݲ���RMS�㷨�ļ���
            if(--addr<0) addr = RMS_CALU_SEGMENT_NUM-1;         //��ȡRMS������������ʱ����õĵ�ַ
            p = &RmsSumOut[FIRST_RMSNUM];                       //���RMS��������ַ
            pSrc = &RmsSumQ[addr][FIRST_RMSNUM];
            for(j=ALL_RMSNUM-FIRST_RMSNUM;j;j--)
            {
                *p++ += *pSrc++ ;   //�����������λ�����ȡ��
            }
         //   *p++;                                             //ָ����һ��RMS�������
        }

        p=&RmsSumOut[FIRST_RMSNUM];                             //�ػ��RMS��������ַ


        THDI_RMS_CALU(VoltInA_rms);//������ѹ          г����Чֵ/�ܵ���Чֵ=THDI;������Чֵ/�ܵ���Чֵ=THDF;
        THDI_RMS_CALU(VoltInB_rms);
        THDI_RMS_CALU(VoltInC_rms);

        THDI_RMS_CALU(VoltOutA_rms);//���ص�ѹ
        THDI_RMS_CALU(VoltOutB_rms);
        THDI_RMS_CALU(VoltOutC_rms);

        THDI_RMS_CALU(gridCurA_rms);//��������
        THDI_RMS_CALU(gridCurB_rms);
        THDI_RMS_CALU(gridCurC_rms);


        StateFlag.cntForTHDi = 1;               //����THDi�ļ���
        GpVoltA_rmsReciprocal=ki_Cur/(VoltInA_rms*SQRT_2DIV3);
        GpVoltB_rmsReciprocal=ki_Cur/(VoltInB_rms*SQRT_2DIV3);
        GpVoltC_rmsReciprocal=ki_Cur/(VoltInC_rms*SQRT_2DIV3);
        LoadFundaCurZRms    = sqrtf(*p++ *(POWERGRID_FREQ*CTRLSTEP_ALGORITHM));
        CurFundNgARms       = sqrtf(*p++ *(POWERGRID_FREQ*CTRLSTEP_ALGORITHM));
        CurFundNgBRms       = sqrtf(*p++ *(POWERGRID_FREQ*CTRLSTEP_ALGORITHM));
        CurFundNgCRms       = sqrtf(*p++ *(POWERGRID_FREQ*CTRLSTEP_ALGORITHM));
        CurrHarmRefZ        = sqrtf(*p++ *(POWERGRID_FREQ*CTRLSTEP_ALGORITHM));

        ApfOutCurZRms       = sqrtf(*p++ *(POWERGRID_FREQ*CTRLSTEP_ALGORITHM));
        GridNeutralCurRms   = sqrtf(*p++ *(POWERGRID_FREQ*CTRLSTEP_ALGORITHM));
        LoadRealCurZRms     = sqrtf(*p++ *(POWERGRID_FREQ*CTRLSTEP_ALGORITHM));

        GridVoltRms=(VoltInA_rms+VoltInB_rms+VoltInC_rms)*S1DIV3;
        ThreeHarmGridVoltRms=GridVoltRms*(S1DIVSQRT3*SQRT2*S1DIV6);
        if(GridVoltRms>3000)            GridVoltRms=3000;
        FLVFUN(GridVoltRms_F,GridVoltRms,VOLT);

        GridCurRms = (gridCurA_rms + gridCurB_rms + gridCurC_rms)*S1DIV3;
        if(GridCurRms>10000)            GridCurRms=10000;
        FLVFUN(GridCurRms_F,GridCurRms,GRIDCURR);

        LoadVoltRms = (VoltOutA_rms + VoltOutB_rms + VoltOutC_rms)*S1DIV3;
        if(LoadVoltRms>10000)            LoadVoltRms=10000;
        FLVFUN(LoadVoltRms_F,LoadVoltRms,LOADCURR);
    }
}

/*
 * ���ܣ������ѹ�͵�����Чֵ��WY
 */
void FirstRmsCalc(void)           //������Чֵ�㷨,ȡ���������ڵ�1/4֮��,��ֱ�ӽ�����Чֵ����.
{
	Uint16 i, j;
	int16 addr;
	float *p = &RmsSumOut[0], *pSrc; //���RMS���������׵�ַ

	for(i = FIRST_RMSNUM; i; i --)
		*p ++ = 0; //RMS�����������

	addr = RmsCaluCnt;

	for(i = RMS_CALU_SEGMENT; i; i --)
	{ //����RMS_CALU_SEGMENT�����ݼ���
		if(-- addr < 0)
			addr = RMS_CALU_SEGMENT_NUM - 1; //��ȡRMS������������ʱ����õĵ�ַ
		p = &RmsSumOut[0]; //���RMS���������׵�ַ
		pSrc = &RmsSumQ[addr][0];
		for(j = FIRST_RMSNUM; j; j --) //��FIRST_RMSNUM~ALL_RMSNUM������ALL_RMSNUM·���
		{
			*p ++ += *pSrc ++; //�����������λ�����ȡ��
		}
	}

	p = &RmsSumOut[0]; //�ػ��RMS���������׵�ַ

	ESCFlagA.VoltIn_rms = VoltInA_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));
	ESCFlagB.VoltIn_rms = VoltInB_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));
	ESCFlagC.VoltIn_rms = VoltInC_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));

	ESCFlagA.VoltOut_rms = VoltOutA_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));
	ESCFlagB.VoltOut_rms = VoltOutB_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));
	ESCFlagC.VoltOut_rms = VoltOutC_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));

	if(StateFlag.VoltageModeFlag == 0)
	{
		UnCurrData[0] = ESCFlagA.gridCur_rms = CURRData[0] = gridCurA_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));
		UnCurrData[1] = ESCFlagB.gridCur_rms = CURRData[1] = gridCurB_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));
		UnCurrData[2] = ESCFlagC.gridCur_rms = CURRData[2] = gridCurC_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));

		CURRData[3] = ESCFlagA.gridCurrBY_rms = gridCurrBYAF_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));
		CURRData[4] = ESCFlagB.gridCurrBY_rms = gridCurrBYBF_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));
		CURRData[5] = ESCFlagC.gridCurrBY_rms = gridCurrBYCF_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));
	}
	else if(StateFlag.VoltageModeFlag == 1)
	{
		ESCFlagA.gridCur_rms = CURRData[0] = gridCurA_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));
		ESCFlagB.gridCur_rms = CURRData[1] = gridCurB_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));
		ESCFlagC.gridCur_rms = CURRData[2] = gridCurC_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));

		UnCurrData[0] = CURRData[3] = ESCFlagA.gridCurrBY_rms = gridCurrBYAF_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));
		UnCurrData[1] = CURRData[4] = ESCFlagB.gridCurrBY_rms = gridCurrBYBF_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));
		UnCurrData[2] = CURRData[5] = ESCFlagC.gridCurrBY_rms = gridCurrBYCF_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));
	}

	ResonProtcABCRms = sqrtf(*p * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));

#if ESC_THREEPHASECTRL
	GridVoltRms = (VoltInA_rms + VoltInB_rms + VoltInC_rms) * S1DIV3;
	ThreeHarmGridVoltRms = GridVoltRms * (S1DIVSQRT3 * SQRT2 * S1DIV6);
	if(GridVoltRms > 3000)
		GridVoltRms = 3000;
	FLVFUN(GridVoltRms_F, GridVoltRms, VOLT);

	LoadVoltRms = (VoltOutA_rms + VoltOutB_rms + VoltOutC_rms) * S1DIV3;
	if(LoadVoltRms > 10000)
		LoadVoltRms = 10000;
	FLVFUN(LoadVoltRms_F, LoadVoltRms, LOADCURR);

	GridCurRms = (gridCurA_rms + gridCurB_rms + gridCurC_rms) * S1DIV3;
	if(GridCurRms > 10000)
		GridCurRms = 10000;
	FLVFUN(GridCurRms_F, GridCurRms, GRIDCURR);

	GridCurBYRms = (gridCurrBYAF_rms + gridCurrBYBF_rms + gridCurrBYCF_rms) * S1DIV3;
	if(GridCurBYRms > 10000)
		GridCurBYRms = 10000;
	FLVFUN(GridCurBYRms_F, GridCurBYRms, GRIDCURR);
#elif ESC_SINGLEPHASECTRL
    GridVoltRms = VoltInA_rms;
    if(GridVoltRms>3000)            GridVoltRms=3000;
    FLVFUN(GridVoltRms_F,GridVoltRms,VOLT);

    LoadVoltRms = VoltOutA_rms;
    if(LoadVoltRms>10000)           LoadVoltRms=10000;
    FLVFUN(LoadVoltRms_F,LoadVoltRms,LOADCURR);

    GridCurRms = gridCurA_rms;
    if(GridCurRms>10000)            GridCurRms=10000;
    FLVFUN(GridCurRms_F,GridCurRms,GRIDCURR);

    GridCurBYRms = gridCurrBYAF_rms;
    if(GridCurBYRms>10000)          GridCurBYRms=10000;
    FLVFUN(GridCurBYRms_F,GridCurBYRms,GRIDCURR);
#endif
}
void GetCapaReactCorr(void)
{
	if(StateFlag.reactPrCompFlag){
		CapaReactCorr	=CurrLimitPri(reactPowerCompCurQ*S1DIVSQRT3);		//�õ�����Q����޷�ϵ��(�޹����������)
	}else{
		CapaReactCorr=0;													//�ر��޹�����,���޹�ϵ��Ϊ��
	}
}

void GetPhCaReactCorr(void)
{
	WholeRmsLimitPH = WholeRmsLimit;
	if(StateFlag.reactPrCompFlag){
		CapaReactCorrA = CurrLimitPriPH(ReactPowerCurQPA);
		CapaReactCorrB = CurrLimitPriPH(ReactPowerCurQPB);
		CapaReactCorrC = CurrLimitPriPH(ReactPowerCurQPC);
	}else{
		CapaReactCorrA=CapaReactCorrB=CapaReactCorrC=0;
	}
}

void GetHarmLimitCorr(void)
{
//	if(StateFlag.harmCompEn){												//г����������
//		harmSum	=Max3(HarmSumA,HarmSumB,HarmSumC)*(1/SQRT2);				//ȡ����г���е����ֵ
//		float32 harmLimtCorr	=CurrLimitPri(harmSum);								//г�����ߵ����޷�ϵ��
//		float32 harmZLimtCorr	=CurrLimitZPri(CurrHarmRefZ);						//г�����ߵ����޷�ϵ��
//		if(StateFlag.isHarmCompensateMode)									//�ǲ�г��ģʽ
//			HarmLimtCorr=harmLimtCorr*harmZLimtCorr*harmCompPerc;			//г��ϵ��=����ϵ��*����ϵ��*����г��ϵ��
//		else
//			HarmLimtCorr=harmZLimtCorr*harmCompPerc;						//г��ϵ��=����ϵ��*����ϵ��
//	}else{
//		HarmLimtCorr=0;														//�ر�г������,��г��ϵ������
//	}
}

void GetCapaUnbalCorr(void)
{
	if(StateFlag.negCurCompFlag){											//��ƽ�ⲹ������
		curFundNg=Max3(CurFundNgARms,CurFundNgBRms,CurFundNgCRms);			//ȡ���������ֵ
		float32 capaUnbalCorr	=CurrLimitPri(curFundNg);							//��ƽ��������ߵ����޷�ϵ��
		float32 capaUnbalZCorr	=CurrLimitZPri(LoadFundaCurZRms);					//��ƽ��������ߵ����޷�ϵ��
		CapaUnbalCorr =capaUnbalCorr*capaUnbalZCorr;										//��ƽ��ϵ��=���ߵ����޷�ϵ��*���ߵ����޷�ϵ��
	}else{
		CapaUnbalCorr=0;
	}
}

void RMSLimit(void)
{
	static int16 count=0;

	WholeRmsLimit	=WholeOutCurRmsLimit;							//Saving computation, this variable is defined as RMS*RMS
//	WholeZRmsLimit	=NeutralCurLimit;

	//�ڴ˼����ֵ����װ��Ҫ�������Чֵ����Ϊ��׼.����һ��ģ���������ϵ��ʵ�ֵ����Զ��޷�
	if( ((StateEventFlag_A == STATE_EVENT_RUN_A)||(StateEventFlag_A == STATE_EVENT_WAIT_A))|| \
	    ((StateEventFlag_B == STATE_EVENT_RUN_B)||(StateEventFlag_B == STATE_EVENT_WAIT_B))|| \
	    ((StateEventFlag_C == STATE_EVENT_RUN_C)||(StateEventFlag_C == STATE_EVENT_WAIT_C)) ){
		switch(StateFlag.prioritizedModeFlag){
		case 4:	//���ٲ���(��δʵ��)
		case 5:	//�Զ�����(��δʵ��)
		case 0:	//Ĭ��ģʽ(��ǰ�汾����)
		case 1:   // �޹�����
			CurrRefDCorr	=CurrLimitPri(currentRefD*S1DIVSQRT3);					//�õ�����D����޷�ϵ��(ֱ����ѹ������)
			GetPhCaReactCorr();
			GetCapaReactCorr();
			GetHarmLimitCorr();
			GetCapaUnbalCorr();
			break;

		case 2:  // г������
			CurrRefDCorr	=CurrLimitPri(currentRefD*S1DIVSQRT3);
			GetHarmLimitCorr();
			GetCapaUnbalCorr();
			GetCapaReactCorr();
			break;

		case 3: // ��ƽ������
			CurrRefDCorr	=CurrLimitPri(currentRefD*S1DIVSQRT3);
			GetCapaUnbalCorr();
			GetHarmLimitCorr();
			GetCapaReactCorr();
			break;
		default:break;
		}
	}else{
		CapaUnbalCorr=1;
		HarmLimtCorr=1;
		CapaReactCorr=1;
	}

	if(CapaUnbalCorr>2) CapaUnbalCorr=2.0f;	if(CapaUnbalCorr<-2) CapaUnbalCorr=-2.0f;
	if(HarmLimtCorr>2)  HarmLimtCorr =2.0f;	if(HarmLimtCorr <-2) HarmLimtCorr =-2.0f;
	if(CapaReactCorr>2) CapaReactCorr=2.0f;	if(CapaReactCorr<-2) CapaReactCorr=-2.0f;
	if(count++>1600){		//0.5sˢ��һ��
		count=0;
		storageUsed=100*(sqrtf(WholeOutCurRmsLimit)-sqrtf(WholeRmsLimit))/sqrtf(WholeOutCurRmsLimit);
	}
}

void ESCfilterMemCopy(Uint16 num)
{
    int16 i;
    if(num > 2)    num = 0;
    for(i=0;i<3;i++){
        ConstantCurr[i].VolttargetCorr = ConstantCurrDefault[num].VolttargetCorr;
        ConstantCurr[i].RMS_CONSTANT_CURRENT_OVERLOAD = ConstantCurrDefault[num].RMS_CONSTANT_CURRENT_OVERLOAD;
        ConstantCurr[i].RMS_CONSTANT_CURRENT_DIFF = ConstantCurrDefault[num].RMS_CONSTANT_CURRENT_DIFF;
        ConstantCurr[i].RMS_CONSTANT_CURRENT_RATED = ConstantCurrDefault[num].RMS_CONSTANT_CURRENT_RATED;
        ConstantCurr[i].INS_CONSTANT_CURRENT_RATED = ConstantCurrDefault[num].INS_CONSTANT_CURRENT_RATED;

    }
}


float RunPIRE(PIcalc *p, float rk, float yk)
{
	float err = rk - yk;
	p->i6 += (err - p->i10) * p->Kp + err * p->Ki;
	p->i10 = err;
	if(p->i6>p->Umax)
		p->i6 = p->Umax;
	if(p->i6<p->Umin)
		p->i6 = p->Umin;
	return p->i6;
}


/*void VoltageCompensation(void)    PI����
{
    if(StateEventFlag == STATE_EVENT_RUN)
    {
        if(UserSetting.WordMode.B.VoltageMode!=0)
        {
            if(StateEventFlag != STATE_EVENT_RUN)           CntSec.VolComDelay = 0;
            if(CntSec.VolComDelay>3)						StateFlag.StabilizeFlag = 1;		//�ȶ�ʱ��														//�����ȶ���־
            else											StateFlag.StabilizeFlag = 0;

    //		if(LoadVoltU_rms<VolUpCo && LoadVoltU_rms>VolDownCo)
    //		if(GridVoltRms<VolUpCo&&GridVoltRms>VolDownCo)	CntMs.VolSurDelay = 0;

            if((CntMs.VolSurDelay>VolSurTime)||(UserSetting.WordMode.B.VoltageMode==2))
                StateFlag.VolSurTimeFlag = 1;		//���ѹ����

            if(StateFlag.StabilizeFlag && StateFlag.VolSurTimeFlag){

//                if(LoadVoltU_rms     > VolUpSurCo)				  VolCorU = VolUpSurCo;	//ѡ���ѹĿ��ֵ
//                else if(LoadVoltU_rms< VolDownSurCo)			      VolCorU = VolDownSurCo;
//                else				                                  VolCorU = LoadVoltU_rms;
//
//                if(LoadVoltV_rms     > VolUpSurCo)                  VolCorV = VolUpSurCo;   //ѡ���ѹĿ��ֵ
//                else if(LoadVoltV_rms< VolDownSurCo)                VolCorV = VolDownSurCo;
//                else                                                VolCorV = LoadVoltV_rms;
//
//                if(LoadVoltW_rms     > VolUpSurCo)                  VolCorW = VolUpSurCo;   //ѡ���ѹĿ��ֵ
//                else if(LoadVoltW_rms< VolDownSurCo)                VolCorW = VolDownSurCo;
//                else                                                VolCorW = LoadVoltW_rms;
//
//                if((LoadVoltU_rms>(VolCorU-1)) && (LoadVoltU_rms<(VolCorU+1)))  CntMs.VolSurCountU = 0;
//                if((LoadVoltV_rms>(VolCorV-1)) && (LoadVoltV_rms<(VolCorV+1)))  CntMs.VolSurCountV = 0;
//                if((LoadVoltW_rms>(VolCorW-1)) && (LoadVoltW_rms<(VolCorW+1)))  CntMs.VolSurCountW = 0;

                    VolPIOutPwmVa = DCL_runPI(&PIVolA,TargetValue,VoltOutA_rms);
                    VolPIOutPwmVb = DCL_runPI(&PIVolB,TargetValue,VoltOutB_rms);
                    VolPIOutPwmVc = DCL_runPI(&PIVolC,TargetValue,VoltOutC_rms);
            }
        }else{
            StateFlag.VolSurTimeFlag = 0;
            VolPIOutPwmVa =VolPIOutPwmVb =VolPIOutPwmVc = 1;//����������,δ��������,Ӧ��1��Ҫ����ռ�ձ�
        }
    }
}*/

// �޹�ָ���·����㷽��
// ���ܣ�������λ���·����޹�ֵ���ߺ㹦������ֵ������Ӧ��������޹�Qֵ��
// 1)  P = 1/sqrt(3)*(Vd*Id + Vq*Iq + 2*V0*I0)
// 2)  Q = 1/sqrt(3)*(Vq*Id - Vd*Iq)
// ���⣬�ؼ����������ڣ�����õ����޹��������ڿ��ƻ��ϵ����Ժ͸��� δ֪�������Ҫ���������ǵİ����ˡ�
// �����ͺ��ѹ����õ�������Q��Ϊ��ֵ�������õ����޹�����Ϊ��ֵ������sinFaiҲ�Ǹ��ġ�
// �޹����ʸ���Ϊ��ֵ�����ԡ���ֵ�����ԡ����޹�  constantQFlag = 1��reactPowerGiven = ��kvar��
//                                 �㹦������ constantQFlag = 0��constantCosFai = (-1~+1)
// �����ܳ�����㷽�� 0922
// �����˲����DQ���ģֵ��������ѹ��Чֵ�����ص�����Чֵ�����������Чֵ
// �����޹����ʺ͹��������ķ����������˲������ֵ�����й�P���޹�Q��ÿ�μ�����˲�6���ܲ�
// �ټ������ڹ���  S^2 = P^2 + Q^2
// cos = P*1000/Q
// ���QΪ��ֵ����������Ϊ����
void ReactivePowerGiven(void)
{
	// ���л��������߷�ʽΪ�ӵ��������豸��Ϊ������ô�����任�����Ϊ����
	// ��ֹ������Ϊ���������ո��غͲ�����������Ϊ������
	// ����-10 < ref + iqFilter_F < 0
	// ********************* �޹��·� **********************
	// ���޹�ģʽ�£�����Q����� Iq = (Vq*Id - 1.731789*Q)/Vd
	// Q = SQRT3* U*I  -> I = Q/SQRT3 /U
	// U�ߵ�ѹ��Чֵ    IΪ�������Чֵ
	// ����Vd��Vq��Ϊ���ѹ�ķ�ֵ*1.22��������Ҫ�任Ϊ�ߵ�ѹ��ֵ*1.22
	// ��ѹ����ϵ��1*SQRT3*SQRT3/SQRT2 = 2.2132
	// ��������ϵ��1*SQRT2*SQRT3/SQRT2 = 1.732
	if(StateFlag.reactPrCompFlag&&(StateFlag.VolSurTimeFlag==0))
	{
		switch(StateFlag.constantQFlag){
		case 0:	//���޹�����
			if(reactPowerGiven > reactPowGivenLimit*SQRT3)  reactPowerGiven = reactPowGivenLimit*SQRT3;   //  ������100Kvar�� ��λ���·���λӦ��ΪKvar
			if(reactPowerGiven < -reactPowGivenLimit*SQRT3) reactPowerGiven = -reactPowGivenLimit*SQRT3;
			// Vq*Id - Q = Vd*Iq
			//reactPowerCompCurQ =(GridFundaVoltQ*LoadFundaCurD* -reactPowerGiven *1000.0f*MU_MultRatio)/GridFundaVoltD;   // Q/SQRT3��λ���㣬������Var;
			reactPowerCompCurQ =(reactPowerGiven *MU_MultRatio*1000.0f)/GridFundaVoltD;   // Q/SQRT3��λ���㣬������Var;
			ReactPowerCurQPA = reactPowerCompCurQ*S1DIVSQRT3;
			ReactPowerCurQPB = ReactPowerCurQPC = ReactPowerCurQPA;
			break;
		case 1:	//���޹�����
				ReactPowerCurQPA = restantReactCurrent*reactPrCompPerc*(MU_MultRatio);   //�������ԣ����Ǹ���
				ReactPowerCurQPB = ReactPowerCurQPC = ReactPowerCurQPA;
				reactPowerCompCurQ = ReactPowerCurQPA*SQRT3;   						 //�������ԣ����Ǹ���
				if(StateFlag.ManualUbanCurFlag==1){   //�ֶ�ģʽ����ƽ���蹴ѡ�޹��������޹�����ģʽ
					ReactPowerCurQPA = manualubanlanccurA;
					ReactPowerCurQPB = manualubanlanccurB;
					ReactPowerCurQPC = manualubanlanccurC;
				}
			break;
		case 2:	//�㹦������
			// �㹦������ģʽ�£������޹�����Q
			// ���㷽�������ݸ����й������������ڹ��ʣ������޹�
			// S = P/cos(fai)   Q = sqrt(S^2 - P^2)
			//	if(constantCosFai >=1)		constantCosFai = 1;
			//	if(constantCosFai <= -1)	constantCosFai = -1;
	//		if(LoadFundaCurQ>2)		//cos(pha)=0.99; sin(pha)*S(30A)=0.141;S*0.141=4.23; 4.23*SQRT2*2=12.0A
	//			if(GeneratorStationFlag==1)	//����վģʽ
	//				reactPowerCompCurQ = LoadFundaCurQ*constantCosFai*(MU_MultRatio);   		// ��⵽�����޹�,����վģʽ�Ƿ���.��Ϊ������
	//			else						//����ģʽ
	//				reactPowerCompCurQ = -LoadFundaCurQ*constantCosFai*(MU_MultRatio);   		// �����޹�
	//
	//		else if(LoadFundaCurQ<-2)	//�ͻ�����
	//			if(GeneratorStationFlag==1)	//����վģʽ
	//				reactPowerCompCurQ = LoadFundaCurQ*(2-constantCosFai)*(MU_MultRatio);		// �����޹�
	//			else
	//				reactPowerCompCurQ = -LoadFundaCurQ*(2-constantCosFai)*(MU_MultRatio);		// �����޹�
	//		else ;	//ά�ֲ���.��ֹ�ڹ�������1�����л�
	//		reactPowerCompCurQ = -LoadFundaCurQ*abs(constantCosFai)*(MU_MultRatio);   		//
				if(LoadFundaCurQ>2){
					reactPowerCompCurQ = -LoadFundaCurQ*constantCosFai*reactPrCompPerc*(MU_MultRatio);   		// �����޹�
					ReactPowerCurQPA=-(LoadRealCurQA-LoadRealCurDA*PFConsin)*reactPrCompPerc*MU_MultRatio;
					ReactPowerCurQPB=-(LoadRealCurQB-LoadRealCurDB*PFConsin)*reactPrCompPerc*MU_MultRatio;
					ReactPowerCurQPC=-(LoadRealCurQC-LoadRealCurDC*PFConsin)*reactPrCompPerc*MU_MultRatio;
					CapreactPowerCurQ = Max3(ReactPowerCurQPA,ReactPowerCurQPB,ReactPowerCurQPC)*SQRT3;
					if(ReactPowerCurQPA>0)	ReactPowerCurQPA = 0;
					if(ReactPowerCurQPB>0)	ReactPowerCurQPB = 0;
					if(ReactPowerCurQPC>0)	ReactPowerCurQPC = 0;
				}else if(LoadFundaCurQ<-2){
	//				PFConsin = -PFConsin;
					reactPowerCompCurQ = -LoadFundaCurQ*reactPrCompPerc*(MU_MultRatio);	// �����޹�
					ReactPowerCurQPA = -LoadRealCurQA*reactPrCompPerc*(MU_MultRatio);
					ReactPowerCurQPB = -LoadRealCurQB*reactPrCompPerc*(MU_MultRatio);
					ReactPowerCurQPC = -LoadRealCurQC*reactPrCompPerc*(MU_MultRatio);
					CapreactPowerCurQ = reactPowerCompCurQ;
				}else{
					ReactPowerCurQPA = ReactPowerCurQPB = ReactPowerCurQPC = 0;
					CapreactPowerCurQ = reactPowerCompCurQ = 0;
				}
				break;
			default:break;
		}
	}else{
		ReactPowerCurQPA = ReactPowerCurQPB = ReactPowerCurQPC = 0;
		CapreactPowerCurQ = reactPowerCompCurQ = 0;
	}
}

float CurrLimitPri(float in){
	float capTmp=(in)*(in);
	float corr=1.0f;
	if( (WholeRmsLimit)>=(capTmp) ){
		WholeRmsLimit -= capTmp;
	}else{
		(corr)=sqrtf(WholeRmsLimit/capTmp);
		(WholeRmsLimit)=0;
	}
	return corr;
}
float CurrLimitPriPH(float in){
	float capTmp=(in)*(in);
	float corr=1.0f;
	if( (WholeRmsLimitPH)>=(capTmp) ){
		;
	}else{
		(corr)=sqrtf(WholeRmsLimitPH/capTmp);
	}
	return corr;
}

float CurrLimitZPri(float in){
	float capTmp=(in)*(in);
	float corr=1.0f;
	if( (WholeZRmsLimit)>=(capTmp) ){
		(WholeZRmsLimit)-= (capTmp);
	}else{
		(corr)=sqrtf(WholeZRmsLimit/capTmp);
		(WholeZRmsLimit)=0;
	}
	return corr;
}

void RMSAllInit(void)
{
    Uint16 *pInitVar,i;

    pInitVar = (Uint16 *)(&RmsSumIn);
    for(i=0;i<sizeof(RmsSumIn);i++)
        *pInitVar++ = 0;

    pInitVar = (Uint16 *)(&RmsSumOut);
     for(i=0;i<sizeof(RmsSumOut);i++)
         *pInitVar++ = 0;

     pInitVar = (Uint16 *)(&RmsSumQ);
      for(i=0;i<sizeof(RmsSumQ);i++)
          *pInitVar++ = 0;
}


// ��ʼ������ʹ�ã���������ʱ����������
// ������Ҫ�ۼӵ����ݣ����Ǳ��滻�������ݣ����磺
// BW�˲����������˲�������ֵ�˲�����һ�׵�ͨ�˲���
void InitParameters(void)
{
	int16 i,j;

	PWM_PERIOD_MAX = T1PR+5;
	PWM_PERIOD_MIN = T1PR-5;
	for(i=0;i<3*2;i++)
		for(j=0;j<CALIBRATION_COEFF_LEN+1;j++){
            IFFTData[i][j]=0;
	        InvFFTData[i][j]=0;
		}
	for(i=0;i<GENERATOR_COEFF_LEN;i++){
		LimitVal[0][i]=1.0f;
		LimitVal[1][i]=1.0f;
		LimitVal[2][i]=1.0f;
		LimitDly[0][i]=0;
		LimitDly[1][i]=0;
		LimitDly[2][i]=0;
	}

	CtrlVoltA=CtrlVoltB=CtrlVoltC=0;
	CurFundA=CurFundB=CurFundC=0;
	CurrHarmRefA=CurrHarmRefB=CurrHarmRefC=0;
	FundwaveA=FundwaveB=FundwaveC=0;
	CurrRefA=CurrRefB=CurrRefC=0;
	CurFundNgA=CurFundNgB=CurFundNgC=0;
	initStartVar();
}

void MCUfilterMemCopy(DF22 *dst,int16 num)
{
    int16 i;

    Uint16 *pInitVar    =(Uint16 *)dst;
    Uint16 *pSrcVar     =(Uint16 *)(&AllFilterCoeff[T1PRPwmFrequency][num]);
     for(i=0;i<sizeof(UdcUpFilter);i++)
         *pInitVar++=*pSrcVar++;
}

#define FFTMAG_SEGMENT1 5
#define FFTMAG_SEGMENT2 10
#define FFTMAG_SEGMENT3 (CALIBRATION_COEFF_LEN - FFTMAG_SEGMENT1 - FFTMAG_SEGMENT2)

void InitMCUFilter(void) //ѡ������Ƶ��
{
    int i;
    MCUfilterMemCopy(&VolAndInvCurrFilter[0],           GRIDVOLT_DEFAULT_NUM);
    MCUfilterMemCopy(&VolAndInvCurrFilter[1],           GRIDVOLT_DEFAULT_NUM);
    MCUfilterMemCopy(&VolAndInvCurrFilter[2],           GRIDVOLT_DEFAULT_NUM);
    MCUfilterMemCopy(&VolAndInvCurrFilter[3],           GRIDVOLT_DEFAULT_NUM);
    MCUfilterMemCopy(&VolAndInvCurrFilter[4],           GRIDVOLT_DEFAULT_NUM);
    MCUfilterMemCopy(&VolAndInvCurrFilter[5],           GRIDVOLT_DEFAULT_NUM);
    MCUfilterMemCopy(&VolAndInvCurrFilter[6],           APFOUT1_DEFAULT_NUM);
    MCUfilterMemCopy(&VolAndInvCurrFilter[7],           APFOUT1_DEFAULT_NUM);
    MCUfilterMemCopy(&VolAndInvCurrFilter[8],           APFOUT1_DEFAULT_NUM);
    MCUfilterMemCopy(&VolAndInvCurrFilter[9],           APFOUT1_DEFAULT_NUM);
    MCUfilterMemCopy(&VolAndInvCurrFilter[10],          APFOUT1_DEFAULT_NUM);
    MCUfilterMemCopy(&VolAndInvCurrFilter[11],          APFOUT1_DEFAULT_NUM);
    MCUfilterMemCopy(&VolAndInvCurrFilter[12],          UDC_PNF_DEFAULT_NUM);
    MCUfilterMemCopy(&VolAndInvCurrFilter[13],          UDC_PNF_DEFAULT_NUM);
    MCUfilterMemCopy(&VolAndInvCurrFilter[14],          UDC_PNF_DEFAULT_NUM);
    MCUfilterMemCopy(&ResonancePortectABC[0],           RESONANCE_PORTECT_ABC_NUM);
    MCUfilterMemCopy(&ResonancePortectABC[1],           RESONANCE_PORTECT_ABC_NUM);
    MCUfilterMemCopy(&UdcUpFilter,                      UDC_PNF_DEFAULT_NUM);
    MCUfilterMemCopy(&UdcDnFilter,                      UDC_PNF_DEFAULT_NUM);
    MCUfilterMemCopy(&bwForFreq,                        BWFILTER_FREQ_DEFAULTS_NUM);

    for(i=0;i<FFTMAG_SEGMENT1;i++)
    {
        MCUfilterMemCopy(&fftmag[0][i],              FFT_MAG1_ALL_DEFAULT_NUM);
        MCUfilterMemCopy(&fftmag[1][i],              FFT_MAG1_ALL_DEFAULT_NUM);
        MCUfilterMemCopy(&fftmag[2][i],              FFT_MAG1_ALL_DEFAULT_NUM);
        MCUfilterMemCopy(&fftpha[0][i],              FFT_PHA1_ALL_DEFAULT_NUM);
        MCUfilterMemCopy(&fftpha[1][i],              FFT_PHA1_ALL_DEFAULT_NUM);
        MCUfilterMemCopy(&fftpha[2][i],              FFT_PHA1_ALL_DEFAULT_NUM);
    }

    for(i=FFTMAG_SEGMENT1;i<FFTMAG_SEGMENT2;i++)
    {
        MCUfilterMemCopy(&fftmag[0][i],              FFT_MAG2_ALL_DEFAULT_NUM);
        MCUfilterMemCopy(&fftmag[1][i],              FFT_MAG2_ALL_DEFAULT_NUM);
        MCUfilterMemCopy(&fftmag[2][i],              FFT_MAG2_ALL_DEFAULT_NUM);
        MCUfilterMemCopy(&fftpha[0][i],              FFT_PHA2_ALL_DEFAULT_NUM);
        MCUfilterMemCopy(&fftpha[1][i],              FFT_PHA2_ALL_DEFAULT_NUM);
        MCUfilterMemCopy(&fftpha[2][i],              FFT_PHA2_ALL_DEFAULT_NUM);
    }

    for(i=FFTMAG_SEGMENT2;i<FFTMAG_SEGMENT3;i++)
    {
        MCUfilterMemCopy(&fftmag[0][i],              FFT_MAG3_ALL_DEFAULT_NUM);
        MCUfilterMemCopy(&fftmag[1][i],              FFT_MAG3_ALL_DEFAULT_NUM);
        MCUfilterMemCopy(&fftmag[2][i],              FFT_MAG3_ALL_DEFAULT_NUM);
        MCUfilterMemCopy(&fftpha[0][i],              FFT_PHA3_ALL_DEFAULT_NUM);
        MCUfilterMemCopy(&fftpha[1][i],              FFT_PHA3_ALL_DEFAULT_NUM);
        MCUfilterMemCopy(&fftpha[2][i],              FFT_PHA3_ALL_DEFAULT_NUM);
    }
    //    MCUfilterMemCopy(&gridVoltFilterLowA,           GRID_VOLT_LOWDEFAULT_NUM);
    //    MCUfilterMemCopy(&gridVoltFilterLowB,           GRID_VOLT_LOWDEFAULT_NUM);
    //    MCUfilterMemCopy(&gridVoltFilterLowC,           GRID_VOLT_LOWDEFAULT_NUM);
    //    MCUfilterMemCopy(&PLLAGCFilter,                 PLL_AGC_FILTER_DEFAULT_NUM);
    //    MCUfilterMemCopy(&gridVoltPosiDFilter,          PLL_VOLT_Q_DEFAULT_NUM);
    //    MCUfilterMemCopy(&gridVoltPosiQFilter,          PLL_VOLT_Q_DEFAULT_NUM);
    //    MCUfilterMemCopy(&gridVoltNegDFilter,           PLL_VOLT_Q_DEFAULT_NUM);
    //    MCUfilterMemCopy(&gridVoltNegQFilter,           PLL_VOLT_Q_DEFAULT_NUM);
    //    MCUfilterMemCopy(&RectPowerDispFilter,          RECTPOWER_DISP_DEFAULT_NUM);
    //    MCUfilterMemCopy(&ActivePowerDispFilter,        RECTPOWER_DISP_DEFAULT_NUM);
}

void CLAfilterMemCopy(DCL_DF22_CLA *dst,int16 num)
{
//    int16 i;
//
//    Uint32 *pInitVar    =(Uint32 *)(&ApfOutA);
//    Uint32 *pSrcVar     =(Uint32 *)(&CLAFilterCoeff[T1PRPwmFrequency][num]);
//    for(i=0;i<sizeof(ApfOutA);i++)
//        *pInitVar=*pSrcVar;
}

void InitCLAFilter(void)
{
}

void InitAfterReadROM(void)
{
    InitCLAFilter();
    InitMCUFilter();
    StateFlag.RecordWritting=1;
//    cbInit(&FlashWaveFifo,FLASHWAVE_FIFO_NUM);
}

void InitBeforeReadROM(void)
{
	int16 i;
	Uint16 *pInitVar;

	pInitVar = (Uint16 *)(&StateFlag);
	for(i=0;i<sizeof(StateFlag);i++)
		*pInitVar++ = 0;

	pInitVar = (Uint16 *)(tmpOffsetValue);
	for(i=0;i<sizeof(tmpOffsetValue);i++)
		*pInitVar++ = 0;

	pInitVar = (Uint16 *)(FlvSlot);
	for(i=0;i<sizeof(FlvSlot);i++)
		*pInitVar++ = 0;

	pInitVar = (Uint16 *)(&CntMs);
	for(i=0;i<sizeof(CntMs);i++)
		*pInitVar++ = 0;

	pInitVar = (Uint16 *)(&CntSec);
	for(i=0;i<sizeof(CntSec);i++)
		*pInitVar++ = 0;

    pInitVar = (Uint16 *)(&ESCFlagA);
    for(i=0;i<sizeof(ESCFlagA);i++)
        *pInitVar++ = 0;

    pInitVar = (Uint16 *)(&ESCFlagB);
    for(i=0;i<sizeof(ESCFlagB);i++)
        *pInitVar++ = 0;

    pInitVar = (Uint16 *)(&ESCFlagC);
    for(i=0;i<sizeof(ESCFlagC);i++)
        *pInitVar++ = 0;

	pInitVar = (Uint16 *)HarmTHD;
	for(i=0;i<sizeof(HarmTHD);i++)
		*pInitVar++=0;

	pInitVar = (Uint16 *)(&CntFaultDelay);
	for(i=0;i<sizeof(CntFaultDelay);i++)
		*pInitVar++ = 0;

	pInitVar = (Uint16 *)(&CntFaultDelayLong);
	for(i=0;i<sizeof(CntFaultDelayLong);i++)
		*pInitVar++ = 0;

	pInitVar = (Uint16 *)(&SyncInformation);
	for(i=0;i<sizeof(SyncInformation);i++)
		*pInitVar++ = 0;

	pInitVar = (Uint16 *)&IncidentRecord;
	for(i=0;i<sizeof(IncidentRecord);i++)
		*pInitVar++ = 0;

    pInitVar = (Uint16 *)(&VoltSlid);
    for(i=0;i<sizeof(VoltSlid);i++)
        *pInitVar++ = 0;

    pInitVar = (Uint16 *)(&VoltSlidA);
	for(i=0;i<sizeof(VoltSlidA);i++)
		*pInitVar++ = 0;

	pInitVar = (Uint16 *)(&VoltSlidB);
	for(i=0;i<sizeof(VoltSlidB);i++)
		*pInitVar++ = 0;

	pInitVar = (Uint16 *)(&VoltSlidC);
	for(i=0;i<sizeof(VoltSlidC);i++)
        *pInitVar++ = 0;

    pInitVar = (Uint16 *)(&GridCurrSlidA);
    for(i=0;i<sizeof(GridCurrSlidA);i++)
        *pInitVar++ = 0;

    pInitVar = (Uint16 *)(&GridCurrSlidB);
    for(i=0;i<sizeof(GridCurrSlidA);i++)
        *pInitVar++ = 0;

    pInitVar = (Uint16 *)(&GridCurrSlidC);
    for(i=0;i<sizeof(GridCurrSlidA);i++)
        *pInitVar++ = 0;

    pInitVar = (Uint16 *)(&RmsSumIn);
    for(i=0;i<sizeof(RmsSumIn);i++)
        *pInitVar++ = 0;

    pInitVar = (Uint16 *)(&RmsSumOut);
    for(i=0;i<sizeof(RmsSumOut);i++)
        *pInitVar++ = 0;

    pInitVar = (Uint16 *)(&RmsSumQ);
    for(i=0;i<sizeof(RmsSumQ);i++)
        *pInitVar++ = 0;

    pInitVar = (Uint16 *)(&DQbufIn);
    for(i=0;i<sizeof(DQbufIn);i++)
        *pInitVar++ = 0;

//	pInitVar = (Uint16 *)&GcurSlidA[0];
//	for(i=0;i<sizeof(GcurSlidA);i++)
//		*pInitVar++ = 0;
//
//	pInitVar = (Uint16 *)&GcurSlidB[0];
//	for(i=0;i<sizeof(GcurSlidB);i++)
//		*pInitVar++ = 0;
//
//	pInitVar = (Uint16 *)&GcurSlidC[0];
//	for(i=0;i<sizeof(GcurSlidC);i++)
//		*pInitVar++ = 0;
//
//	pInitVar = (Uint16 *)&LoadSlidA[0];
//	for(i=0;i<sizeof(LoadSlidA);i++)
//		*pInitVar++ = 0;
//
//	pInitVar = (Uint16 *)&LoadSlidB[0];
//	for(i=0;i<sizeof(LoadSlidB);i++)
//		*pInitVar++ = 0;
//
//	pInitVar = (Uint16 *)&LoadSlidC[0];
//	for(i=0;i<sizeof(LoadSlidC);i++)
//		*pInitVar++ = 0;

	pInitVar = (Uint16 *)(&harmCompPercParameter);
	for(i=0;i<sizeof(harmCompPercParameter);i++)
//		*pInitVar++ = 10000;
		*pInitVar++ = 0;

	pInitVar = (Uint16 *)(&Harmonic);
	for(i=0;i<sizeof(Harmonic);i++)
		*pInitVar++ = 0;

	pInitVar = (Uint16 *)(&SOE);
	for(i=0;i<sizeof(SOE);i++)
		*pInitVar++ = 0;

    pInitVar = (Uint16 *)(&Modbuff[0][0]);
    for(i=0;i<sizeof(Modbuff[0][0]);i++)
        *pInitVar++ = 0;

    pInitVar = (Uint16 *)(&Modbuff[1][0]);
    for(i=0;i<sizeof(Modbuff[1][0]);i++)
        *pInitVar++ = 0;

    pInitVar = (Uint16 *)(&Modbuff[2][0]);
    for(i=0;i<sizeof(Modbuff[2][0]);i++)
        *pInitVar++ = 0;

    pInitVar = (Uint16 *)(&RecordFlash);
    for(i=0;i<sizeof(RecordFlash);i++)
        *pInitVar++ = 0;

    pInitVar = (Uint16 *)&ErrorRecord;
    for(i=0;i<sizeof(ErrorRecord);i++)
        *pInitVar++ = 0;

    pInitVar = (Uint16 *)(TestLoadCur);
    for(i=0;i<sizeof(TestLoadCur);i++)
        *pInitVar++ = 0;


//	pInitVar = (Uint16 *)(&GenFFTCoeff);
//	for(i=0;i<sizeof(GenFFTCoeff);i++)
//		*pInitVar++ = 0;
    Information_Structure.Correction.all = 0;
	RMSAllInit();
//	MeanAllInit();
    cbInit(&BlockFifo,FIFO_DQ_BLOCK_NUM);
    varMean_initialize();
	FM_Write_Status(UNLOCK);				//StateFlag��ʼ�������
    FM_DatWordWrite(0x1FFE,63);
    FM_DatWordWrite(0x7FFE,255);
    Uint16 TypeJudgment1,TypeJudgment2;
    TypeJudgment1 = FM_DatReadWord(0x1FFE);
    TypeJudgment2 = FM_DatReadWord(0x7FFE);
    if((TypeJudgment1==63)&&(TypeJudgment2==255))
    {
        StorageTypeFlag = 1;
//        FM_DatRead16(WAVE_RECORD_START_ADDR,sizeof(debugW),&debugW[0][0]);
    }

	SaveDataEead();
	SetResetExecute();  					//Fault flag reset
	StateFlag.onceTimeAdcAutoAdjust = 1;	//�ϵ�,��ƫУ׼һ��
	ESCFlagA.autoStFlag = ORIGINAL_STATE_A;  //�ϵ�,��λ������״̬��
	ESCFlagB.autoStFlag = ORIGINAL_STATE_B;
	ESCFlagC.autoStFlag = ORIGINAL_STATE_C;
	StateEventFlag_A = STATE_EVENT_STANDBY_A;   //�ϵ�,��λ״̬��
	StateEventFlag_B = STATE_EVENT_STANDBY_B;
	StateEventFlag_C = STATE_EVENT_STANDBY_C;
	ESCFlagA.FaultJudgeFlag = 0;  //�ϵ��ʼ��Ϊ0
	ESCFlagB.FaultJudgeFlag = 0;
	ESCFlagC.FaultJudgeFlag = 0;
	StateFlag.ManufacturerParameters = true;
    CurrThisPos = 0;
    CntSec.StopDelay = 0;
    ESCFlagA.BYFEEDBACKFLAG = 1;
    ESCFlagB.BYFEEDBACKFLAG = 1;
    ESCFlagC.BYFEEDBACKFLAG = 1;

    ESCFlagA.onceRunStartFlag = 0;
    ESCFlagB.onceRunStartFlag = 0;
    ESCFlagC.onceRunStartFlag = 0;

    ESCFlagA.onceTimeStateMachine = 1;     //�ϵ�,��ʱ200ms��Ϊ��ͨ��բ��״̬
    ESCFlagB.onceTimeStateMachine = 1;
    ESCFlagC.onceTimeStateMachine = 1;
    ESCFlagA.ESCCntSec.PRECHARGEDelayBY = 0;//��ֹ�ϵ������乤������·�ű���������ը��,��λ��ʼ��
    ESCFlagB.ESCCntSec.PRECHARGEDelayBY = 0;
    ESCFlagC.ESCCntSec.PRECHARGEDelayBY = 0;
    ESCFlagA.ESCCntSec.PRECHARGEDelay = 0;//��ֹ�ϵ������乤���и�/��ѹ�ű���������ը��.��λ��ʼ��
    ESCFlagB.ESCCntSec.PRECHARGEDelay = 0;
    ESCFlagC.ESCCntSec.PRECHARGEDelay = 0;
    ESCFlagA.RELAYCONTROLFlag = 1;//�ϵ�̵������Ʊ�־λ,��λ��ʼ��
    ESCFlagB.RELAYCONTROLFlag = 1;
    ESCFlagC.RELAYCONTROLFlag = 1;
    ESCFlagA.FAULTCONFIRFlag = 0; //����ȷ������һ�෢������,��λ��ʼ��
    ESCFlagB.FAULTCONFIRFlag = 0;
    ESCFlagC.FAULTCONFIRFlag = 0;
    ESCHighLowRelayCNTA = 0;
    ESCHighLowRelayCNTB = 0;
    ESCHighLowRelayCNTC = 0;
    ESCSicFaultCNTA = 0;
    ESCSicFaultCNTB = 0;
    ESCSicFaultCNTC = 0;
    ESCBYRelayCNTA = 0;
    ESCBYRelayCNTB = 0;
    ESCBYRelayCNTC = 0;
    ESCFlagA.PHASE = 1;             //A��
    ESCFlagB.PHASE = 2;             //B��
    ESCFlagC.PHASE = 3;             //C��
    SET_POWER_CTRL(1);  //��ʼ���ϵ�,��15V����
    Delayus(TIME_WRITE_15VOLT_REDAY);

    CurrPrvPos= (MEANPOINT_QUARTER_NUM-MEANPOINT_QUARTER);
//    SET_FLback(0);//����debug
//    SET_BYPASS_FEEDBACK(0);//����debug
}

void clock1ms(void)
{
	ServiceDog();
	if (++Time.MicroSec >= 999)//1��
		Time.MicroSec = 0;

	if (SCIC.SendTrsformDelayTim)
	{
		if (--SCIC.SendTrsformDelayTim == 0)
			vMBPortSerialEnable(&SCIC, TRUE, FALSE);
	}

	if (SCIB.SendTrsformDelayTim)
	{
		if (--SCIB.SendTrsformDelayTim == 0)
			vMBPortSerialEnable(&SCIB, TRUE, FALSE);
	}

	if (SCID.SendTrsformDelayTim)
	{
		if (--SCID.SendTrsformDelayTim == 0)
			vMBPortSerialEnable(&SCID, TRUE, FALSE);
	}

	if (SCICMASTERENABLE == 1)
		ModBusMasterTimeCal(&SCIC);
	else
		ModBusSlaveTimeCal(&SCIC);

	ModBusSlaveTimeCal(&SCIB);

	if (SCIDMASTERENABLE == 1)
		ModBusMasterTimeCal(&SCID);
	else
		ModBusSlaveTimeCal(&SCID);
	if (++CapEmergTim >= 4500)
		CapEmergTim = 4500;
	if (++RandTimCount >= 60)
		RandTimCount = 60;
}

/*
 * ���ܣ����Ĳ�����WY
 *
 * ˵�����ú�����ִ������Ϊ5ms��
 */
void clock5ms(void)
{
	static int j = 2;
	int16 *pCnt;
	Uint16 i;

	pCnt = (int16*) &CntMs;
	for (i = sizeof(CntMs); i != 0; i--)
	{
		if ((*pCnt)++ > 30000)
			(*pCnt) = 30000;
		pCnt++;
	}

	pCnt = (int16*) &ESCFlagA.ESCCntMs;
	for (i = sizeof(ESCFlagA.ESCCntMs); i != 0; i--)
	{
		if ((*pCnt)++ > 30000)
			(*pCnt) = 30000;
		pCnt++;
	}

	pCnt = (int16*) &ESCFlagB.ESCCntMs;
	for (i = sizeof(ESCFlagB.ESCCntMs); i != 0; i--)
	{
		if ((*pCnt)++ > 30000)
			(*pCnt) = 30000;
		pCnt++;
	}

	pCnt = (int16*) &ESCFlagC.ESCCntMs;
	for (i = sizeof(ESCFlagC.ESCCntMs); i != 0; i--)
	{
		if ((*pCnt)++ > 30000)
			(*pCnt) = 30000;
		pCnt++;
	}

	if (j-- <= 0)
	{
		Event_post(Event_SetStartSM, Event_Id_01);
		Event_post(Event_CapSwitchSM, Event_Id_02);
		Event_post(Event_Outside, Event_Id_00);
		Event_post(Event_SCI_Reveived, Event_Id_00 | Event_Id_01 | Event_Id_02);
		j = 1;
	}
	MainContactConfirm(25); //�ú���δ���塣WY
}

void clock1Second(void)
{
	int16 *pCnt;
	Uint16 i;

	if (TimSwitchOnStart == 1)
	{
		if (++SwitchTimeOn > 600)
			SwitchTimeOn = 600;
	}
	if (TimSwitchOffStart == 1)
	{
		if (++SwitchTimeOff > 600)
			SwitchTimeOff = 600;
	}
	if (++FlashRecordTim > 10)
		StateFlag.RecordWritting = 1;
	if (++ReFFTDelayTim > 40)
		ReFFTDelayTim = 40;
	if (++CpuHeatDelayTime > 15)
		CpuHeatDelayTime = 15;

	pCnt = &CntSec.MeasureDelay;
	for (i = 0; i < sizeof(CntSec); i++)
	{
		if ((*pCnt)++ > 3600)
			(*pCnt) = 3600;
		pCnt++;
	}
	pCnt = &ESCFlagA.ESCCntSec.ChargingTime;
	for (i = 0; i < sizeof(ESCFlagA.ESCCntSec); i++)
	{
		if ((*pCnt)++ > 3600)
			(*pCnt) = 3600;
		pCnt++;
	}
	pCnt = &ESCFlagB.ESCCntSec.ChargingTime;
	for (i = 0; i < sizeof(ESCFlagB.ESCCntSec); i++)
	{
		if ((*pCnt)++ > 3600)
			(*pCnt) = 3600;
		pCnt++;
	}
	pCnt = &ESCFlagC.ESCCntSec.ChargingTime;
	for (i = 0; i < sizeof(ESCFlagC.ESCCntSec); i++)
	{
		if ((*pCnt)++ > 3600)
			(*pCnt) = 3600;
		pCnt++;
	}

	Fan_Cnt();
	if (++Time.Second >= 60)
	{//1���Ӷ�ʱ����
		Time.Second = 0;
		cntForAutoStIn30++;
		if (++Time.Minute >= 60)
		{// 1Сʱ��ʱ����
			Time.Minute = 0;

			if (++Time.Hour >= 24)
			{
				Time.Hour = 0;
				if (++Time.Day > Monthnum(Time.Year, Time.Month))//������������
				{
					Time.Day = 1;
					if (++Time.Month > 12)
					{
						Time.Month = 1;
						Time.Year++;
					}
				}
			}
		}
	}
}

//2 ���ܿ���ģ��
//���ܿ���ģ�����ÿ�������ж����ܡ�ÿ�������ж����ܡ����ڼ��㹦�ܡ��ֱ��ɺ���Monthnum()��Yearnum()��IsWeek������*getWeek()
//��1��   int Monthnum(inty,int m)���ж�ÿ��������
//��2��   int Yearnum(int y)���ж�����������
//��3��   int isWeek(inty,int m)�����ݸ������¼�����µ�һ���Ӧ�����ڡ�
//��4��   char *getWeek(inty,int m,int d)�����ݸ��������ռ������Ӧ�����ڡ�
//#define LeapYear (y) (y%4==0&&y%100!=0||y%400==0)?1:0

int Monthnum(unsigned char y,unsigned char m)
{
	static int month[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
	if(y%4==0)
		month[2]=29;//�������29�죬ƽ��28��
	return month[m];
}
