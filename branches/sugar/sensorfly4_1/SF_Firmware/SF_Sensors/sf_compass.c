/**
 * SensorFly Compass Driver
 * @author: Aveek Purohit
 * @date: 05/07/2009
 */
#include "../../common/sensorfly.h"
#include "../../common/lpc214x.h"
#include "sf_compass.h"

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
#define GET_ACCELERATIONS_CMD 0x40
#define ENTER_CALIBRATION_CMD 0x71
#define EXIT_CALIBRATION_CMD 0x7E
#define READ_EEPROM_CMD 0xE1
#define WRITE_EEPROM_CMD 0xF1


/**
 * Definitions
 */
void InitI2C(void) {
   rI2C0CONCLR = 0x6C;
   rPINSEL0  |= 0x50;   				// Set pinouts as scl and sda
   rI2C0SCLL = 75;        			// speed at 100Khz VPBDIV = 0 at SCLK 60 MHz
   rI2C0SCLH = 75;
   rI2C0CONSET = 0x40;    			// Master Transmitter Mode on I2C bus
}

uint8_t calibrationRoutine() {
	// First, enter calibration routine, rotate the helicopter about its Y axis but try to keep it steady
	
	InitI2C();

	if (SendI2CAddress((uint8_t)SLAV_ADD_W)) {
		StopI2C();
		return 1;
	}

	if (WriteI2C((uint8_t)ENTER_CALIBRATION_CMD)) {
		StopI2C();
		return 1;
	}

	StopI2C();

//  int16_t i; for(i=10000;i>0;i--); //wait
	
	// Now exit calibration routine
	
	InitI2C();

	if (SendI2CAddress((uint8_t)SLAV_ADD_W)) {
		StopI2C();
		return 1;
	}
	
	if (WriteI2C((uint8_t)EXIT_CALIBRATION_CMD)) {
			StopI2C();
			return 1;
	}
	
	StopI2C();
	return 0;
}

uint8_t eepromReadRoutine(char eepromAddress, char * eepromState) {	
	InitI2C();

	if (SendI2CAddress((uint8_t)SLAV_ADD_W)) {
		StopI2C();
		return 10;
	}

	if (WriteI2C((uint8_t)READ_EEPROM_CMD)) {
		StopI2C();
		return 20;
	}
	else {
		if (WriteI2C((uint8_t)eepromAddress)) {  // (send argument: eeprom address)
			StopI2C();
			return 30;
		}
	}

	StopI2C();

    int16_t i; for(i=10000;i>0;i--); //wait
	
	// now read status from eeprom
	if (SendI2CAddress((uint8_t)SLAV_ADD_R)) {
		StopI2C();
		return 40;
	}

	if (ReadI2C(eepromState, 1)) {
		StopI2C();
		return 50; // Read first byte
	}
	
	StopI2C();
	return 0;
}

uint8_t setHeadingRefreshRateTo10Hz_itsTheMaximum() {	
	InitI2C();

	if (SendI2CAddress((uint8_t)SLAV_ADD_W)) {
		StopI2C();
		return 10;
	}

	if (WriteI2C((uint8_t)WRITE_EEPROM_CMD)) {
		StopI2C();
		return 20;
	}
	else {
		if (WriteI2C((uint8_t)0x05)) {  // register 0x05 controls refresh rates.
			StopI2C();
			return 30;
		}
		else {
			if (WriteI2C((uint8_t)0x02)) {  // the value 0x02 means we are requesting 10 Hz update rate for compass values (it's the maximum).
				StopI2C();
				return 40;
			}
		}
	}

	StopI2C();
}

uint8_t setIIRFilterTo14_itsTheMaximum() {	
	InitI2C();

	if (SendI2CAddress((uint8_t)SLAV_ADD_W)) {
		StopI2C();
		return 10;
	}

	if (WriteI2C((uint8_t)WRITE_EEPROM_CMD)) {
		StopI2C();
		return 20;
	}
	else {
		if (WriteI2C((uint8_t)0x14)) {  // register 0x014 controls the IIR Filter
			StopI2C();
			return 30;
		}
		else {
			if (WriteI2C((uint8_t)0x0E)) {  // the value 0x0F means we are requesting each compass reading to be averaged based on the last 14 readings (it's the maximum).
				StopI2C();
				return 40;
			}
		}
	}

	StopI2C();
}

