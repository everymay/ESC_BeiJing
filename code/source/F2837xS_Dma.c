//###########################################################################
//
// FILE:    F2837xS_DMA.c
//
// TITLE:   F2837xS Device DMA Initialization & Support Functions.
//
//###########################################################################
// $TI Release: F2837xS Support Library v3.05.00.00 $
// $Release Date: Thu Oct 18 15:50:26 CDT 2018 $
// $Copyright:
// Copyright (C) 2014-2018 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions 
// are met:
// 
//   Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the 
//   documentation and/or other materials provided with the   
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//###########################################################################

//
// Included Files
//
#include "F2837xS_device.h"
#include "F2837xS_Examples.h"
#include "CoreCtrl.h"
#pragma CODE_SECTION(DMACH1ConfigAndRun, "ram2func");
#pragma CODE_SECTION(SecondDMACH1ConfigAndRun, "ram2func");
#pragma CODE_SECTION(StartDMACH1, "ram2func");
#pragma CODE_SECTION(StartDMACH2, "ram2func");

void DMA1Initialize(void);
void DMA2Initialize(void);

int16 firstDMAReadLen = 0;
int16 SecondReadLen = 0;
float32 *SecondDMAsrc;

void DMACH1ConfigAndRun(float32 *src,int16 ADPos)
{
/*	int16 firstReadPos=(ADPos-(RFFT_SIZE));	     //当前位置与当前位置的前FFTSIZE个点为FFT的src长度

//	ProfileGetVar(LOCALVAR);
//	ProfileInTime(LOCALVAR);

	if(firstReadPos<0){
		firstReadPos+=ADTEMPBUFLEN;					//环形缓冲区范围调整为可用范围
		firstDMAReadLen=ADTEMPBUFLEN-firstReadPos;
	}else{
		firstDMAReadLen=RFFT_SIZE;					//一次传输完成
	}

	EALLOW;
	DmaRegs.CH1.SRC_ADDR_SHADOW =     (Uint32)(src+firstReadPos);
	DmaRegs.CH1.DST_ADDR_SHADOW =     (Uint32)&RFFTin1Buff[0];
	if(firstDMAReadLen>=RFFT_SIZE){					//启动一次DMA
		SecondReadLen=0;							//关断第二次中断
		//TRANSFERSIZE These bits specify the number of bursts to transfer:0x0000 Transfer 1 burst
		DmaRegs.CH1.TRANSFER_SIZE = RFFT_SIZE-1;   	//Number of bursts per transfer, DMA interrupt will occur after completed transfer
	}else{											//启动贰次DMA
		SecondReadLen=RFFT_SIZE-firstDMAReadLen;	//使能第二次中断,并且设置数据长度
		SecondDMAsrc=src;
		DmaRegs.CH1.TRANSFER_SIZE = firstDMAReadLen-1;//Number of bursts per transfer, DMA interrupt will occur after completed transfer
	}

	DmaRegs.CH1.CONTROL.bit.RUN = 1;				//开启此DMA
    DmaRegs.CH1.CONTROL.bit.PERINTFRC = 1;			//软件启动DMA
//    ProfileOutTime(LOCALVAR);
//    MemProfileData(LOCALVAR,PROFILE_CHAN_DMA);
	EDIS;
	*/
}

void SecondDMACH1ConfigAndRun(void)
{
/*	EALLOW;
	DmaRegs.CH1.SRC_ADDR_SHADOW =     (Uint32)(SecondDMAsrc);
	DmaRegs.CH1.DST_ADDR_SHADOW =     (Uint32)(&RFFTin1Buff[0]+firstDMAReadLen);
	DmaRegs.CH1.TRANSFER_SIZE = SecondReadLen-1;   	//Number of bursts per transfer, DMA interrupt will occur after completed transfer
	SecondReadLen=0;								//关闭第二次中断计数器
    DmaRegs.CH1.CONTROL.bit.RUN = 1;				//开启此DMA
    DmaRegs.CH1.CONTROL.bit.PERINTFRC = 1;			//软件启动DMA
    EDIS;
//	ProfileOutTime(Global2);
//	MemProfileData(Global2,PROFILE_CHAN_ADDMA);
 */
}

void DMAInitialize(void)
{
    EALLOW;
    // Perform a hard reset on DMA
    DmaRegs.DMACTRL.bit.HARDRESET = 1;
    asm (" nop"); // one NOP required after HARDRESET

    // Allow DMA to run free on emulation suspend
    DmaRegs.DEBUGCTRL.bit.FREE = 1;
    EDIS;
    DMA1Initialize();
//    DMA2Initialize();
}

void DMA1Initialize(void)
{
}

//dma2初始化
//377平台改为内部乒乓缓存几k的数据.然后通知DMA往SPIram的fifo中连续写入块数据.
//当停止录波时.关闭此DMA.并自动存储在spiflash中.
//当需要读取录波时用task方式读取spiflash
void DMA2Initialize(void)
{
    EALLOW;
	//用ONESHOT方式,burst长度为10,TRANSFER_COUNT=1
	DMACH2AddrConfig((volatile Uint16 *)debugW[0],(volatile Uint16 *)debugW[0]);	//Point DMA destination to the beginning of the array,//Point DMA source to ADC result
	DMACH2BurstConfig((DEBUG_WAVE_GROUP-1),1,DEBUG_WAVE_LEN-1);										//2 byte per burst
	DMACH2TransferConfig(DEBUG_WAVE_LEN-1,0,DEBUG_WAVE_LEN-1);	    //so need to increment by sizeof(float32) to grab the correct location
	DMACH2WrapConfig(0,0,DEBUG_WAVE_LEN-1,DEBUG_WAVE_LEN*(DEBUG_WAVE_GROUP));	  						//disable wrap
	DMACH2ModeConfig(0,PERINT_ENABLE,ONESHOT_DISABLE,CONT_DISABLE,SYNC_DISABLE,SYNC_SRC,
		             OVRFLOW_DISABLE,THIRTYTWO_BIT,CHINT_END,CHINT_DISABLE);
    EDIS;
}

