/*
 * Copyright (c) 2015-2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 *  ======== TMS320F28377S.cmd ========
 *  Define the memory block start/length for the TMS320F28377S
 */

// Set this variable to 1 to use the CLA Math Tables in the CLA Data ROM
// If set to 0, make sure the right CLA Math library (one without the _datarom
// suffix) is used in the project
--define=CLA_MATH_TABLES_IN_ROM=1

MEMORY
{
PAGE 0 :  /* Program Memory */
          /* BEGIN is used for the "boot to FLASH" bootloader mode   */

    D1GS03   : origin = 0x00A000, length = 0x004000 /* on-chip RAM */
	LS25SRAM : origin = 0x008800, length = 0x001000

    /* Flash boot address */
    BEGIN   : origin = 0x080000, length = 0x000008
    RESET   : origin = 0x3FFFC0, length = 0x000002

    /* Flash sectors */
    FLASHA  : origin = 0x080008, length = 0x001FF0  /* boot Flash */
    FLASHB  : origin = 0x082000, length = 0x002000  /* code Flash */
    FLASHCG : origin = 0x084000, length = 0x01C000  /* code Flash */
//  FLASHC  : origin = 0x084000, length = 0x002000  /* code Flash */
//  FLASHD  : origin = 0x086000, length = 0x002000  /* code Flash */
//  FLASHE  : origin = 0x088000, length = 0x008000  /* code Flash */
//  FLASHF  : origin = 0x090000, length = 0x008000  /* code Flash */
//  FLASHG  : origin = 0x098000, length = 0x008000  /* code Flash */
    FLASHH  : origin = 0x0A0000, length = 0x008000  /* on-chip Flash */
    FLASHI  : origin = 0x0A8000, length = 0x008000  /* on-chip Flash */
    FLASHJ  : origin = 0x0B0000, length = 0x008000  /* on-chip Flash */
    FLASHK  : origin = 0x0B8000, length = 0x002000  /* on-chip Flash */
    FLASHL  : origin = 0x0BA000, length = 0x002000  /* on-chip Flash */
    FLASHM  : origin = 0x0BC000, length = 0x002000  /* on-chip Flash */
    FLASHN  : origin = 0x0BE000, length = 0x002000  /* on-chip Flash */
    FLASHOAA : origin = 0x0C0000, length = 0x03e000	/* on-chip Flash FLASHO~FLASHAB=0X40000=64000*4.096*/
//  FLASHO  : origin = 0x0C0000, length = 0x002000	/* on-chip Flash */
//  FLASHP  : origin = 0x0C2000, length = 0x002000	/* on-chip Flash */
//  FLASHQ  : origin = 0x0C4000, length = 0x002000	/* on-chip Flash */
//  FLASHR  : origin = 0x0C6000, length = 0x002000	/* on-chip Flash */
//  FLASHS  : origin = 0x0C8000, length = 0x008000	/* on-chip Flash */
//  FLASHT  : origin = 0x0D0000, length = 0x008000	/* on-chip Flash */
//  FLASHU  : origin = 0x0D8000, length = 0x008000	/* on-chip Flash */
//  FLASHV  : origin = 0x0E0000, length = 0x008000	/* on-chip Flash */
//  FLASHW  : origin = 0x0E8000, length = 0x008000	/* on-chip Flash */
//  FLASHX  : origin = 0x0F0000, length = 0x008000	/* on-chip Flash */
//  FLASHY  : origin = 0x0F8000, length = 0x002000	/* on-chip Flash */
//  FLASHZ  : origin = 0x0FA000, length = 0x002000	/* on-chip Flash */
//  FLASHAA : origin = 0x0FC000, length = 0x002000	/* on-chip Flash */
	FLASHAB : origin = 0x0FE000, length = 0x002000	/* on-chip Flash */
 
//   IQTABLES    : origin = 0x3FE000, length = 0x000b50     /* IQ Math Tables in Boot ROM */
//   IQTABLES2   : origin = 0x3FEB50, length = 0x00008c     /* IQ Math Tables in Boot ROM */
     FPUTABLES   : origin = 0x3FEBDC, length = 0x0006A0     /* FPU Tables in Boot ROM */
//   CLA1TABLES	 : origin = 0x01001870, length = 0x00000784     /* FPU Tables in Boot ROM */

//  BootROM   	 : origin = 0x3FF27C, length = 0x000D44  	  /* Boot ROM */
//   VECTORS     : origin = 0x3FFFC2, length = 0x00003E     /* part of boot ROM  */

//   CSM_RSVD   : origin = 0x33FF80, length = 0x000076    /*   Part of FLASHA.  Program with all 0x0000 when CSM is in use. */

//   CSM_PWL    : origin = 0x33FFF8, length = 0x000008     /*   Part of FLASHA.  CSM password locations in FLASHA            */
//   OTP        : origin = 0x380400, length = 0x000400     /* on-chip OTP  */
//   ADC_CAL    : origin = 0x380080, length = 0x000009

PAGE 1 : /* Data Memory */

    BOOT_RSVD : origin = 0x000002, length = 0x000050 /* Part of M0, BOOT rom will use this for stack */

//    M01SARAM : origin = 0x000400, length = 0x000400
    M01SARAM : origin = 0x000052, length = 0x00074E  /* on-chip RAM */
    D0SARAM  : origin = 0x009800, length = 0x000800

    RAMLS01  : origin = 0x008000, length = 0x000800
//	RAMLS1   : origin = 0x008800, length = 0x000800
//  LS01SARAM : origin = 0x008000, length = 0x001000 /* on-chip RAM */
    /* on-chip Global shared RAMs */
    RAMGS48 : origin = 0x00E000, length = 0x009800
//  RAMGS0  : origin = 0x00C000, length = 0x001000
//  RAMGS1  : origin = 0x00D000, length = 0x001000
//  RAMGS2  : origin = 0x00E000, length = 0x001000
//  RAMGS3  : origin = 0x00F000, length = 0x001000
//  RAMGS4  : origin = 0x010000, length = 0x001000
//  RAMGS5  : origin = 0x011000, length = 0x001000
//  RAMGS6  : origin = 0x012000, length = 0x001000
//  RAMGS7  : origin = 0x013000, length = 0x001000
//  RAMGS8  : origin = 0x014000, length = 0x001000
//  RAMGS9  : origin = 0x015000, length = 0x001000
//  RAMGS10 : origin = 0x016000, length = 0x001000
//  RAMGS11 : origin = 0x017000, length = 0x001000
//  RAMGS12 : origin = 0x018000, length = 0x001000
//  RAMGS13 : origin = 0x019000, length = 0x001000
//  RAMGS14 : origin = 0x01A000, length = 0x001000
//  RAMGS15 : origin = 0x01B000, length = 0x001000
	RAMGS9F : origin = 0x017800, length = 0x004800
 
   EMIF1_CS0n       : origin = 0x80000000, length = 0x10000000
   EMIF1_CS2n       : origin = 0x00100000, length = 0x00200000
   EMIF1_CS3n       : origin = 0x00300000, length = 0x00080000
   EMIF1_CS4n       : origin = 0x00380000, length = 0x00060000
   EMIF2_CS0n       : origin = 0x90000000, length = 0x10000000
   EMIF2_CS2n       : origin = 0x00002000, length = 0x00001000

   CPU2TOCPU1RAM   : origin = 0x03F800, length = 0x000400
   CPU1TOCPU2RAM   : origin = 0x03FC00, length = 0x000400

   CLA1_MSGRAMLOW   : origin = 0x001480, length = 0x000080
   CLA1_MSGRAMHIGH  : origin = 0x001500, length = 0x000080
}


