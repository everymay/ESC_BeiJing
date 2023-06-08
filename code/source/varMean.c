#include "F2837xS_device.h"
#include "F2837xS_Examples.h"
#include "VarMean.h"
#include "CoreCtrl.h"

float VarMeanOut[MEAN_N]={0};   //���������
float rtb_Vq[MEAN_N]={0};       //���뻺����
B_varMean_T varMean_B;
#pragma CODE_SECTION(varMean_step, "ram2func")
Uint16 Times=0;
Uint16 Times1=0;
float ModData[MEAN_N]={0};
float ModDataOnce[MEAN_N]={0};
void varMean_step(/*B_varMean_T varMean_B*/)
{
	float rtb_Kp6;
	float *tmp,*tnp,*tkp,*tbp,*thp,*vmo;
	int i,k,m;
	tmp = &varMean_B.Integ4[0];
	tnp = &varMean_B.Integ4_DSTATE[0];
	tkp = &rtb_Vq[0];                               //����

    /* DiscreteIntegrator: '<S6>/Integ4' */
    if (varMean_B.Integ4_SYSTEM_ENABLE == 0U) {      //��ʼ�ж�
        for(i=MEAN_N;i;i--)
            *tmp++ = Integ4_gainval_d * *tkp++ + *tnp++;        //���λ��ֵ�һ��
//-           varMean_B.Integ4[i] = varMean_B.Integ4_gainval_d * rtb_Vq[i] +  varMean_B.Integ4_DSTATE[i];   //��ɢ��������
    } else {
        for(i=MEAN_N;i;i--)
            *tmp++ = *tnp++;
        varMean_B.Integ4_SYSTEM_ENABLE = 0;       /* S-Function block: <S19>/S-Function  */                                                               //����sfcn ��ֵ
    }

    //������Ƶ�ʱ仯������bufƫ�Ƽ���ֵƫ��
    float rtb_UnitDelay = GridFreq;
	/* Saturate: '<S6>/To avoid division by zero' incorporates:*/
	if (rtb_UnitDelay >= Toavoiddivisionbyzero_UpperSa_o){//�ж� ���������rtb_Kp6 ������1e6������1e-6֮��
		rtb_Kp6 = Toavoiddivisionbyzero_UpperSa_o;
	} else if (rtb_UnitDelay <= Toavoiddivisionbyzero_LowerSa_h) {
		rtb_Kp6 = Toavoiddivisionbyzero_LowerSa_h;
	} else {
		rtb_Kp6 = rtb_UnitDelay;
	}
	float rtb_Sum1_p = (Step) / rtb_Kp6;              //N      // rtb_Kp6:fcn����������u[1]
	int rtb_sinwt2pi3_b = (int)(rtb_Sum1_p+1);        //F      // rtb_Sum1_p:ceil��������/
	float Delay = Gain_Gain_e * rtb_sinwt2pi3_b;                  // ���ñ���Delay,Ts:Gain*ceil
    rtb_Kp6 = rtb_Sum1_p - rtb_sinwt2pi3_b;
    float rtb_vari = rtb_Kp6 * Gain1_Gain_o;
    float rtb_varr = rtb_Kp6 / rtb_Sum1_p;

    /* Calculate delayed index */
    int discreteDelay =(int)((Delay * Step) + 0.5);   //GAIN������varMean_B.Delay��S-fcn������������
    if (discreteDelay > (bufSz - 1)){               // ��ɢ�����ӳٿ�����Ԥ��ֵ֮��
        discreteDelay = (bufSz - 1);
    }
    int indDelayed = varMean_B.indEnd - ((discreteDelay > 0) ? discreteDelay : 0); //����ƫ��ֵ
    if (indDelayed < 0) {
        if (varMean_B.indBeg == 0)
            indDelayed = 0;
        else
            indDelayed += bufSz;
    }

	/* S-Function block: <S19>/S-Function  */
    tkp = &varMean_B.Integ4[0];
    tbp = &varMean_B.SFunction[0];
    tnp = &varMean_B.dvtd_buffer[varMean_B.indEnd][0];
    tmp = &varMean_B.dvtd_buffer[indDelayed][0];
    for(i=MEAN_N;i;i--){
		*tnp++ = *tkp++;
        *tbp++ = *tmp++;
//-	    dvtd_buffer[n][varMean_B.indEnd] = varMean_B.Integ4[n];//Sfcn��In����
//-     varMean_B.SFunction[m] = dvtd_buffer[m][indDelayed];
	}

	tnp = &rtb_Vq[0];
    vmo = &VarMeanOut[0];
	tmp = &varMean_B.UnitDelay_DSTATE_n[0];
	tkp = &varMean_B.Integ4[0];
	tbp = &varMean_B.SFunction[0];
	thp = &varMean_B.Integ4_DSTATE[0];
	for(i=MEAN_N;i;i--){
		*vmo++ = ((*tnp - *tmp) * rtb_vari+ *tnp) * (rtb_varr) + (*tkp - *tbp++) * rtb_UnitDelay;

		*thp++ = Integ4_gainval_d * *tnp + *tkp++;      //���λ��ֵڶ���/* Update for DiscreteIntegrator: '<S6>/Integ4' */
	    *tmp++ = *tnp++;                                /* <S18>/Unit Delay'����correctionģ�� */

	}
	if(Times1++>=64000){
	    Times++;
	    Times1 = 64000;
	}
	for(k=0;k<MEAN_N;k++){
      if(Times == 12800){
          ModData[k]= varMean_B.Integ4_DSTATE[k]-ModDataOnce[k];
      }
      if(Times>=25600){
          Times=25601;
          varMean_B.Integ4_DSTATE[k] -= 2*ModData[k];
      }
	}
    if(Times==25601){
        Times = 0;
        for(m=0;m<MEAN_N;m++)
            ModDataOnce[m] = varMean_B.Integ4_DSTATE[m];
    }

    int indBeg = varMean_B.indBeg;
    int indEnd = varMean_B.indEnd;
    indEnd = indEnd < bufSz-1 ? indEnd+1 : 0;
    if (indEnd == indBeg) {
        indBeg = indBeg < bufSz-1 ? indBeg+1 : 0;
    }
    varMean_B.indBeg = indBeg;
    varMean_B.indEnd = indEnd;
//  for(k=0;k<MEAN_N;k++){
//      /* Sum: '<S18>/Sum5' */
//      rtb_Kp6 = rtb_Sum1_p - rtb_sinwt2pi3_b;
//      rtb_Divide[k] = ((rtb_Vq[k] - varMean_B.UnitDelay_DSTATE_n[k]) * rtb_Kp6 *
//    		  	  	  varMean_B.Gain1_Gain_o + rtb_Vq[k]) * (rtb_Kp6 / rtb_Sum1_p) +
//    				  (varMean_B.Integ4[k] - varMean_B.SFunction[k]) * rtb_UnitDelay;
//      MeanOutTest[k] = rtb_Divide[k];
//    varMean_B.UnitDelay_DSTATE_n[y] = rtb_Vq[y];
//  /* Update for DiscreteIntegrator: '<S6>/Integ4' */
//  varMean_B.Integ4_SYSTEM_ENABLE = 0U;                                //������ʼ�ж�����
//  varMean_B.Integ4_DSTATE[k] = varMean_B.Integ4_gainval_d * rtb_Vq[k] + varMean_B.Integ4[k];
//  }
}