//
// DMAInitialize - This function initializes the DMA to a known state.
//
//void DMAInitialize(void)
//{
//    EALLOW;
//
//    //
//    // Perform a hard reset on DMA
//    //
//    DmaRegs.DMACTRL.bit.HARDRESET = 1;
//   __asm (" nop"); // one NOP required after HARDRESET
//
//    //
//    // Allow DMA to run free on emulation suspend
//    //
//    DmaRegs.DEBUGCTRL.bit.FREE = 1;
//
//    EDIS;
//}

//
// DMACH1AddrConfig - DMA Channel 1 Address Configuration
//
void DMACH1AddrConfig(volatile Uint16 *DMA_Dest,volatile Uint16 *DMA_Source)
{
    EALLOW;

    //
    // Set up SOURCE address:
    //
    DmaRegs.CH1.SRC_BEG_ADDR_SHADOW = (Uint32)DMA_Source;   // Point to
                                                            // beginning of
                                                            // source buffer
    DmaRegs.CH1.SRC_ADDR_SHADOW =     (Uint32)DMA_Source;

    //
    // Set up DESTINATION address:
    //
    DmaRegs.CH1.DST_BEG_ADDR_SHADOW = (Uint32)DMA_Dest;  // Point to
                                                         // beginning of
                                                         // destination buffer
    DmaRegs.CH1.DST_ADDR_SHADOW =     (Uint32)DMA_Dest;

    EDIS;
}

//
// DMACH1BurstConfig - DMA Channel 1 Burst size configuration
//
void DMACH1BurstConfig(Uint16 bsize, int16 srcbstep, int16 desbstep)
{
    EALLOW;

    //
    // Set up BURST registers:
    //
    DmaRegs.CH1.BURST_SIZE.all = bsize;      // Number of words(X-1)
                                             // x-ferred in a burst.
    DmaRegs.CH1.SRC_BURST_STEP = srcbstep;   // Increment source addr between
                                             // each word x-ferred.
    DmaRegs.CH1.DST_BURST_STEP = desbstep;   // Increment dest addr between
                                             // each word x-ferred.

    EDIS;
}

//
// DMACH1TransferConfig - DMA Channel 1 Transfer size configuration
//
void DMACH1TransferConfig(Uint16 tsize, int16 srctstep, int16 deststep)
{
    EALLOW;

    //
    // Set up TRANSFER registers:
    //
    DmaRegs.CH1.TRANSFER_SIZE = tsize;        // Number of bursts per transfer,
                                              // DMA interrupt will occur after
                                              // completed transfer.
    DmaRegs.CH1.SRC_TRANSFER_STEP = srctstep; // TRANSFER_STEP is ignored
                                              // when WRAP occurs.
    DmaRegs.CH1.DST_TRANSFER_STEP = deststep; // TRANSFER_STEP is ignored
                                              // when WRAP occurs.

    EDIS;
}

//
// DMACH1WrapConfig - DMA Channel 1 Wrap size configuration
//
void DMACH1WrapConfig(Uint16 srcwsize, int16 srcwstep, Uint16 deswsize,
                      int16 deswstep)
{
    EALLOW;

    //
    // Set up WRAP registers:
    //
    DmaRegs.CH1.SRC_WRAP_SIZE = srcwsize; // Wrap source address after N bursts
    DmaRegs.CH1.SRC_WRAP_STEP = srcwstep; // Step for source wrap

    DmaRegs.CH1.DST_WRAP_SIZE = deswsize; // Wrap destination address after
                                          // N bursts.
    DmaRegs.CH1.DST_WRAP_STEP = deswstep; // Step for destination wrap

    EDIS;
}

//
// DMACH1ModeConfig - DMA Channel 1 Mode configuration
//
void DMACH1ModeConfig(Uint16 persel, Uint16 perinte, Uint16 oneshot,
                      Uint16 cont, Uint16 synce, Uint16 syncsel,
                      Uint16 ovrinte, Uint16 datasize, Uint16 chintmode,
                      Uint16 chinte)
{
    EALLOW;

    //
    // Set up MODE Register:
    // persel - Source select
    // PERINTSEL - Should be hard coded to channel, above now selects source
    // PERINTE - Peripheral interrupt enable
    // ONESHOT - Oneshot enable
    // CONTINUOUS - Continuous enable
    // OVRINTE - Enable/disable the overflow interrupt
    // DATASIZE - 16-bit/32-bit data size transfers
    // CHINTMODE - Generate interrupt to CPU at beginning/end of transfer
    // CHINTE - Channel Interrupt to  CPU enable
    //
    DmaClaSrcSelRegs.DMACHSRCSEL1.bit.CH1 = persel;
    DmaRegs.CH1.MODE.bit.PERINTSEL = 1;
    DmaRegs.CH1.MODE.bit.PERINTE = perinte;
    DmaRegs.CH1.MODE.bit.ONESHOT = oneshot;
    DmaRegs.CH1.MODE.bit.CONTINUOUS = cont;
    DmaRegs.CH1.MODE.bit.OVRINTE = ovrinte;
    DmaRegs.CH1.MODE.bit.DATASIZE = datasize;
    DmaRegs.CH1.MODE.bit.CHINTMODE = chintmode;
    DmaRegs.CH1.MODE.bit.CHINTE = chinte;

    //
    // Clear any spurious flags: interrupt and sync error flags
    //
    DmaRegs.CH1.CONTROL.bit.PERINTCLR = 1;
    DmaRegs.CH1.CONTROL.bit.ERRCLR = 1;

    //
    // Initialize PIE vector for CPU interrupt:
    // Enable DMA CH1 interrupt in PIE
    //
    PieCtrlRegs.PIEIER7.bit.INTx1 = 1;

    EDIS;
}

