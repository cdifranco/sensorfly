/*=========================================================================*/
/*  Includes                                                               */
/*=========================================================================*/
#include "../common/LPC214X.h"
#include "../common/sf_irq.h"
#include "../TNKernel/tn.h"

/*=========================================================================*/
/*  DEFINE: All Structures and Common Constants                            */
/*=========================================================================*/
typedef void (*IRQ_FUNC)(void);

/*=========================================================================*/
/*  DEFINE: Definition of all local Data                                   */
/*=========================================================================*/
static unsigned long *VectorAddressTable = (unsigned long*)(VIC_BASE + VICVectAddr0_OFFSET);
static unsigned long *VectorControlTable = (unsigned long*)(VIC_BASE + VICVectCntl0_OFFSET);

/*=========================================================================*/
/*  DEFINE: Definition of all local Procedures                             */
/*=========================================================================*/
/***************************************************************************/
/*  DefaultVICHandler                                                      */
/*                                                                         */
/*  This handler is set to deal with spurious interrupt.                   */
/*  When an IRQ service routine reads the Vector Address register          */
/*  (VICVectAddr), and no IRQ slot responds as described above,            */
/*  this function will be called.                                          */
/*                                                                         */
/*  In    : none                                                           */
/*  Out   : none                                                           */
/*  Return: none                                                           */
/***************************************************************************/
void DefaultVICHandler (void)
{
   /* 
    * If the IRQ is not installed into the VIC, and interrupt occurs, the
    * default interrupt VIC address will be used. This could happen in a 
    * race condition. For debugging, use this endless loop to trace back. 
    *
    * For more details, see Philips appnote AN10414 
    */
   VICVectAddr = 0;  /* Acknowledge Interrupt */ 
   
   while ( 1 )
   {
   }
}

/*=========================================================================*/
/*  DEFINE: All code exported                                              */
/*=========================================================================*/

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

/***************************************************************************/
/*  tn_irq_init                                                            */
/*                                                                         */
/*  Here the irq system of the cpu will be init.                           */
/*  E.g. a spurious interrupt will be set.                                 */
/*                                                                         */
/*  In    : none                                                           */
/*  Out   : none                                                           */
/*  Return: none                                                           */
/***************************************************************************/
void tn_irq_init (void)
{
   int i;
   
   /* Disable both IRQ & FIR */
   tn_arm_disable_interrupts();
   
   /* Disable all interrupts first */
   VICIntEnClr = 0xffffffff;
   
   /* Acknowledge Interrupt */
   VICVectAddr = 0;
   
   /* Select all interrupts for IRQ, instead FIQ */
   VICIntSelect = 0;
   
   /* Set VectorAddress and VectorControl to 0 */
   for(i=0; i<TN_MAX_IRQ_PRIO; i++)
   {
      VectorAddressTable[i] = 0;
      VectorControlTable[i] = 0;
   }

   /* Install the default VIC handler */
   VICDefVectAddr = (unsigned long)DefaultVICHandler;   
}

/***************************************************************************/
/*  tn_irq_install                                                         */
/*                                                                         */
/*  In    : id, prio, interrupt function                                   */
/*  Out   : none                                                           */
/*  Return: TERR_NO_ERR / TERR_WRONG_PARAM                                 */
/***************************************************************************/
int tn_irq_install (TN_IRQ_ID   id,
                    TN_IRQ_PRIO prio,
                    void (*irq_func)(void))
{
   int rc = TERR_NO_ERR;
   
   if( (id >= TN_MAX_IRQ_ID) || (prio >= TN_MAX_IRQ_PRIO) )
   {
      rc = TERR_WRONG_PARAM;
   }
   else
   {
      /* Disable interrupt first */
      VICIntEnClr = (1L << id);
   
      VectorAddressTable[prio] = (unsigned long)irq_func;
      VectorControlTable[prio] = (unsigned long)(0x20 | id);
   }
   
   return(rc);
} 

/*** EOF ***/