uint8_t SendI2CAddress(uint8_t Addr_S) {
    uint8_t r;

	rI2C0CONCLR = 0xFF;             	// clear I2C - included if User forgot to "StopI2C()"
	                             	// else this function would hang.
	rI2C0CONSET = 0x40;             	// Master Transmitter Mode on I2C bus
	if((Addr_S & 0x01))          	// test if it's reading
	   rI2C0CONSET = STA | AA;     	// set STA - allow master to acknowlege slave;
	else
	   rI2C0CONSET = STA;           	// set STA dont allow acknowledges;
	while(rI2C0STAT!=0x08) ;        	// Wait for start to be completed

	rI2C0DAT    = Addr_S;           	// Change slave Address
	rI2C0CONCLR = SIC | STAC;       	// Clear i2c interrupt bit to send the data
	while( ! ( rI2C0CONSET & SI)) ; 	// wait till status available
	r = rI2C0STAT;                   // read Status

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
	rI2C0CONCLR = SIC;                // clear SIC;
	while( ! (rI2C0CONSET & 0x8));    // wait till status available
	r=rI2C0STAT;                      // check for error

	if(r==0x50){ // data received ACK
		*Data = rI2C0DAT;
		Data++;
		if (--Bytes > 1)
		{
			rI2C0CONSET = 0x04;
		}
		else // next byte is last byte
		{
			rI2C0CONCLR = 0x04; // set AA condition
		}
		return 0;
	}

	if(r==0x58){
		*Data = rI2C0DAT; // last byte
		rI2C0CONSET = 0x14; // set STO and AA bits
		return 0;
	}
	return 1;
}

uint8_t WriteI2C(uint8_t Data) {
    uint8_t r;
    rI2C0DAT    = Data;              // Change Data
    rI2C0CONCLR = 0x8;               // SIC; Clear i2c interrupt bit to send the data
    while( ! (rI2C0CONSET & 0x8));   // wait till status available
    r=rI2C0STAT;

    if (r == 0x28){                 // look for "Data byte in S1DAT has been transmitted; ACK has been received"
       return 0;
    }
    return 1;
}

void StopI2C(void){
	while( ! ( rI2C0CONSET & SI));
    rI2C0CONCLR = SIC;
    rI2C0CONSET = STO;
}

uint8_t GetCompassHeading(uint16_t *Heading, uint16_t *Pitch, uint16_t *Roll){

	uint8_t s, datal, datah;
	int16_t i;

	InitI2C();

	if (SendI2CAddress((uint8_t)SLAV_ADD_W)) {
		StopI2C();
		return 10;
	}

	if (WriteI2C((uint8_t)A_COMMAND)) {
		StopI2C();
		return 20;
	}

	StopI2C();

	for(i=10000;i>0;i--); //wait

	if (SendI2CAddress((uint8_t)SLAV_ADD_R)) {
		StopI2C();
		return 30;
	}

	if (ReadI2C(&datah, 6)) {
		StopI2C();
		return 40; // Read first byte
	}

	if (ReadI2C(&datal, 5)) {
		StopI2C();
		return 50; // Read second byte
	}

	*Heading = (datah << 8) | datal;

	if (ReadI2C(&datah, 4)) {
		StopI2C();
		return 60; // Read first byte
	}

	if (ReadI2C(&datal, 3)) {
		StopI2C();
		return 70; // Read second byte
	}

	*Pitch = (datah << 8) | datal;

	if (ReadI2C(&datah, 2)) {
		StopI2C();
		return 80; // Read first byte
	}

	if (ReadI2C(&datal, 1)) {
		StopI2C();
		return 90; // Read second byte
	}

	*Roll = (datah << 8) | datal;

	StopI2C();
	return 0;
}

uint8_t getAcceleration(int16_t *ax, int16_t *ay, int16_t *az){

	uint8_t s, datal, datah;
	int16_t i;

	InitI2C();

	if (SendI2CAddress((uint8_t)SLAV_ADD_W)) {
		StopI2C();
		return 10;
	}

	if (WriteI2C((uint8_t)A_COMMAND)) {
		StopI2C();
		return 20;
	}

	StopI2C();

	for(i=10000;i>0;i--); //wait

	if (SendI2CAddress((uint8_t)SLAV_ADD_R)) {
		StopI2C();
		return 30;
	}

	if (ReadI2C(&datah, 6)) {
		StopI2C();
		return 40; // Read first byte
	}

	if (ReadI2C(&datal, 5)) {
		StopI2C();
		return 50; // Read second byte
	}

	*ax = (datah << 8) | datal;

	if (ReadI2C(&datah, 4)) {
		StopI2C();
		return 60; // Read first byte
	}

	if (ReadI2C(&datal, 3)) {
		StopI2C();
		return 70; // Read second byte
	}

	*ay = (datah << 8) | datal;

	if (ReadI2C(&datah, 2)) {
		StopI2C();
		return 80; // Read first byte
	}

	if (ReadI2C(&datal, 1)) {
		StopI2C();
		return 90; // Read second byte
	}

	*az = (datah << 8) | datal;

	StopI2C();
	return 0;
}