//
// StartDMACH1 - This function starts DMA Channel 1.
//
void StartDMACH1(void)
{
    EALLOW;
    DmaRegs.CH1.CONTROL.bit.RUN = 1;
    EDIS;
}

//
// DMACH2AddrConfig - DMA Channel 2 Address Configuration
//
void DMACH2AddrConfig(volatile Uint16 *DMA_Dest,volatile Uint16 *DMA_Source)
{
    EALLOW;

    //
    // Set up SOURCE address:
    //
    DmaRegs.CH2.SRC_BEG_ADDR_SHADOW = (Uint32)DMA_Source;   // Point to
                                                            // beginning of
                                                            // source buffer.
    DmaRegs.CH2.SRC_ADDR_SHADOW =     (Uint32)DMA_Source;

    //
    // Set up DESTINATION address:
    //
    DmaRegs.CH2.DST_BEG_ADDR_SHADOW = (Uint32)DMA_Dest;  // Point to beginning
                                                         // of destination
                                                         // buffer.
    DmaRegs.CH2.DST_ADDR_SHADOW =     (Uint32)DMA_Dest;

    EDIS;
}

//
// DMACH2BurstConfig - DMA Channel 2 Burst size configuration
//
void DMACH2BurstConfig(Uint16 bsize, int16 srcbstep, int16 desbstep)
{
    EALLOW;

    //
    // Set up BURST registers:
    //
    DmaRegs.CH2.BURST_SIZE.all = bsize;     // Number of words(X-1) x-ferred in
                                            // a burst.
    DmaRegs.CH2.SRC_BURST_STEP = srcbstep;  // Increment source addr between
                                            // each word x-ferred.
    DmaRegs.CH2.DST_BURST_STEP = desbstep;  // Increment dest addr between each
                                            // word x-ferred.

    EDIS;
}

//
// DMACH2TransferConfig - DMA Channel 2 Transfer size Configuration
//
void DMACH2TransferConfig(Uint16 tsize, int16 srctstep, int16 deststep)
{
    EALLOW;

    //
    // Set up TRANSFER registers:
    //
    DmaRegs.CH2.TRANSFER_SIZE = tsize;        // Number of bursts per transfer,
                                              // DMA interrupt will occur after
                                              // completed transfer.
    DmaRegs.CH2.SRC_TRANSFER_STEP = srctstep; // TRANSFER_STEP is ignored when
                                              // WRAP occurs.
    DmaRegs.CH2.DST_TRANSFER_STEP = deststep; // TRANSFER_STEP is ignored when
                                              // WRAP occurs.

    EDIS;
}

//
// DMACH2WrapConfig - DMA Channel 2 Wrap size configuration
//
void DMACH2WrapConfig(Uint16 srcwsize, int16 srcwstep, Uint16 deswsize,
                      int16 deswstep)
{
    EALLOW;

    //
    // Set up WRAP registers:
    //
    DmaRegs.CH2.SRC_WRAP_SIZE = srcwsize; // Wrap source address after N bursts
    DmaRegs.CH2.SRC_WRAP_STEP = srcwstep; // Step for source wrap

    DmaRegs.CH2.DST_WRAP_SIZE = deswsize; // Wrap destination address after
                                          // N bursts.
    DmaRegs.CH2.DST_WRAP_STEP = deswstep; // Step for destination wrap

    EDIS;
}

//
// DMACH2ModeConfig - DMA Channel 2 Mode configuration
//
void DMACH2ModeConfig(Uint16 persel, Uint16 perinte, Uint16 oneshot,
                      Uint16 cont, Uint16 synce, Uint16 syncsel,
                      Uint16 ovrinte, Uint16 datasize, Uint16 chintmode,
                      Uint16 chinte)
{
    EALLOW;

    //
    // Set up MODE Register:
    // persel - Source select
    // PERINTSEL - Should be hard coded to channel, above now selects source
    // PERINTE - Peripheral interrupt enable
    // ONESHOT - Oneshot enable
    // CONTINUOUS - Continuous enable
    // OVRINTE - Enable/disable the overflow interrupt
    // DATASIZE - 16-bit/32-bit data size transfers
    // CHINTMODE - Generate interrupt to CPU at beginning/end of transfer
    // CHINTE - Channel Interrupt to  CPU enable
    //
    DmaClaSrcSelRegs.DMACHSRCSEL1.bit.CH2 = persel;
    DmaRegs.CH2.MODE.bit.PERINTSEL = 2;
    DmaRegs.CH2.MODE.bit.PERINTE = perinte;
    DmaRegs.CH2.MODE.bit.ONESHOT = oneshot;
    DmaRegs.CH2.MODE.bit.CONTINUOUS = cont;
    DmaRegs.CH2.MODE.bit.OVRINTE = ovrinte;
    DmaRegs.CH2.MODE.bit.DATASIZE = datasize;
    DmaRegs.CH2.MODE.bit.CHINTMODE = chintmode;
    DmaRegs.CH2.MODE.bit.CHINTE = chinte;

    //
    // Clear any spurious flags: Interrupt flags and sync error flags
    //
    DmaRegs.CH2.CONTROL.bit.PERINTCLR = 1;
    DmaRegs.CH2.CONTROL.bit.ERRCLR = 1;

    //
    // Initialize PIE vector for CPU interrupt:
    // Enable DMA CH2 interrupt in PIE
    //
    PieCtrlRegs.PIEIER7.bit.INTx2 = 1;

    EDIS;
}

