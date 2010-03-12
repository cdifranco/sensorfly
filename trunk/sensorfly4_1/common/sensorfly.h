#ifndef SF_SENSORFLY_H_
#define SF_SENSORFLY_H_

//-----------------------------------------------------------------------------
// Standard Headers
//-----------------------------------------------------------------------------
#include <stdint.h>
#include <string.h>

//-----------------------------------------------------------------------------
// Kernel and Platform headers
//-----------------------------------------------------------------------------
#include "LPC214x.h"
#include "sf_user.h"
#include "../TNKernel/tn.h"
#include "../TNKernel/tn_port.h"

#include "../SF_Firmware/SF_UART/sf_uart.h"

//-----------------------------------------------------------------------------
// LED declarations
//-----------------------------------------------------------------------------
void sf_led_init();
void sf_led_on();
void sf_led_off();

#endif /* SF_SENSORFLY_H_ */