void varMean_initialize(void)
{
    int i;

	int *q=(int *)(&varMean_B.Integ4_DSTATE[0]); //ע�������ָ�����
	for(i=0;i<sizeof(B_varMean_T);i++){
		*q++ = 0;
	}

	varMean_B.indEnd = 1;
	varMean_B.Integ4_SYSTEM_ENABLE = 1;
}


//////FIFO��ά����/////////////////////////////////////////////////////////////////
#pragma CODE_SECTION(cbWrite, "ram2func")
#pragma CODE_SECTION(cbRead, "ram2func")
#pragma CODE_SECTION(cbIsEmpty, "ram2func")

CircularBuffer BlockFifo;
ElemType ELEMS_DAT;
//extern float ELEMS_DAT[FIFO_DQ_BLOCK_NUM][FIFO_DQ_DATA_SIZE];
//float32 ELEMS_DAT[FIFO_DQ_BLOCK_NUM][FIFO_DQ_DATA_SIZE];
#define FIRST_CYCLE_FIFO_MEAN_IN 0
#if FIRST_CYCLE_FIFO_MEAN_IN
#define cbInit()
#define cbIsFull(cb)    (cb->end == (cb->start ^ cb->size) ? 1 :0)
#define cbIsEmpty(cb)   (cb->end == cb->start ? 1 :0)
#define cbIncr(cb,p)    ((p + 1) & (2 * cb->size - 1))
#else
void cbInit(CircularBuffer *cb, int size) //��ʼ��,���ٿռ�
{
    cb->size = size;
    cb->start = 0;
    cb->end = 0;
}

inline int cbIsFull(CircularBuffer *cb)     //�жϻ�����Ϊ��
{
    return cb->end == (cb->start ^ cb->size);   // This inverts the most significant bit of start before comparison
                                                //  (дָ�� == (��ָ�� ��� ����������) //����൱�ڼ�,������ʱ��д��ַ(����ָʾλ)
}

int cbIsEmpty(CircularBuffer *cb) //�жϻ������Ƿ�λ��
{
    return cb->end == cb->start;
}

inline int cbIncr(CircularBuffer *cb, int p)    //��ַ��1
{
    return (p + 1) & (2 * cb->size - 1); // start and end pointers incrementation is done modulo 2 * size
}


