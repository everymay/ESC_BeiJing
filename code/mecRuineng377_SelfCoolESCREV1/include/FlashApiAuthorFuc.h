/*
 * FlashApiAuthorFuc.h
 *
 *  Created on: 2019年2月18日
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
    Uint16   property;        //当前数据属性信息（暂时用不到）
    Uint16   address_H16;     //地址高16位
    Uint16   address_L16;     //地址低16位
    Uint16   code_num;        //更新数据个数
    Uint16   code_cnt;
    Uint16   data_num;        //更新dsp数据个数
    Uint32   address;         //dsp数据地址
    Uint16   address_offset;  //地址偏移量
    Uint16   sector;          //数据所在扇区
    Uint16   sector_offset;   //数据所在扇区偏移
    Uint16   state;           //数据更新状态0：加载个数数据
    Uint16  data;            //有效数据
};
extern struct CODE_DSP_REG code_reg;

extern Uint16 WaveRecCfgHalf[FLASHCOIG_NUM];
void FlashProgram(Uint32 *u32Index, Uint16 *Databuffer, Uint16 FlashDataNum);
void FlashCoigDataCopy(Uint16 chan);

#endif /* INCLUDE_FLASHAPIAUTHORFUC_H_ */
