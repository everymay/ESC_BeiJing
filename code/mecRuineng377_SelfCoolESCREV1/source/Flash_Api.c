/*
 * Flash_Api.c
 *
 *  Created on: 2018��9��18��
 *      Author: XuYuLin
 */

#include "F2837xS_device.h"
#include "F2837xS_Examples.h"
#include "VariblesRef.h"
#include "Flash2833x_API_Library.h"
#include "flash_programming_c28.h"      // Flash API example header file
#include "F021_F2837xS_C28x.h"
#include "F2837xS_flash.h"
#include "FlashApiAuthorFuc.h"

//#define FLASHA_ADDR 0x338000
//#define FLASHB_ADDR 0x330000
//#define FLASHC_ADDR 0x328000
//#define FLASHD_ADDR 0x320000
//#define FLASHE_ADDR 0x318000
//#define FLASHF_ADDR 0x310000
//#define FLASHG_ADDR 0x308000
//#define FLASHH_ADDR 0x300000
#define	FlASH1_COIG 0x30FA00
#define	FlASH2_COIG 0x31FA00

#ifndef NULL
#define NULL 0
#endif

//����FLASH�洢����ֱ�Ӷ��嵽ָ����FLASH��ַ,Ȼ���ǰ��Ĺ�����,����ʱ��ȱ��� �ڲ�������ʱд�뵽
//�����ļ�����ʾ��ַ
#pragma DATA_SECTION(RecordChanName0, "WAVEDATA2")
#pragma DATA_SECTION(RecordChanName1, "WAVEDATA2")
#pragma DATA_SECTION(RecordChanName2, "WAVEDATA2")

#define RecordCase90	 {"GridVoltAF","GridVoltBF","GridVoltCF","AdcMirror.ADCRESULT5","AdcMirror.ADCRESULT0",\
						  "AdcMirror.ADCRESULT3","CurrRefA","CurrRefB","LoadRealCurA","CurrHarmRefA","CurrHarmRefB",\
						  "VoltTheta","PwmVa","Theta","currentRefQ","currentRefD","GridPLLVoltD","GridPLLVoltQ",\
						  "VoltSlidA[VoltPrvPos]","PllPiOutput"}
#define RecordCase91	 {"GridVoltAF","GridVoltBF","GridVoltCF","GridCurrAF","GridCurrBF","GridCurrCF","LoadVoltUF",\
						  "LoadVoltVF","LoadVoltWF","ApfOutCurA","ApfOutCurB","ApfOutCurC","AdcMirror.ADCRESULT4","AdcMirror.ADCRESULT2",\
						  "AdcMirror.ADCRESULT1","ErrorRecord.all","VolFundA","CurrHarmRefA","dcVoltUp","dcVoltDn"}
#define RecordCase92	 {"GridVoltAF","GridVoltBF","GridVoltCF","GridFundaCurD","GridFundaCurQ","GridPLLVoltD","GridPLLVoltQ",\
						  "negCurCompD","negCurCompQ","LoadFundaCurZ","currentRefD","currentRefQ","UdcBalanceCurr","InvFundaCurD",\
						  "InvFundaCurQ","InvFundaCurNQ","InvFundaCurND","ApfOutCurZ","GridZeroCur","LoadRealCurZ"}
const char RecordChanName0[][25] = RecordCase90;
const char RecordChanName1[][25] = RecordCase91;
const char RecordChanName2[][25] = RecordCase92;

Uint16 pro_err,erase_state;

#define API_VERSION_MAJOR       210 //API�汾��
#define MAINSOFT_VERSION_MAJOR  105   //������汾��
#define SPANSOFT_VERSION_MAJOR  100 //����Ӱ汾��
#define GET_FLASH_DATA_ADDR(block)  (BOne_SectorP_start + ((sizeof(debugW) + sizeof(WaveRecCfg)) * (block)))
#define GET_FLASH_DATA_CFG(block)   (BOne_SectorP_start + ((sizeof(debugW) + sizeof(WaveRecCfg)) * (block)) + sizeof(debugW))
#define GET_FLASH_STATUS_BEFORE_POWERDOWN (BOne_SectorO_start)
#define FLASH_DATA_BUFF_ADDR (&debugW)

struct STRU_FLASH FlashStatus;
struct CODE_DSP_REG code_reg;
Fapi_StatusType oReturnCheck;
Uint16 WaveRecCfgHalf[FLASHCOIG_NUM];
Uint16 FlashRcveData[256];
Uint16 FlashReadData[16];
//Uint16 Databuffer[100];
#define BONE_SECTOR8k_SIZE 0x2000
#define BONE_SECTOR32k_SIZE 0x8000

//CircularBuffer FlashWaveFifo;

