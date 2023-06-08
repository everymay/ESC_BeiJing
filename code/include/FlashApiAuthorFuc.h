/*
 * FlashApiAuthorFuc.h
 *
 *  Created on: 2019��2��18��
 *      Author: XYL
 */

#ifndef INCLUDE_FLASHAPIAUTHORFUC_H_
#define INCLUDE_FLASHAPIAUTHORFUC_H_
#include "F2837xS_device.h"
#include <string.h>
#include "flash_programming_c28.h"      // Flash API example header file
#include "F021_F2837xS_C28x.h"


#define  CPU_MAIN_FREQUENCE_150M    0
#define  CPU_MAIN_FREQUENCE_200M    1
#define WaittingTime                3000
#define SECTOR_377S                 0xFFFF
#define SECTORAB                    0x0001
#define FLASHCOIG_NUM               180      // 16+12*13

#define FLASHA_ADDR     0x080000
#define FLASHB_ADDR     0x082000
#define FLASHC_ADDR     0x084000
#define FLASHD_ADDR     0x086000
#define FLASHE_ADDR     0x088000
#define FLASHF_ADDR     0x090000
#define FLASHG_ADDR     0x098000
#define FLASHH_ADDR     0x0A0000

#define SECTORA_ADDR    1
#define SECTORB_ADDR    2
#define SECTORC_ADDR    3
#define SECTORD_ADDR    4
#define SECTORE_ADDR    5
#define SECTORF_ADDR    6
#define SECTORG_ADDR    7
#define SECTORH_ADDR    8

#define  WORDS_IN_FLASH_BUFFER    20  // Programming data buffer, words
#define PUMPREQUEST *(unsigned long*)(0x00050024)

#ifdef __TI_COMPILER_VERSION__
    #if __TI_COMPILER_VERSION__ >= 15009000
        #define ramFuncSection ".TI.ramfunc"
    #else
        #define ramFuncSection "ramfuncs"
    #endif
#endif

extern Uint16 FlashErase;
extern Uint16 FlashProgm;

typedef union{
    Uint16 all;
    struct{
        Uint16  EraseStatus             :3;
        Uint16  FlashAnState            :3;
        Uint16  FlashCheckOnce          :1;
        Uint16  FlashCrcErr             :1;
        Uint16  FlashHexErr             :1;
        Uint16  ProgramErr              :1;
        Uint16  ProgramEnd              :1;
        Uint16  FlashMainJump           :1;
        Uint16  StateLine               :1;

    }B;
}FLASHCTRL;
struct STRU_FLASH{
    FLASHCTRL   FlashCtrl;
    Uint16      FlashSector;
    Uint16      FlashTimCnt1s;
    Uint16      DataAnlyState;
};
extern struct STRU_FLASH FlashStatus;

struct CODE_DSP_REG{
    Uint16   property;        //��ǰ����������Ϣ����ʱ�ò�����
    Uint16   address_H16;     //��ַ��16λ
    Uint16   address_L16;     //��ַ��16λ
    Uint16   code_num;        //�������ݸ���
    Uint16   code_cnt;
    Uint16   data_num;        //����dsp���ݸ���
    Uint32   address;         //dsp���ݵ�ַ
    Uint16   address_offset;  //��ַƫ����
    Uint16   sector;          //������������
    Uint16   sector_offset;   //������������ƫ��
    Uint16   state;           //���ݸ���״̬0�����ظ�������
    Uint16  data;            //��Ч����
};
extern struct CODE_DSP_REG code_reg;

extern Uint16 WaveRecCfgHalf[FLASHCOIG_NUM];
void FlashProgram(Uint32 *u32Index, Uint16 *Databuffer, Uint16 FlashDataNum);
void FlashCoigDataCopy(Uint16 chan);

#endif /* INCLUDE_FLASHAPIAUTHORFUC_H_ */
