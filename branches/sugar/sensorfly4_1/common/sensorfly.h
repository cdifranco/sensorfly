#ifndef SF_SENSORFLY_H_
#define SF_SENSORFLY_H_
//#define  RTS_CTS_ENABLE 1
//-----------------------------------------------------------------------------
// Standard Headers
//-----------------------------------------------------------------------------
#include <stdint.h>
#include <string.h>

//-----------------------------------------------------------------------------
// Kernel and Platform headers
//-----------------------------------------------------------------------------
#include "lpc214x.h"
#include "sf_user.h"
#include "../TNKernel/tn.h"
#include "../TNKernel/tn_port.h"
#include "../SF_Firmware/SF_Sensors/sf_compass.h"
#include "../SF_Firmware/SF_UART/sf_uart.h"

//-----------------------------------------------------------------------------
// LED declarations
//-----------------------------------------------------------------------------
void sf_led_init();
void sf_led_on();
void sf_led_off();

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
char sf_fly_command;

//-----------------------------------------------------------------------------
// Interrupt Defines
//-----------------------------------------------------------------------------
#define TIMER0_ID 4
#define TIMER1_ID 5
#define UART0_ID 6
#define CTS_ID 17


#endif /* SF_SENSORFLY_H_ */