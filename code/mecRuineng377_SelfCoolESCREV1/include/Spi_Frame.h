#ifndef   SPI_FRAME_H
#define   SPI_FRAME_H
#include "F2837xS_device.h"     // Headerfile Include File


#define SET_SPICS(VAL)  if(VAL) GpioDataRegs.GPCSET.bit.GPIO66 = 1; else GpioDataRegs.GPCCLEAR.bit.GPIO66 = 1
#define GET_SPICS()             GpioDataRegs.GPCDAT.bit.GPIO66          //片选
#define SET_SPIWPn(VAL)
#define GET_SPIWPn()
//#define SET_SPIWPn(VAL)	if(VAL) GpioDataRegs.GPBSET.bit.GPIO48 = 1; else GpioDataRegs.GPBCLEAR.bit.GPIO48 = 1
//#define GET_SPIWPn()    		GpioDataRegs.GPBDAT.bit.GPIO48 = 1		//EEPROM写保护关闭

//#define SPICS GpioDataRegs.GPBDAT.bit.GPIO57

#define WREN   0x0600
#define WRDI   0x0400
#define RDSR   0x0500
#define WRSR   0x0100
#define READ   0x0300
#define WRITE  0x0200 

#define  LOCK    0x8c00
#define  UNLOCK  0x8000  

Uint16 SpiWrite(Uint16 data);
void   InitESpi(void);
void   InitSpiGpio(void);

void   FM_Write_Enable(void);
void   FM_Write_Disable(void);
Uint16 FM_Read_Status();
void   FM_Write_Status(Uint16 data);
Uint16 FM_Read_Byte(Uint16 address);
void   FM_Write_Byte(Uint16 address,Uint16 data);
void   FM_StructureDatReset(int addr, int length);
void   FM_StructureDatWrite(int addr, int length, Uint16 *ptr);
Uint16 FM_StructureDatWrite2(void);
void   FM_DatWrite(int addr, int length, int *ptr);
void   FM_DatRead(int addr, int length, int *ptr);
void   FM_DatRead16(int addr, int length, int *ptr);
Uint16 FM_DatReadWord(Uint16 address);
void   FM_DatWordWrite(int addr,int data);
void SoeRecData(Uint16);
void InitSoeData(void);
void ResetSoeData(int srcGroup);
void SaveDataEead(void);
void AddrWriteData(void);
void ReadAddrData(int g);
void SOE_Write(void);
void SOE_Wave_Record(void);
void UserParamSetting(void);
void FactoryParamSetting(void);
void CapParamSetting(void);
void FaultParamSetting(void);
void UserHarmnoicSetting(void);
void HarmnoicParamSetting(void);
void CommunicationParamSetting(void);
void ManufacturerParametersSetting(void);
void UserPreferencesSetting(void);
void IDPreferencesSetting(void);
void FlashPreferencesSetting(void);
void VirtuPreferencesSetting(void);
void Multiple_Parallel_Message(void);
//void FlagReset(void);

#endif