const Uint32 FlashAddrDef[10][4]={
                       {BOne_SectorP_start,BOne_SectorQ_start,BOne_SectorR_start},\
                       {BOne_SectorR_start,BOne_SectorS_start,0,0},\
                       {BOne_SectorT_start,0,0,0},\
                       {BOne_SectorT_start,BOne_SectorU_start,0,0},\
                       {BOne_SectorU_start,0,0,0},\
                       {BOne_SectorU_start,BOne_SectorV_start,0,0},\
                       {BOne_SectorV_start,BOne_SectorW_start,0,0},\
                       {BOne_SectorW_start,BOne_SectorX_start,0,0},\
                       {BOne_SectorX_start,0,0,0},\
                       {BOne_SectorX_start,BOne_SectorY_start,BOne_SectorZ_start,BOne_SectorAA_start}};

void FlashFaultRecord(void)
{
//	if(RecordFlash.FlashState.B.FlashApiEnable){
//		if(RecordFlash.FlashState.B.FlashWriteReady){
//			DINT;
//			FlashProgram((Uint32 *)GET_FLASH_DATA_ADDR(RecordFlash.FlashBlockWrite), (Uint16 *) FLASH_DATA_BUFF_ADDR,  FLASHDATA_NUM); //���ݴ洢��ַ��
//            FlashProgram((Uint32 *)GET_FLASH_DATA_ADDR(RecordFlash.FlashBlockWrite), (Uint16 *) WaveRecCfg,            FLASHCOIG_NUM); //�������ļ�
//			EINT;
//
//			if(++RecordFlash.FlashBlockWrite>10){
//			    RecordFlash.FlashBlockWrite = 0;
//				RecordFlash.FlashState.B.FlashWriteReady = 0;
//				RecordFlash.RecordBlockNum = RecordFlash.FlashBlockWrite;
//			}
////		    FM_DatWrite(FLASH_UI_BLOCK_READ_ADDR,sizeof(RecordFlash),&RecordFlash);
//
//			//flashCbWrite(FlashBlockFifo,RecordFlash.FlashBlockWrite); //�洢д��λ��
//		}
//	}else{
//		RecordFlash.RecordBlockNum = 1;             //��ʹ��Flash���� ֻ�ܴ�һ��
//	}
//
//	if(FaultRecSel.RecordMode!=0){
//	    RecordFlash.FlashState.B.AutoRecordReadFlag = 1;
//	    ReFFTDelayTim = 0;
//	}else{
//	    ReFFTDelayTim = 0xFF;
//	}
//	StateFlag.RecordWritting = 1;
    RecordFlash.RecordBlockNum = 1;
}

void FlashProgram(Uint32 *u32Index, Uint16 *Databuffer, Uint16 FlashDataNum)
{
//    Uint16 i;
//    volatile Fapi_FlashStatusType oFlashStatus;
//    DINT;
//    EALLOW;
//    PUMPREQUEST = 0x5A5A0002;
//    for(i=0; i<=WORDS_IN_FLASH_BUFFER; i++)
//    {
//        Databuffer[i] = i;
//    }
//    FlashDataNum = 20;
//    code_reg.address = 0x98005;
//    u32Index = code_reg.address;
//    Flash1EccRegs.ECC_ENABLE.bit.ENABLE = 0x0;
//    for(i=0;i< FlashDataNum;i++){
//        oReturnCheck = Fapi_issueProgrammingCommand((Uint32 *)u32Index,Databuffer+i,1,0,0,Fapi_DataOnly);
//        u32Index++;
//        while(Fapi_checkFsmForReady() == Fapi_Status_FsmBusy);
//        if(oReturnCheck != Fapi_Status_Success){
//            FlashStatus.FlashCtrl.B.ProgramErr = 1;
//        }
//    }
//    EDIS;
//    EINT;
//    cbWrite();
}

void WriteFlashConfig(Uint16 FaultNum)
{
	//ֻ��IGBT���Ϸ���,˲ʱֵ���� ����¼��,����¼�������з�����
	if(     (FaultNum==101)||(FaultNum==102)||(FaultNum==103)||(FaultNum==104)||\
	        (FaultNum==105)||(FaultNum==106)||(FaultNum==107)||(FaultNum==108)||\
	        (FaultNum==110)||(FaultNum==111)||(FaultNum==112)||(FaultNum==130)||\
	        (FaultNum==109)||(FaultNum==113)||(FaultNum==123)||(FaultNum==124)||\
	        (FaultNum==120)||(FaultNum==121)||(FaultNum==122)||\
	        (FaultNum==125)     ){
        Uint16 *Ps = (Uint16 *)(&WaveRecCfgHalf);
        *Ps   =  Time.Year<<8;
        *Ps++ |= Time.Month&0xFF;
        *Ps =  Time.Day<<8;
        *Ps++ |= Time.Hour&0xFF;
        *Ps =  Time.Minute<<8;
        *Ps++ |= Time.Second&0xFF;
        *Ps++ =  Time.MicroSec;
        *Ps++ =  FaultNum;
        *Ps++ =  WaveRecordHandler++;
        FlashFaultRecord();
	}
}