SECTIONS
{
    /* Allocate program areas: */
    .cinit              : > FLASHCG PAGE = 0, ALIGN(4)
    .pinit              : > FLASHCG PAGE = 0, ALIGN(4)
    .binit              : > FLASHCG PAGE = 0, ALIGN(4)
    .text               : > FLASHCG PAGE = 0, ALIGN(4)
	codestart           : > BEGIN   PAGE = 0, ALIGN(4)

  /* Allocate uninitalized data sections: */
    .stack              : > M01SARAM PAGE = 1
    .ebss               : > RAMGS48 PAGE = 1
    .esysmem            : > RAMGS48 PAGE = 1
  
  /* Initalized sections go in Flash */
    .econst             : > FLASHCG PAGE = 0, ALIGN(4)
    .switch             : > FLASHCG PAGE = 0, ALIGN(4)
 
    .reset              : > RESET,  PAGE = 0, TYPE = DSECT /* not used, */
  
	Filter_RegsFile  : > RAMLS01,	   PAGE = 1
 
    .em2_cs0         : > EMIF2_CS0n, PAGE = 1
    .em2_cs2         : > EMIF2_CS2n, PAGE = 1

    .sysmem          : > RAMGS48,    PAGE = 1
    .cio             : > RAMGS48,    PAGE = 1

  /* CLA specific sections */
    Cla1Prog         : LOAD = FLASHCG,
                      RUN = LS25SRAM,
                      LOAD_START(_Cla1funcsLoadStart),
                      LOAD_END(_Cla1funcsLoadEnd),
                      RUN_START(_Cla1funcsRunStart),
                      PAGE = 0  ALIGN(4)

   CLADataLS01		: > RAMLS01, PAGE=1

   Cla1ToCpuMsgRAM  : > CLA1_MSGRAMLOW,   PAGE = 1
   CpuToCla1MsgRAM  : > CLA1_MSGRAMHIGH,  PAGE = 1
   .ebss:taskStackSection:  > D0SARAM	,  PAGE = 1

#ifdef __TI_COMPILER_VERSION__
   #if __TI_COMPILER_VERSION__ >= 15009000
    .TI.ramfunc : {} LOAD = FLASHCG,
						 RUN = D1GS03,
                         LOAD_START(_RamfuncsLoadStart),
                         LOAD_END(_RamfuncsLoadEnd),
                         RUN_START(_RamfuncsRunStart),
						 PAGE = 0, ALIGN(4)
   #else
   ramfuncs         : LOAD = FLASHCG,
                      RUN = D1GS03,
                      LOAD_START(_RamfuncsLoadStart),
                      LOAD_END(_RamfuncsLoadEnd),
                      RUN_START(_RamfuncsRunStart),
                      PAGE = 1, ALIGN(4)
   #endif
#endif
   ram2func         :
	                  LOAD = FLASHCG,
                      RUN = D1GS03,
                      LOAD_START(_ram2funcLoadStart),
                      LOAD_END(_ram2funcLoadEnd),
                      RUN_START(_ram2funcRunStart),
                      PAGE = 0

/*	.asmfunc : 	  	  LOAD = FLASHCG, {--library=rts2800_fpu32_fast_supplement.lib}
                      RUN = D1GS03,
                      LOAD_START(_asmfuncLoadStart),
                      LOAD_END(_asmfuncLoadEnd),
                      RUN_START(_asmfuncRunStart),
                      PAGE = 0*/
//                	  --library=rts2800_fpu32_fast_supplement_coff.lib<sincos_f32.obj> (.text)
//               	  	  --library=rts2800_fpu32_fast_supplement_coff.lib<div_f32.obj> (.text)	//因为硬件锁相用到除法,如果在RAM中则因未load进ram跑飞.


  FPUmathTables    	: LOAD = FLASHCG,
                      RUN = D1GS03,
                      LOAD_START(_mathTablesLoadStart),
                      LOAD_END(_mathTablesLoadEnd),
                      RUN_START(_mathTablesRunStart),
                      PAGE = 0

#if  !(CLA_MATH_TABLES_IN_ROM)
   //Load tables to Flash and copy over to RAM
   /* CLA Math Tables in ROM*/
   CLA1mathTables   :  LOAD = FLASHCG,
                       RUN = LS25SRAM,
                       RUN_START(_CLA1mathTablesRunStart),
                       LOAD_START(_CLA1mathTablesLoadStart),
                       LOAD_END(_CLA1mathTablesLoadEnd),
                       PAGE = 0
#endif //CLA_MATH_TABLES_IN_ROM
   dclfuncs         : LOAD = FLASHCG ,
                      RUN = D1GS03,
                      LOAD_START(_dclfuncsLoadStart),
                      LOAD_END(_dclfuncsLoadEnd),
                      RUN_START(_dclfuncsRunStart),
                      PAGE = 0


/* --------------------------RAM和FLASH的CMD都一致------------------------------*/
	RFFTin	        : > RAMGS48,      PAGE = 1, ALIGN(512)		/*The input buffer must be aligned to a multiple of the 2*FFTSize*/
//	RFFTcoef         : > M01SARAM,     PAGE = 1
//	RFFTout          : > RAML6,        PAGE = 1
	EBSS2				: > D0SARAM,      PAGE = 1
 	WAVEDATA1           : > RAMGS9F PAGE = 1
 	WAVEDATA2           : > FLASHB  PAGE = 0
 	EBSS1               : > M01SARAM PAGE = 1
 	EBSS3               : > RAMLS01 PAGE = 1


   /* The following section definition are for SDFM examples */
   Filter1_RegsFile : > RAMGS48,	PAGE = 1, fill=0x1111
   Filter2_RegsFile : > RAMGS48,	PAGE = 1, fill=0x2222
   Filter3_RegsFile : > RAMGS48,	PAGE = 1, fill=0x3333
   Filter4_RegsFile : > RAMGS48,	PAGE = 1, fill=0x4444

#ifdef CLA_C
   /* CLA C compiler sections */
   //
   // Must be allocated to memory the CLA has write access to
   //
   CLAscratch       :
                     { *.obj(CLAscratch)
                     . += CLA_SCRATCHPAD_SIZE;
                     *.obj(CLAscratch_end) } >  RAMLS01,  PAGE = 1

   .scratchpad      : > RAMLS01,       PAGE = 1
   .bss_cla		    : > RAMLS01,       PAGE = 1
   .const_cla	    :  LOAD = FLASHB,
                       RUN = LS25SRAM,
                       LOAD_START(_Cla1ConstLoadStart),
                       LOAD_END(_Cla1ConstLoadEnd),
                       RUN_START(_Cla1ConstRunStart),
                       PAGE = 0
#endif //CLA_C


.knl: 		{ *.*(.text:*ti_sysbios_knl*) 	*.*(.text:*ti_sysbios*_Hwi_*) 	*.*(.text:*ti_sysbios_gates_*)}
  					  LOAD = FLASHCG,
                      RUN = D1GS03,
                      LOAD_START(_knlLoadStart),
                      LOAD_END(_knlLoadEnd),
                      RUN_START(_knlRunStart),
                      PAGE = 0
}

_CLAsinTable 						= 0xFd0a;	//see doc TMS320F2837xS Delfino Microcontrollers Technical Reference Manual_spruhx5c.pdf page534
_CLAsincosTable						= 0xFd0a;
_CLAsincosTable_Sin0				= 0xFd0a;
_CLAcosTable 						= 0xFd4a;
_CLAsincosTable_Cos0 				= 0xFd4a;
_CLAsincosTable_Coef0 				= 0xFe54;
_CLAsincosTable_Coef1 				= 0xFe56;
_CLAsincosTable_Coef2 				= 0xFe5a;
_CLAsincosTable_Coef3 				= 0xFe5c;
_CLAsincosTable_TABLE_SIZEDivTwoPi 	= 0xFe4e;
_CLAsincosTable_TwoPiDivTABLE_SIZE 	= 0xFe50;
_CLAsincosTable_TABLE_MASK 			= 0xFe52;
_CLAsincosTable_Coef1_pos 			= 0xFe58;
_CLAsincosTable_Coef3_neg			= 0xFe5e;
/*
//===========================================================================
// End of file.
//===========================================================================
*/
