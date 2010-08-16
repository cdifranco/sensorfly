/*============================================================================
  sf_user.c
*===========================================================================*/
#include "../common/sensorfly.h"
#include "../common/tn_irq.h"
#include "../SF_Firmware/SF_Sensors/sf_compass.h"


/*=========================================================================*/
/*  DEFINE: All Structures and Common Constants                            */
/*=========================================================================*/
#define TIMER0_ID 4
#define UART0_ID 6

typedef void (*IRQ_FUNC)(void);

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
void sf_timer0_int_handler (void)
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

  
  /* Set I/O port directions */
  // Set P0.0-0.31 as input, but later one of these pins will be set as output for enabling flow control with the radio
  rIO0DIR = 0x000000;	
  rIO0CLR = 0x000000;
  
  // Set P1.22 as output (UART)
  rIO1DIR = 0x200000;
  rIO1CLR = 0x200000;
  
  /* Initialize the UART */
  sf_uart0_init();

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
  * CPU clock of 60MHz and a APB bus clock of 30MHz.
  * The timer tick will be set to 0.1ms (30MHz / 10KHz = 3000)
  */
  rT0MR0 = (3000-1);
  
  /*
  * Interrupt on MR0, an interrupt is generated when MR0 matches 
  * the value in the TC.
  *
  * Reset on MR0: the TC will be reset if MR0 matches it.
  */
  rT0MCR = 0x0003;
  
  /* Enable the Timer0 */   
  rT0TCR = 1;
  
  
  /* Install UART0 with priority 1 */
  tn_irq_install(UART0_ID, 1, sf_uart0_int_handler);
  
  /* Install timer0 with priority 2 */
  tn_irq_install(TIMER0_ID, 2, sf_timer0_int_handler);
  
  
  /* Initialize LED */
  sf_led_init();

  //enable following line if sensorfly has a compass chip
  //setHeadingRefreshRateTo10Hz_itsTheMaximum(); //provided in sf_compass.c

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
   /* Enable UART0 interrupt */
   VICIntEnable = (1<<UART0_ID);

   /* Enable Timer0 interrupt */
   VICIntEnable = (1<<TIMER0_ID);
   
   tn_arm_enable_interrupts();
}


/***************************************************************************/
/*  tn_cpu_irq_handler                                                     */
/*                                                                         */
/*  This function will execute the user interrupt function.                */
/*                                                                         */                                                                       
/*  In    : none                                                           */
/*  Out   : none                                                           */
/*  Return: none                                                           */
/***************************************************************************/
void tn_cpu_irq_handler (void)
{
   IRQ_FUNC IrqFunc;
   
   IrqFunc = (IRQ_FUNC)VICVectAddr;   
   if (IrqFunc != (IRQ_FUNC)0)
   {
      IrqFunc();
   }

   /* Acknowledge Interrupt */
   VICVectAddr = 0;
}

/*** EOF ***/
