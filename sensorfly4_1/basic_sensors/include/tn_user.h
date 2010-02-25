#ifndef TN_USER_H_
#define TN_USER_H_

/***************************************************************************/
/*  DEFINE: All Structures and Commn Constants                             */
/***************************************************************************/

/*
 * Define the irq id and prio type, this makes it easier
 * if it must changed e.g. from unsigned char to unsigned short.
 */
typedef unsigned char   TN_IRQ_ID;
typedef unsigned char   TN_IRQ_PRIO;

/*
 * The lpc2000 suports 32 irq's with 16 priority levels.
 */
#define TN_MAX_IRQ_ID   32
#define TN_MAX_IRQ_PRIO 16

/***************************************************************************/
/*                 P R O T O T Y P E S                                     */
/***************************************************************************/
void tn_irq_init (void);
int tn_irq_install (TN_IRQ_ID   id,
                    TN_IRQ_PRIO prio,
                    void (*irq_func)(void));

#endif