//ͨ�Ź�Լ "��7-14�������¼�������ļ�,����ַ0xFA00~0xFE0F"
void FlashApiDataCopy(Uint16 Samprate){
	Uint16 *Ps = (Uint16 *)(&WaveRecCfgHalf[6]);
	*Ps++ = APF_ID;                         //0xFA06
	*Ps++ = 0x0101;						    //0xFA07
	*Ps++ = 8;
	*Ps++ = 9;
	*Ps++ = 10;
	*Ps++ = 11;
	*Ps++ = 12;
	switch(Samprate){
		case 0:*Ps++ =  2*(Uint16)PWMFREQUENCY;break;//0xFA0D����Ƶ��
		case 1:*Ps++ =  PWMFREQUENCY;break;
		case 2:*Ps++ =  6400;break;
		case 3:*Ps++ =  3200;break;
		case 4:*Ps++ =  1600;break;
		case 5:*Ps++ =  800;break;
		case 6:*Ps++ =  400;break;
		case 7:*Ps++ =  200;break;
		default:*Ps++ =  200;break;
	}
	*Ps++ = DEBUG_WAVE_LEN;
    *Ps++ = DEBUG_WAVE_GROUP;               //FA0Fͨ������
	 FlashCoigDataCopy(FaultRecSel.RecordChan);
}
//----------flash��������������-------------//

//void FlashStatusCheck(void)
//{
//    EALLOW;
//    PUMPREQUEST = 0x5A5A0002;
//    oReturnCheck = Fapi_initializeAPI(F021_CPU0_W0_BASE_ADDRESS, 194);
//    if(oReturnCheck != Fapi_Status_Success){
//        FlashStatus.FlashCtrl.B.FlashCheckOnce = 1;
//    }
//
//    oReturnCheck = Fapi_setActiveFlashBank(Fapi_FlashBank1);
//    if(oReturnCheck != Fapi_Status_Success){
//        FlashStatus.FlashCtrl.B.FlashCheckOnce = 1;
//    }
//    EDIS;

//    FM_DatRead(FLASH_UI_BLOCK_READ_ADDR,sizeof(RecordFlash),&RecordFlash);

//    void FlashCbRead(&FlashBlockFifo,float *out);

//    //����¼���ϵ�ǰ״̬�ָ�
//    RecordFlash.UiBlockRead     = FM_Read_Byte(FLASH_UI_BLOCK_READ_ADDR);   //�����ֶ���ȡ��λ��
//    RecordFlash.FlashBlockRead  = FM_Read_Byte(FLASH_BLOCK_READ_ADDR);      //����¼�����ȡλ��
//    RecordFlash.FlashBlockWrite = FM_Read_Byte(FLASH_BLOCK_WRITE_ADDR);     //����¼����洢λ��
//    RecordFlash.RecordBlockNum  = FM_Read_Byte(FLASH_RECORD_BLOCK_ADDR);    //����¼����洢λ��
//    WaveRecordHandler           = FM_Read_Byte(FLASH_RECORD_HANDLER_ADDR);  //����¼������
//}


//return err:0,�޴���,1����һ��.1***.���ַ����.
//block:���ַ.0~7
//�õ����˹�Ԥ����������е�debugW�Ĵ�С,�����е�ַ��Ϊ10��(block),��Ҫ������һ��,����FlashAddrDef����в鵽��Ҫerase�Ŀ�
//����һ�ַ����Ǹ���Ҫд��ĵ�ַ���ձ��������Ҫ�����ļ���.
//int EraseSector(int block)
int EraseSector()
{
    int err=0;
//    int j;
//    Fapi_FlashStatusWordType oFlashStatusWord;
//
//    if(block<7){
//        DINT;
//        if(RecordFlash.FlashState.B.FlashApiEnable){
//            FlashStatus.FlashCtrl.B.EraseStatus = 1;    //�����ɹ�
//            for(j=0;j<4;j++){
//                if(FlashAddrDef[block][j] != 0){
//                    oReturnCheck = Fapi_issueAsyncCommandWithAddress(Fapi_EraseSector,(Uint32 *)FlashAddrDef[block][j]);
//                    while(Fapi_checkFsmForReady() != Fapi_Status_FsmReady){}
//
////                    oReturnCheck = Fapi_doBlankCheck((Uint32 *)FlashAddrDef[block][j],  //��������Ƿ�����ɹ�
////                                                     FlashAddrDef[block][5],
////                                                     &oFlashStatusWord);
////                    if(oReturnCheck != Fapi_Status_Success){
////                        FlashStatus.FlashCtrl.B.EraseStatus = 2;                        //����ʧ��
////                        err++;
////                    }
//                }
//            }
//        }
//        EINT;
//
////        FM_DatWordWrite(FLASH_UI_BLOCK_READ_ADDR,   RecordFlash.UiBlockRead);       //�����ֶ���ȡ��λ��
////        FM_DatWordWrite(FLASH_BLOCK_READ_ADDR,      RecordFlash.FlashBlockRead);    //����¼�����ȡλ��
////        FM_DatWordWrite(FLASH_BLOCK_WRITE_ADDR,     RecordFlash.FlashBlockWrite);   //����¼����洢λ��
////        FM_DatWordWrite(FLASH_RECORD_BLOCK_ADDR,    RecordFlash.RecordBlockNum);    //����¼����洢λ��
////        FM_DatWordWrite(FLASH_RECORD_HANDLER_ADDR,  WaveRecordHandler);              //����¼������
//    }else{
//        err = err | 1000;
//    }
//	RecordFlash.RecordBlockNum = 0;
	return err;
}