//
// StartDMACH2 - This function starts DMA Channel 2.
//
void StartDMACH2(void)
{
    EALLOW;
    DmaRegs.CH2.CONTROL.bit.RUN = 1;
    DmaRegs.CH2.CONTROL.bit.PERINTFRC = 1;
    EDIS;
}

//
// DMACH3AddrConfig - DMA Channel 3 Address configuration
//
void DMACH3AddrConfig(volatile Uint16 *DMA_Dest,volatile Uint16 *DMA_Source)
{
    EALLOW;

    //
    // Set up SOURCE address:
    //
    DmaRegs.CH3.SRC_BEG_ADDR_SHADOW = (Uint32)DMA_Source; // Point to beginning
                                                          // of source buffer.
    DmaRegs.CH3.SRC_ADDR_SHADOW =     (Uint32)DMA_Source;

    //
    // Set up DESTINATION address:
    //
    DmaRegs.CH3.DST_BEG_ADDR_SHADOW = (Uint32)DMA_Dest; // Point to beginning
                                                        // of destination
                                                        // buffer.
    DmaRegs.CH3.DST_ADDR_SHADOW =     (Uint32)DMA_Dest;

    EDIS;
}

//
// DMACH3BurstConfig - DMA Channel 3 burst size configuration
//
void DMACH3BurstConfig(Uint16 bsize, int16 srcbstep, int16 desbstep)
{
    EALLOW;

    //
    // Set up BURST registers:
    //
    DmaRegs.CH3.BURST_SIZE.all = bsize;     // Number of words(X-1) x-ferred in
                                            // a burst.
    DmaRegs.CH3.SRC_BURST_STEP = srcbstep;  // Increment source addr between
                                            // each word x-ferred.
    DmaRegs.CH3.DST_BURST_STEP = desbstep;  // Increment dest addr between each
                                            // word x-ferred.

    EDIS;
}

//
// DMACH3TransferConfig - DMA channel 3 transfer size configuration
//
void DMACH3TransferConfig(Uint16 tsize, int16 srctstep, int16 deststep)
{
    EALLOW;

    //
    // Set up TRANSFER registers:
    //
    DmaRegs.CH3.TRANSFER_SIZE = tsize;        // Number of bursts per transfer,
                                              // DMA interrupt will occur after
                                              // completed transfer.
    DmaRegs.CH3.SRC_TRANSFER_STEP = srctstep; // TRANSFER_STEP is ignored when
                                              // WRAP occurs.
    DmaRegs.CH3.DST_TRANSFER_STEP = deststep; // TRANSFER_STEP is ignored when
                                              // WRAP occurs.

    EDIS;
}

//
// DMACH3WrapConfig - DMA Channel 3 wrap size configuration
//
void DMACH3WrapConfig(Uint16 srcwsize, int16 srcwstep, Uint16 deswsize,
                      int16 deswstep)
{
    EALLOW;

    //
    // Set up WRAP registers:
    //
    DmaRegs.CH3.SRC_WRAP_SIZE = srcwsize; // Wrap source address after N bursts
    DmaRegs.CH3.SRC_WRAP_STEP = srcwstep; // Step for source wrap

    DmaRegs.CH3.DST_WRAP_SIZE = deswsize; // Wrap destination address after N
                                          // bursts.
    DmaRegs.CH3.DST_WRAP_STEP = deswstep; // Step for destination wrap

    EDIS;
}

//
// DMACH3ModeConfig - DMA Channel 3 mode configuration
//
void DMACH3ModeConfig(Uint16 persel, Uint16 perinte, Uint16 oneshot,
                      Uint16 cont, Uint16 synce, Uint16 syncsel,
                      Uint16 ovrinte, Uint16 datasize, Uint16 chintmode,
                      Uint16 chinte)
{
    EALLOW;

    //
    // Set up MODE Register:
    // persel - Source select
    // PERINTSEL - Should be hard coded to channel, above now selects source
    // PERINTE - Peripheral interrupt enable
    // ONESHOT - Oneshot enable
    // CONTINUOUS - Continuous enable
    // OVRINTE - Enable/disable the overflow interrupt
    // DATASIZE - 16-bit/32-bit data size transfers
    // CHINTMODE - Generate interrupt to CPU at beginning/end of transfer
    // CHINTE - Channel Interrupt to  CPU enable
    //
    DmaClaSrcSelRegs.DMACHSRCSEL1.bit.CH3 = persel;
    DmaRegs.CH3.MODE.bit.PERINTSEL = 3;
    DmaRegs.CH3.MODE.bit.PERINTE = perinte;
    DmaRegs.CH3.MODE.bit.ONESHOT = oneshot;
    DmaRegs.CH3.MODE.bit.CONTINUOUS = cont;
    DmaRegs.CH3.MODE.bit.OVRINTE = ovrinte;
    DmaRegs.CH3.MODE.bit.DATASIZE = datasize;
    DmaRegs.CH3.MODE.bit.CHINTMODE = chintmode;
    DmaRegs.CH3.MODE.bit.CHINTE = chinte;

    //
    // Clear any spurious flags: interrupt flags and sync error flags
    //
    DmaRegs.CH3.CONTROL.bit.PERINTCLR = 1;
    DmaRegs.CH3.CONTROL.bit.ERRCLR = 1;

    //
    // Initialize PIE vector for CPU interrupt:
    // Enable DMA CH3 interrupt in PIE
    //
    PieCtrlRegs.PIEIER7.bit.INTx3 = 1;

    EDIS;
}

