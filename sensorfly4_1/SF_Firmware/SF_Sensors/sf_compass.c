/**
 * SensorFly Compass Driver
 * @author: Aveek Purohit
 * @date: 05/07/2009
 */
#include "sensorfly.h"
#include "compass.h"

/**
 * Declarations
 */

void InitI2C(void);
uint8_t SendI2CAddress(uint8_t Addr_S);
uint8_t ReadI2C(uint8_t *Data, uint8_t Bytes);
uint8_t WriteI2C(uint8_t Data);
void StopI2C(void);

#define STA  0x20
#define SIC  0x08
#define SI   0x08
#define STO  0x10
#define STAC 0x20
#define AA   0x04

#define SLAV_ADD_W 0x32
#define SLAV_ADD_R 0x33
#define A_COMMAND  0x50


/**
 * Definitions
 */
void InitI2C(void) {

   I2C0CONCLR = 0x6C;
   PINSEL0  |= 0x50;   				// Set pinouts as scl and sda
   I2C0SCLL = 75;        			// speed at 100Khz VPBDIV = 0 at SCLK 60 MHz
   I2C0SCLH = 75;
   I2C0CONSET = 0x40;    			// Master Transmitter Mode on I2C bus
}

uint8_t SendI2CAddress(uint8_t Addr_S) {
    uint8_t r;

	I2C0CONCLR = 0xFF;             	// clear I2C - included if User forgot to "StopI2C()"
	                             	// else this function would hang.
	I2C0CONSET = 0x40;             	// Master Transmitter Mode on I2C bus
	if((Addr_S & 0x01))          	// test if it's reading
	   I2C0CONSET = STA | AA;     	// set STA - allow master to acknowlege slave;
	else
	   I2C0CONSET = STA;           	// set STA dont allow acknowledges;
	while(I2C0STAT!=0x08) ;        	// Wait for start to be completed

	I2C0DAT    = Addr_S;           	// Change slave Address
	I2C0CONCLR = SIC | STAC;       	// Clear i2c interrupt bit to send the data
	while( ! ( I2C0CONSET & SI)) ; 	// wait till status available
	r = I2C0STAT;                   // read Status

	if(!(Addr_S & 0x01)) {       	// if we are doing a write
	    if (r == 0x18) {            // look for "SLA+W has been transmitted; ACK has been received"
	         return 0;
	    }
	 } else {
	    if (r == 0x40) {            // look for "SLA+R has been transmitted; ACK has been received"
	         return 0;
	    }
	 }
	return 1;
}

uint8_t ReadI2C(uint8_t *Data, uint8_t Bytes) {
	uint8_t r;
	I2C0CONCLR = SIC;                // clear SIC;
	while( ! (I2C0CONSET & 0x8));    // wait till status available
	r=I2C0STAT;                      // check for error

	if(r==0x50){ // data received ACK
		*Data = I2C0DAT;
		Data++;
		if (--Bytes > 1)
		{
			I2C0CONSET = 0x04;
		}
		else // next byte is last byte
		{
			I2C0CONCLR = 0x04; // set AA condition
		}
		return 0;
	}

	if(r==0x58){
		*Data = I2C0DAT; // last byte
		I2C0CONSET = 0x14; // set STO and AA bits
		return 0;
	}
	return 1;
}

uint8_t WriteI2C(uint8_t Data) {
    uint8_t r;
    I2C0DAT    = Data;              // Change Data
    I2C0CONCLR = 0x8;               // SIC; Clear i2c interrupt bit to send the data
    while( ! (I2C0CONSET & 0x8));   // wait till status available
    r=I2C0STAT;

    if (r == 0x28){                 // look for "Data byte in S1DAT has been transmitted; ACK has been received"
       return 0;
    }
    return 1;
}

void StopI2C(void){
	while( ! ( I2C0CONSET & SI));
    I2C0CONCLR = SIC;
    I2C0CONSET = STO;
}

uint8_t GetCompassHeading(uint16_t *Heading, uint16_t *Pitch, uint16_t *Roll){

	uint8_t s, datal, datah;
	int16_t i;

	InitI2C();

	if (SendI2CAddress((uint8_t)SLAV_ADD_W)) {
		StopI2C();
		return 1;
	}

	if (WriteI2C((uint8_t)A_COMMAND)) {
		StopI2C();
		return 1;
	}

	StopI2C();

	for(i=10000;i>0;i--); //wait

	if (SendI2CAddress((uint8_t)SLAV_ADD_R)) {
		StopI2C();
		return 1;
	}

	if (ReadI2C(&datah, 6)) {
		StopI2C();
		return 1; // Read first byte
	}

	if (ReadI2C(&datal, 5)) {
		StopI2C();
		return 1; // Read second byte
	}

	*Heading = (datah << 8) | datal;

	if (ReadI2C(&datah, 4)) {
		StopI2C();
		return 1; // Read first byte
	}

	if (ReadI2C(&datal, 3)) {
		StopI2C();
		return 1; // Read second byte
	}

	*Pitch = (datah << 8) | datal;

	if (ReadI2C(&datah, 2)) {
		StopI2C();
		return 1; // Read first byte
	}

	if (ReadI2C(&datal, 1)) {
		StopI2C();
		return 1; // Read second byte
	}

	*Roll = (datah << 8) | datal;

	StopI2C();
	return 0;
}


