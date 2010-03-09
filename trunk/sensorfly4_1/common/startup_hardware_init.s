/*
    TNKernel startup hardware init for LPC21XX(LPC22XX) processor
    (for Rowley Associates Ltd.(R) CrossStudio(C) IDE
     and any other IDE uses GCC compiler)

    GCC ARM assembler

Copyright © 2004,2005 Yuri Tiomkin
All rights reserved.

Permission to use, copy, modify, and distribute this software in source
and binary forms and its documentation for any purpose and without fee
is hereby granted, provided that the above copyright notice appear
in all copies and that both that copyright notice and this permission
notice appear in supporting documentation.

THIS SOFTWARE IS PROVIDED BY THE YURI TIOMKIN AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL YURI TIOMKIN OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.
*/

//-----------------------------------------------------------------------------
//  Clock Settings
//-----------------------------------------------------------------------------

/* Fosc = 12Mhz, P = 2, M = 5 : Fcco = 240Mhz, cclk = 60Mhz */
#define PLLCFG_VAL 0x24

/* 
 *   Override the default VPB divider value (VPB bus clock is one half of the
 *   processor clock) by defining VPBDIV_VAL. Default pclk = 30MHz
 */
#ifndef VPBDIV_VAL
#define VPBDIV_VAL 2
#endif

#define SCB_BASE         0xE01FC000
#define SCB_PLLCON_OFFS  0x080
#define SCB_PLLCFG_OFFS  0x084
#define SCB_PLLSTAT_OFFS 0x088
#define SCB_PLLFEED_OFFS 0x08C
#define SCB_VPBDIV_OFFS  0x100

//-----------------------------------------------------------------------------

  .equ    MC_FMR,    0xFFFFFF60
  .equ    WDT_MR,    0xFFFFFD44
  .equ    CKGR_MOR,  0xFFFFFC20
  .equ    PMC_SR,    0xFFFFFC68
  .equ    CKGR_PLLR, 0xFFFFFC2C
  .equ    PMC_MCKR,  0xFFFFFC30
  .equ    MC_RCR,    0xFFFFFF00

  .equ    NOINT,     0xc0 


  .text
  .code 32
  .align 0

  .global tn_startup_hardware_init
  .global tn_arm_disable_interrupts
  .global tn_arm_enable_interrupts

rMAMTIM:         .word  0xE01FC004
rMAMCR:          .word  0xE01FC000

//----------------------------------------------------------------------------
//  This routine is called immediately after reset to setup hardware that is
// vital for processor's functionality (for instance,SDRAM controller setup,
// PLL setup,etc.)
//  It is assumed that other hardware's init routine(s) will be invoked later
// by C-language function call.
//----------------------------------------------------------------------------

//-- For LPC2101/03, LPC2141/48

tn_startup_hardware_init:
      ldr r0, =SCB_BASE
#if defined(PLLCFG_VAL) && !defined(NO_PLL_ENABLE)
      /* Configure PLL Multiplier/Divider */
      ldr r1, =PLLCFG_VAL
      str r1, [r0, #SCB_PLLCFG_OFFS]
      /* Enable PLL */
      mov r1, #0x1
      str r1, [r0, #SCB_PLLCON_OFFS]
      mov r1, #0xAA
      str r1, [r0, #SCB_PLLFEED_OFFS]
      mov r1, #0x55
      str r1, [r0, #SCB_PLLFEED_OFFS]
      /* Wait for PLL to lock */
    pll_lock_loop:
      ldr r1, [r0, #SCB_PLLSTAT_OFFS]
      tst r1, #0x400
      beq pll_lock_loop
      /* PLL Locked, connect PLL as clock source */
      mov r1, #0x3
      str r1, [r0, #SCB_PLLCON_OFFS]
      mov r1, #0xAA
      str r1, [r0, #SCB_PLLFEED_OFFS]
      mov r1, #0x55
      str r1, [r0, #SCB_PLLFEED_OFFS]
#endif

     /* Flash speed */
     /* rMAMTIM = 3 */
     ldr   r0, rMAMTIM
     mov   r1, #3
     strb  r1, [r0]

     /* rMAMCR  = 2 */
     ldr   r0, rMAMCR
     mov   r1, #2
     strb  r1, [r0]

     /* Configure the VPB clock */
     ldr r1, =VPBDIV_VAL
     str r1, [r0, #SCB_VPBDIV_OFFS]

     bx   lr

//----------------------------------------------------------------------------

tn_arm_disable_interrupts:

     mrs  r0, cpsr
     orr  r0, r0, #NOINT
     msr  cpsr_c, r0
     bx   lr


//----------------------------------------------------------------------------

tn_arm_enable_interrupts:

     mrs  r0, cpsr
     bic  r0, r0, #NOINT
     msr  cpsr_c, r0
     bx   lr


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------