//
// StartDMACH3 - This function starts DMA Channel 3.
//
void StartDMACH3(void)
{
    EALLOW;
    DmaRegs.CH3.CONTROL.bit.RUN = 1;
    EDIS;
}

//
// DMACH4AddrConfig - DMA Channel 4 address configuration
//
void DMACH4AddrConfig(volatile Uint16 *DMA_Dest,volatile Uint16 *DMA_Source)
{
    EALLOW;

    //
    // Set up SOURCE address:
    //
    DmaRegs.CH4.SRC_BEG_ADDR_SHADOW = (Uint32)DMA_Source; // Point to beginning
                                                          // of source buffer.
    DmaRegs.CH4.SRC_ADDR_SHADOW =     (Uint32)DMA_Source;

    //
    // Set up DESTINATION address:
    //
    DmaRegs.CH4.DST_BEG_ADDR_SHADOW = (Uint32)DMA_Dest;   // Point to beginning
                                                          // of destination
                                                          // buffer.
    DmaRegs.CH4.DST_ADDR_SHADOW =     (Uint32)DMA_Dest;

    EDIS;
}

//
// DMACH4BurstConfig - DMA Channel 4 burst size configuration
//
void DMACH4BurstConfig(Uint16 bsize, int16 srcbstep, int16 desbstep)
{
    EALLOW;

    //
    // Set up BURST registers:
    //
    DmaRegs.CH4.BURST_SIZE.all = bsize;     // Number of words(X-1) x-ferred in
                                            // a burst.
    DmaRegs.CH4.SRC_BURST_STEP = srcbstep;  // Increment source addr between
                                            // each word x-ferred.
    DmaRegs.CH4.DST_BURST_STEP = desbstep;  // Increment dest addr between each
                                            // word x-ferred.

    EDIS;
}

//
// DMACH4TransferConfig - DMA channel 4 transfer size configuration
//
void DMACH4TransferConfig(Uint16 tsize, int16 srctstep, int16 deststep)
{
    EALLOW;

    //
    // Set up TRANSFER registers:
    //
    DmaRegs.CH4.TRANSFER_SIZE = tsize;        // Number of bursts per transfer,
                                              // DMA interrupt will occur after
                                              // completed transfer.
    DmaRegs.CH4.SRC_TRANSFER_STEP = srctstep; // TRANSFER_STEP is ignored when
                                              // WRAP occurs.
    DmaRegs.CH4.DST_TRANSFER_STEP = deststep; // TRANSFER_STEP is ignored when
                                              // WRAP occurs.

    EDIS;
}

//
// DMACH4WrapConfig - DMA channel 4 wrap size configuration
//
void DMACH4WrapConfig(Uint16 srcwsize, int16 srcwstep, Uint16 deswsize,
                      int16 deswstep)
{
    EALLOW;

    //
    // Set up WRAP registers:
    //
    DmaRegs.CH4.SRC_WRAP_SIZE = srcwsize; // Wrap source address after N bursts
    DmaRegs.CH4.SRC_WRAP_STEP = srcwstep; // Step for source wrap

    DmaRegs.CH4.DST_WRAP_SIZE = deswsize; // Wrap destination address after
                                          // N bursts.
    DmaRegs.CH4.DST_WRAP_STEP = deswstep; // Step for destination wrap

    EDIS;
}

//
// DMACH4ModeConfig - DMA Channel 4 mode configuration
//
void DMACH4ModeConfig(Uint16 persel, Uint16 perinte, Uint16 oneshot,
                      Uint16 cont, Uint16 synce, Uint16 syncsel,
                      Uint16 ovrinte, Uint16 datasize, Uint16 chintmode,
                      Uint16 chinte)
{
    EALLOW;

    //
    // Set up MODE Register:
    // persel - Source select
    // PERINTSEL - Should be hard coded to channel, above now selects source
    // PERINTE - Peripheral interrupt enable
    // ONESHOT - Oneshot enable
    // CONTINUOUS - Continuous enable
    // OVRINTE - Enable/disable the overflow interrupt
    // DATASIZE - 16-bit/32-bit data size transfers
    // CHINTMODE - Generate interrupt to CPU at beginning/end of transfer
    // CHINTE - Channel Interrupt to  CPU enable
    //
    DmaClaSrcSelRegs.DMACHSRCSEL1.bit.CH4 = persel;
    DmaRegs.CH4.MODE.bit.PERINTSEL = 4;
    DmaRegs.CH4.MODE.bit.PERINTE = perinte;
    DmaRegs.CH4.MODE.bit.ONESHOT = oneshot;
    DmaRegs.CH4.MODE.bit.CONTINUOUS = cont;
    DmaRegs.CH4.MODE.bit.OVRINTE = ovrinte;
    DmaRegs.CH4.MODE.bit.DATASIZE = datasize;
    DmaRegs.CH4.MODE.bit.CHINTMODE = chintmode;
    DmaRegs.CH4.MODE.bit.CHINTE = chinte;

    //
    // Clear any spurious flags: Interrupt flags and sync error flags
    //
    DmaRegs.CH4.CONTROL.bit.PERINTCLR = 1;
    DmaRegs.CH4.CONTROL.bit.ERRCLR = 1;

    //
    // Initialize PIE vector for CPU interrupt:
    // Enable DMA CH4 interrupt in PIE
    //
    PieCtrlRegs.PIEIER7.bit.INTx4 = 1;

    EDIS;
}

