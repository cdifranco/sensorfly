/*============================================================================
  tn_user.c
*===========================================================================*/
#include "../common/LPC214X.h"
#include "../common/sf_irq.h"
#include "../TNKernel/tn.h"

/*=========================================================================*/
/*  DEFINE: All Structures and Common Constants                            */
/*=========================================================================*/
#define TIMER0_ID 4


extern void tn_arm_enable_interrupts(void);

/*=========================================================================*/
/*  DEFINE: Definition of all local Procedures                             */
/*=========================================================================*/

/***************************************************************************/
/*  Timer0IRQHandler                                                       */
/*                                                                         */
/*  For the purpose of calculating timeouts and delays, TNKernel uses a    */
/*  time tick timer. In TNKernel, this time tick timer must to be a some   */
/*  kind of hardware timer that produces interrupt for time ticks          */
/*  processing. The period of this timer is user determined (usually in    */
/*  the range 0.5...20 ms). Within the time ticks interrupt processing,    */
/*  it is only necessary to call the tn_tick_int_processing() functions    */
/*                                                                         */
/*  In    : none                                                           */
/*  Out   : none                                                           */
/*  Return: none                                                           */
/***************************************************************************/
void Timer0IRQHandler (void)
{
   /* Clear timer interrupt */
   rT0IR = 1;
   
   tn_tick_int_processing();
}

/*=========================================================================*/
/*  DEFINE: All code exported                                              */
/*=========================================================================*/
/***************************************************************************/
/*  HardwareInit                                                           */
/*                                                                         */
/*  Init the hardware like the timer and the GPIO port for the LED.        */
/*                                                                         */
/*  In    : none                                                           */
/*  Out   : none                                                           */
/*  Return: none                                                           */
/***************************************************************************/
void hardware_init (void)
{
   /*
    * Init the interrupt system first
    */            
   tn_irq_init();

   /*
    * The CPU clock is set to 60MHz and the APB bus to 30MHz
    * by the startup code from CrossWorks for ARM. Here we must 
    * only init the timer. Setup the Timer0 with a period of 1ms.
    */

   /* Disable Timer0 */
   rT0TCR = 0;
   
   /* 
    * Set the prescale counter.
    * This causes the TC to increment on every PCLK when PR = 0
    */
   rT0PR = 0;

   /*
    * Assume a CPU clock of 60MHz and a APB bus clock of 30MHz.
    * The timer tick will be set to 1ms (30MHz / 1KHz = 30000)
    */
   rT0MR0 = (30000-1);
   
   /*
    * Interrupt on MR0, an interrupt is generated when MR0 matches 
    * the value in the TC.
    *
    * Reset on MR0: the TC will be reset if MR0 matches it.
    */
   rT0MCR = 0x0003;

   /* Enable the Timer0 */   
   rT0TCR = 1;

   /* Install timer0 with prio 1 */
   tn_irq_install(TIMER0_ID, 1, Timer0IRQHandler);

}


/***************************************************************************/
/*  tn_cpu_int_enable                                                      */
/*                                                                         */
/*  The tn_cpu_int_enable() function enables all interrupts for vectors    */
/*  utilized by the user project and than enables global interrupts.       */
/*  The user must enable the system time ticks interrupt in this function. */
/*  TNKernel calls this function without user intervention.                */
/*                                                                         */
/*  In    : none                                                           */
/*  Out   : none                                                           */
/*  Return: none                                                           */
/***************************************************************************/
void tn_cpu_int_enable (void)
{
   /* Enable Timer0 interrupt */
   VICIntEnable = (1<<TIMER0_ID);
   
   tn_arm_enable_interrupts();
}

/*** EOF ***/
