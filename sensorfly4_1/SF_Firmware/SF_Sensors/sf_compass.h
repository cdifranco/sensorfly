#ifndef COMPASS_H_
#define COMPASS_H_

uint8_t GetCompassHeading(uint16_t *Heading, uint16_t *Pitch, uint16_t *Roll);
uint8_t setHeadingRefreshRateTo10Hz_itsTheMaximum();
uint8_t setIIRFilterTo14_itsTheMaximum();
uint8_t getAcceleration(int16_t *ax, int16_t *ay, int16_t *az);
#endif /*COMPASS_H_*/