//
// StartDMACH4 - This function starts DMA Channel 4.
//
void StartDMACH4(void)
{
    EALLOW;
    DmaRegs.CH4.CONTROL.bit.RUN = 1;
    EDIS;
}

//
// DMACH5AddrConfig - DMA channel 5 address configuration
//
void DMACH5AddrConfig(volatile Uint16 *DMA_Dest,volatile Uint16 *DMA_Source)
{
    EALLOW;

    //
    // Set up SOURCE address:
    //
    DmaRegs.CH5.SRC_BEG_ADDR_SHADOW = (Uint32)DMA_Source; // Point to beginning
                                                          // of source buffer
    DmaRegs.CH5.SRC_ADDR_SHADOW =     (Uint32)DMA_Source;

    //
    // Set up DESTINATION address:
    //
    DmaRegs.CH5.DST_BEG_ADDR_SHADOW = (Uint32)DMA_Dest;  // Point to beginning
                                                         // of destination
                                                         // buffer.
    DmaRegs.CH5.DST_ADDR_SHADOW =     (Uint32)DMA_Dest;

    EDIS;
}

//
// DMACH5BurstConfig - DMA Channel 5 burst size configuration
//
void DMACH5BurstConfig(Uint16 bsize, int16 srcbstep, int16 desbstep)
{
    EALLOW;

    //
    // Set up BURST registers:
    //
    DmaRegs.CH5.BURST_SIZE.all = bsize;     // Number of words(X-1) x-ferred in
                                            // a burst.
    DmaRegs.CH5.SRC_BURST_STEP = srcbstep;  // Increment source addr between
                                            // each word x-ferred.
    DmaRegs.CH5.DST_BURST_STEP = desbstep;  // Increment dest addr between each
                                            // word x-ferred.

    EDIS;
}

//
// DMACH5TransferConfig - DMA channel 5 transfer size configuration
//
void DMACH5TransferConfig(Uint16 tsize, int16 srctstep, int16 deststep)
{
    EALLOW;

    //
    // Set up TRANSFER registers:
    //
    DmaRegs.CH5.TRANSFER_SIZE = tsize;        // Number of bursts per transfer,
                                              // DMA interrupt will occur after
                                              // completed transfer.
    DmaRegs.CH5.SRC_TRANSFER_STEP = srctstep; // TRANSFER_STEP is ignored when
                                              // WRAP occurs.
    DmaRegs.CH5.DST_TRANSFER_STEP = deststep; // TRANSFER_STEP is ignored when
                                              // WRAP occurs.

    EDIS;
}

//
// DMACH5WrapConfig - DMA Channel 5 wrap size configuration
//
void DMACH5WrapConfig(Uint16 srcwsize, int16 srcwstep, Uint16 deswsize,
                      int16 deswstep)
{
    EALLOW;

    //
    // Set up WRAP registers:
    //
    DmaRegs.CH5.SRC_WRAP_SIZE = srcwsize; // Wrap source address after N bursts
    DmaRegs.CH5.SRC_WRAP_STEP = srcwstep; // Step for source wrap

    DmaRegs.CH5.DST_WRAP_SIZE = deswsize; // Wrap destination address after
                                          // N bursts.
    DmaRegs.CH5.DST_WRAP_STEP = deswstep; // Step for destination wrap

    EDIS;
}

//
// DMACH5ModeConfig - DMA Channel 5 mode configuration
//
void DMACH5ModeConfig(Uint16 persel, Uint16 perinte, Uint16 oneshot,
                      Uint16 cont, Uint16 synce, Uint16 syncsel,
                      Uint16 ovrinte, Uint16 datasize, Uint16 chintmode,
                      Uint16 chinte)
{
    EALLOW;

    //
    // Set up MODE Register:
    // persel - Source select
    // PERINTSEL - Should be hard coded to channel, above now selects source
    // PERINTE - Peripheral interrupt enable
    // ONESHOT - Oneshot enable
    // CONTINUOUS - Continuous enable
    // OVRINTE - Enable/disable the overflow interrupt
    // DATASIZE - 16-bit/32-bit data size transfers
    // CHINTMODE - Generate interrupt to CPU at beginning/end of transfer
    // CHINTE - Channel Interrupt to  CPU enable
    //
    DmaClaSrcSelRegs.DMACHSRCSEL2.bit.CH5 = persel;
    DmaRegs.CH5.MODE.bit.PERINTSEL = 5;
    DmaRegs.CH5.MODE.bit.PERINTE = perinte;
    DmaRegs.CH5.MODE.bit.ONESHOT = oneshot;
    DmaRegs.CH5.MODE.bit.CONTINUOUS = cont;
    DmaRegs.CH5.MODE.bit.OVRINTE = ovrinte;
    DmaRegs.CH5.MODE.bit.DATASIZE = datasize;
    DmaRegs.CH5.MODE.bit.CHINTMODE = chintmode;
    DmaRegs.CH5.MODE.bit.CHINTE = chinte;

    //
    // Clear any spurious flags: Interrupt flags and sync error flags
    //
    DmaRegs.CH5.CONTROL.bit.PERINTCLR = 1;
    DmaRegs.CH5.CONTROL.bit.ERRCLR = 1;

    //
    // Initialize PIE vector for CPU interrupt:
    // Enable DMA CH5 interrupt in PIE
    //
    PieCtrlRegs.PIEIER7.bit.INTx5 = 1;

    EDIS;
}