void cbWrite(CircularBuffer *cb,float *in)
{
    int i;
    float *p=ELEMS_DAT[cb->end & (cb->size - 1)];

    for(i=FIFO_DQ_DATA_SIZE;i;--i){
        *p++ =*in++;
    }

    if (cbIsFull(cb))                       // full, overwrite moves start pointer
        cb->start = cbIncr(cb, cb->start);  //���Ǿ�����,��ʼ����ַ��1

    cb->end = cbIncr(cb, cb->end);          //д��ַ��1
}

void cbRead(CircularBuffer *cb,float *out)
{
    int i;
    float *p=ELEMS_DAT[cb->start & (cb->size - 1)];

    for(i=FIFO_DQ_DATA_SIZE;i;--i){
        *out++ = *p++;
    }

    cb->start = cbIncr(cb, cb->start);
}


/*
 * //////////////////////////////////
#define FIFO_FLASH_DATA_SIZE 1
#define FIFO_FLASH_BLOCK_NUM 1<<4      //�������2����������,����������
CircularBuffer FlashBlockFifo;
int FlashELEMS_DAT[FIFO_FLASH_BLOCK_NUM];

void flashCbWrite(CircularBuffer *cb,float *in)
{
    int i;
    float *p=FlashELEMS_DAT[cb->end & (cb->size - 1)];

    for(i=FIFO_FLASH_DATA_SIZE;i;--i){
        *p++ =*in++;
    }

    if (cbIsFull(cb))                       //full, overwrite moves start pointer
        cb->start = cbIncr(cb, cb->start);  //���Ǿ�����,��ʼ����ַ��1

    cb->end = cbIncr(cb, cb->end);          //д��ַ��1
    FM_DatWrite(FLASH_UI_BLOCK_READ_ADDR,sizeof(FlashBlockFifo),&FlashBlockFifo);
}

void FlashCbRead(CircularBuffer *cb,float *out)
{
    int i;
    float *p=FlashELEMS_DAT[cb->start & (cb->size - 1)];

    for(i=FIFO_FLASH_DATA_SIZE;i;--i){
        *out++ = *p++;
    }

    cb->start = cbIncr(cb, cb->start);
    FM_DatWrite(FLASH_UI_BLOCK_READ_ADDR,sizeof(FlashBlockFifo),&FlashBlockFifo);
}
*/
#endif



//////////fifo////////////
/*float elems[BLOCK_NUM][DATA_SIZE] ={{1,2,3},{4,5,6},{7,8,9},{10,11,12},{13,14,15}};
float testdata[110]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
float outdata[110]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
float *inelem=&testdata[0];
float *outelem=&outdata[0];
//TEST.C�ļ�,��������һ������,Ȼ��ͨ�����ֶ�д����.�������������Ƿ����.
    cbInit(&BlockFifo,BLOCK_NUM);
    int n,k;

    for(n=0;n<5;n++)
    {
        for(k=0;k<5;k++){
            cbWrite(&BlockFifo);
        }

        for(k=0;k<4;k++){
            cbRead(&BlockFifo);

        }
    }
void cbInit(CircularBuffer *cb, int size) //��ʼ��,���ٿռ�
{
    int i;
    cb->size = size;
    cb->start = 0;
    cb->end = 0;

    for(i=0;i<100;i++)
    {
        testdata[i]=i;
        outdata[i] = 0;
    }
}
//FIFO Դ�ļ�
typedef int ElemType;

typedef struct {
    int size;            // maximum number of elements         //�ڴ��С
    int start;           // index of oldest element            //��ʼָ��
    int end;             // index at which to write new element//����ָ��
    ElemType *elems;     // vector of elements                 //��������
} CircularBuffer;

void cbInit(CircularBuffer *cb, int size) //��ʼ��,���ٿռ�
{
    cb->size = size;
    cb->start = 0;
    cb->end = 0;
    cb->elems = (ElemType *)calloc(cb->size, sizeof(ElemType));
}

int cbIsFull(CircularBuffer *cb) //�жϻ�����Ϊ��
{
    return cb->end == (cb->start ^ cb->size); // This inverts the most significant bit of start before comparison
    //  (дָ�� == (��ָ�� ��� ����������) //����൱�ڼ�,������ʱ��д��ַ(����ָʾλ)
}

int cbIsEmpty(CircularBuffer *cb) //�жϻ������Ƿ�λ��
{
    return cb->end == cb->start;
}

int cbIncr(CircularBuffer *cb, int p) //��ַ��1
{
    return (p + 1) & (2 * cb->size - 1); // start and end pointers incrementation is done modulo 2 * size
}

void cbWrite(CircularBuffer *cb, ElemType *elem)
{
    cb->elems[ cb->end & (cb->size - 1) ] = *elem;
    if (cbIsFull(cb)) // full, overwrite moves start pointer
        cb->start = cbIncr(cb, cb->start); //���Ǿ�����,��ʼ����ַ��1
    cb->end = cbIncr(cb, cb->end); //д��ַ��1
}

void cbRead(CircularBuffer *cb, ElemType *elem)
{
    *elem = cb->elems[cb->start & (cb->size - 1)];
    cb->start = cbIncr(cb, cb->start);
}
*/