void StartEraseSector(void)
{
    ;
}
//ÿ��ͨ��ռ��25����ַ,��ʾ24���ַ�,�ԡ�/0����Ϊ�ַ�����β,�����ַ���˳����abcd,��ͨ����������:
//0xFA10 :[bit15~bit8]:a     [bit7~bit0]:b
//0xFA11 :[bit15~bit8]:c     [bit7~bit0]:d
//0xFA12 :[bit15~bit8]:/0    [bit7~bit0]:����
#define CHAN_NAME_CHAR_SIZE (26/2)   //ÿ���ַ�ռ25���ռ�,����13��Uint16
void FlashCoigDataCopy(Uint16 chan)
{
	Uint16 i,k,m,n;
	Uint16 ChanName[CHAN_NAME_CHAR_SIZE];
	Uint16 *Ps = (Uint16 *)(&WaveRecCfgHalf[16]);
	Uint16 *Pn;
	for(i=0;i<DEBUG_WAVE_GROUP;i++){  //377Ŀǰ12��
		m=0;
		for(k=0;k<CHAN_NAME_CHAR_SIZE;k++){
			switch(chan){
				case 90:
					ChanName[m] = RecordChanName0[i][k*2]<<8;   //�����ʽ,����
					ChanName[m] |= RecordChanName0[i][k*2+1];
					break;
				case 91:
					ChanName[m] = RecordChanName1[i][k*2]<<8;
					ChanName[m] |= RecordChanName1[i][k*2+1];
					break;
				case 92:
					ChanName[m] = RecordChanName2[i][k*2]<<8;
					ChanName[m] |= RecordChanName2[i][k*2+1];
					break;
				default:
					ChanName[m] = 0;
					break;
			}
			m++;
		}
		Pn = &ChanName[0];
		for(n=0;n<CHAN_NAME_CHAR_SIZE;n++)	*Ps++ = *Pn++;
	}
}

//isCfg:1������¼���������ļ�.0������¼���Ĳ����ļ�
//��(block):flash�п�ĸ�����,��debugW��ʽ����,��������config�洢,��:blocks[0...11][]�д����12�鲨��,blocks[12]�����config�ļ�.
void FlashDataRead(void)
{
//	Uint16 i;
//	Uint16 *FlashPoint,*dstAddr;

//    if(RecordFlash.FlashState.B.FlashApiEnable){    //wave�ļ���ʱ����debugW[0]��,�ȴ�ͨ�Ź�Լ���ߺ��������������ʹ��
//        if(RecordFlash.FlashBlockRead==0){          //���Զ�
//            dstAddr = (Uint16 *)(&debugW);
//            FlashPoint = (Uint16 *)GET_FLASH_DATA_ADDR(RecordFlash.FlashBlockRead);
//            for(i=0;i<FLASHDATA_NUM;i++){
//                *dstAddr++ = *FlashPoint++;
//            }

//            dstAddr = (Uint16 *)(&WaveRecCfg);      //config�ļ���ʱ����debugW[0]��,�ȴ�ͨ�Ź�Լ���ߺ��������������ʹ��
//            FlashPoint = (Uint16 *)GET_FLASH_DATA_CFG(RecordFlash.FlashBlockRead);
//            for(i=0;i<FLASHDATA_NUM;i++){
//                *dstAddr++ = *FlashPoint++;
//            }
//        }
//    }

	RecordFlash.FlashState.B.AutoRecordReadFlag = 0;
	RecordFlash.UiBlockRead = 0;
}