//
// StartDMACH5 - This function starts DMA Channel 5.
//
void StartDMACH5(void)
{
    EALLOW;
    DmaRegs.CH5.CONTROL.bit.RUN = 1;
    EDIS;
}

//
// DMACH6AddrConfig - DMA Channel 6 address configuration
//
void DMACH6AddrConfig(volatile Uint16 *DMA_Dest,volatile Uint16 *DMA_Source)
{
    EALLOW;

    //
    // Set up SOURCE address:
    //
    DmaRegs.CH6.SRC_BEG_ADDR_SHADOW = (Uint32)DMA_Source; // Point to beginning
                                                          // of source buffer.
    DmaRegs.CH6.SRC_ADDR_SHADOW =     (Uint32)DMA_Source;

    //
    // Set up DESTINATION address:
    //
    DmaRegs.CH6.DST_BEG_ADDR_SHADOW = (Uint32)DMA_Dest;  // Point to beginning
                                                         // of destination
                                                         // buffer.
    DmaRegs.CH6.DST_ADDR_SHADOW =     (Uint32)DMA_Dest;

    EDIS;
}

//
// DMACH6BurstConfig - DMA Channel 6 burst size configuration
//
void DMACH6BurstConfig(Uint16 bsize,Uint16 srcbstep, int16 desbstep)
{
    EALLOW;

    //
    // Set up BURST registers:
    //
    DmaRegs.CH6.BURST_SIZE.all = bsize;     // Number of words(X-1) x-ferred in
                                            // a burst.
    DmaRegs.CH6.SRC_BURST_STEP = srcbstep;  // Increment source addr between
                                            // each word x-ferred.
    DmaRegs.CH6.DST_BURST_STEP = desbstep;  // Increment dest addr between each
                                            // word x-ferred.

    EDIS;
}

//
// DMACH6TransferConfig - DMA channel 6 transfer size configuration
//
void DMACH6TransferConfig(Uint16 tsize, int16 srctstep, int16 deststep)
{
    EALLOW;

    //
    // Set up TRANSFER registers:
    //
    DmaRegs.CH6.TRANSFER_SIZE = tsize;        // Number of bursts per transfer,
                                              // DMA interrupt will occur after
                                              // completed transfer.
    DmaRegs.CH6.SRC_TRANSFER_STEP = srctstep; // TRANSFER_STEP is ignored when
                                              // WRAP occurs.
    DmaRegs.CH6.DST_TRANSFER_STEP = deststep; // TRANSFER_STEP is ignored when
                                              // WRAP occurs.

    EDIS;
}

//
// DMACH6WrapConfig - DMA Channel 6 wrap size configuration
//
void DMACH6WrapConfig(Uint16 srcwsize, int16 srcwstep, Uint16 deswsize,
                      int16 deswstep)
{
    EALLOW;

    //
    // Set up WRAP registers:
    //
    DmaRegs.CH6.SRC_WRAP_SIZE = srcwsize; // Wrap source address after N bursts
    DmaRegs.CH6.SRC_WRAP_STEP = srcwstep; // Step for source wrap

    DmaRegs.CH6.DST_WRAP_SIZE = deswsize; // Wrap destination address after N
                                          // bursts.
    DmaRegs.CH6.DST_WRAP_STEP = deswstep; // Step for destination wrap

    EDIS;
}

//
// DMACH6ModeConfig - DMA Channel 6 mode configuration
//
void DMACH6ModeConfig(Uint16 persel, Uint16 perinte, Uint16 oneshot,
                      Uint16 cont, Uint16 synce, Uint16 syncsel,
                      Uint16 ovrinte, Uint16 datasize, Uint16 chintmode,
                      Uint16 chinte)
{
    EALLOW;

    //
    // Set up MODE Register:
    // persel - Source select
    // PERINTSEL - Should be hard coded to channel, above now selects source
    // PERINTE - Peripheral interrupt enable
    // ONESHOT - Oneshot enable
    // CONTINUOUS - Continuous enable
    // OVRINTE - Enable/disable the overflow interrupt
    // DATASIZE - 16-bit/32-bit data size transfers
    // CHINTMODE - Generate interrupt to CPU at beginning/end of transfer
    // CHINTE - Channel Interrupt to  CPU enable
    //
    DmaClaSrcSelRegs.DMACHSRCSEL2.bit.CH6 = persel;
    DmaRegs.CH6.MODE.bit.PERINTSEL = 6;
    DmaRegs.CH6.MODE.bit.PERINTE = perinte;
    DmaRegs.CH6.MODE.bit.ONESHOT = oneshot;
    DmaRegs.CH6.MODE.bit.CONTINUOUS = cont;
    DmaRegs.CH6.MODE.bit.OVRINTE = ovrinte;
    DmaRegs.CH6.MODE.bit.DATASIZE = datasize;
    DmaRegs.CH6.MODE.bit.CHINTMODE = chintmode;
    DmaRegs.CH6.MODE.bit.CHINTE = chinte;

    //
    // Clear any spurious flags: Interrupt flags and sync error flags
    //
    DmaRegs.CH6.CONTROL.bit.PERINTCLR = 1;
    DmaRegs.CH6.CONTROL.bit.ERRCLR = 1;

    //
    // Initialize PIE vector for CPU interrupt:
    // Enable DMA CH6 interrupt in PIE
    //
    PieCtrlRegs.PIEIER7.bit.INTx6 = 1;

    EDIS;
}

//
// StartDMACH6 - This function starts DMA Channel 6.
//
void StartDMACH6(void)
{
    EALLOW;
    DmaRegs.CH6.CONTROL.bit.RUN = 1;
    EDIS;
}

//
// NOTE:
// Following functions are required for EMIF as the address is out of
// 22bit range
//

//
// DMACH1AddrConfig32bit - DMA Channel 1 address configuration for 32bit
//
void DMACH1AddrConfig32bit(volatile Uint32 *DMA_Dest,
                           volatile Uint32 *DMA_Source)
{
    EALLOW;

    //
    // Set up SOURCE address:
    //
    DmaRegs.CH1.SRC_BEG_ADDR_SHADOW = (Uint32)DMA_Source; // Point to beginning
                                                          // of source buffer
    DmaRegs.CH1.SRC_ADDR_SHADOW =     (Uint32)DMA_Source;

    //
    // Set up DESTINATION address:
    //
    DmaRegs.CH1.DST_BEG_ADDR_SHADOW = (Uint32)DMA_Dest;  // Point to beginning
                                                         // of destination
                                                         // buffer
    DmaRegs.CH1.DST_ADDR_SHADOW =     (Uint32)DMA_Dest;

    EDIS;
}

//
// DMACH2AddrConfig32bit - DMA Channel 2 address configuration for 32bit
//
void DMACH2AddrConfig32bit(volatile Uint32 *DMA_Dest,
                           volatile Uint32 *DMA_Source)
{
    EALLOW;

    //
    // Set up SOURCE address:
    //
    DmaRegs.CH2.SRC_BEG_ADDR_SHADOW = (Uint32)DMA_Source; // Point to beginning
                                                          // of source buffer
    DmaRegs.CH2.SRC_ADDR_SHADOW =     (Uint32)DMA_Source;

    //
    // Set up DESTINATION address:
    //
    DmaRegs.CH2.DST_BEG_ADDR_SHADOW = (Uint32)DMA_Dest;  // Point to beginning
                                                         // of destination
                                                         // buffer
    DmaRegs.CH2.DST_ADDR_SHADOW =     (Uint32)DMA_Dest;

    EDIS;
}

//
// DMACH3AddrConfig32bit - DMA Channel 3 address configuration for 32bit
//
void DMACH3AddrConfig32bit(volatile Uint32 *DMA_Dest,
                           volatile Uint32 *DMA_Source)
{
    EALLOW;

    //
    // Set up SOURCE address:
    //
    DmaRegs.CH3.SRC_BEG_ADDR_SHADOW = (Uint32)DMA_Source; // Point to beginning
                                                          // of source buffer
    DmaRegs.CH3.SRC_ADDR_SHADOW =     (Uint32)DMA_Source;

    //
    // Set up DESTINATION address:
    //
    DmaRegs.CH3.DST_BEG_ADDR_SHADOW = (Uint32)DMA_Dest;  // Point to beginning
                                                         // of destination
                                                         // buffer.
    DmaRegs.CH3.DST_ADDR_SHADOW =     (Uint32)DMA_Dest;

    EDIS;
}

//
// DMACH4AddrConfig32bit - DMA Channel 4 address configuration for 32bit
//
void DMACH4AddrConfig32bit(volatile Uint32 *DMA_Dest,
                           volatile Uint32 *DMA_Source)
{
    EALLOW;

    //
    // Set up SOURCE address:
    //
    DmaRegs.CH4.SRC_BEG_ADDR_SHADOW = (Uint32)DMA_Source; // Point to beginning
                                                          // of source buffer
    DmaRegs.CH4.SRC_ADDR_SHADOW =     (Uint32)DMA_Source;

    //
    // Set up DESTINATION address:
    //
    DmaRegs.CH4.DST_BEG_ADDR_SHADOW = (Uint32)DMA_Dest;   // Point to beginning
                                                          // of destination
                                                          // buffer
    DmaRegs.CH4.DST_ADDR_SHADOW =     (Uint32)DMA_Dest;

    EDIS;
}

//
// DMACH5AddrConfig32bit - DMA Channel 5 address configuration for 32bit
//
void DMACH5AddrConfig32bit(volatile Uint32 *DMA_Dest,
                           volatile Uint32 *DMA_Source)
{
    EALLOW;

    //
    // Set up SOURCE address:
    //
    DmaRegs.CH5.SRC_BEG_ADDR_SHADOW = (Uint32)DMA_Source; // Point to beginning
                                                          // of source buffer
    DmaRegs.CH5.SRC_ADDR_SHADOW =     (Uint32)DMA_Source;

    //
    // Set up DESTINATION address:
    //
    DmaRegs.CH5.DST_BEG_ADDR_SHADOW = (Uint32)DMA_Dest;   // Point to beginning
                                                          // of destination
                                                          // buffer
    DmaRegs.CH5.DST_ADDR_SHADOW =     (Uint32)DMA_Dest;

    EDIS;
}

//
// DMACH6AddrConfig32bit - DMA Channel 6 address configuration for 32bit
//
void DMACH6AddrConfig32bit(volatile Uint32 *DMA_Dest,
                           volatile Uint32 *DMA_Source)
{
    EALLOW;

    //
    // Set up SOURCE address:
    //
    DmaRegs.CH6.SRC_BEG_ADDR_SHADOW = (Uint32)DMA_Source; // Point to beginning
                                                          // of source buffer
    DmaRegs.CH6.SRC_ADDR_SHADOW =     (Uint32)DMA_Source;

    //
    // Set up DESTINATION address:
    //
    DmaRegs.CH6.DST_BEG_ADDR_SHADOW = (Uint32)DMA_Dest;   // Point to beginning
                                                          // of destination
                                                          // buffer
    DmaRegs.CH6.DST_ADDR_SHADOW =     (Uint32)DMA_Dest;

    EDIS;
}

//
// End of file
//
