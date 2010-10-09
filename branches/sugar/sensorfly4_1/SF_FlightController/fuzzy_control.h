#ifndef FUZZY_CONTROL_H_
#define FUZZY_CONTROL_H_
#include "../common/sensorfly.h"

#define MF_TOTAL 3
#define MF_TYPE  3
#define MF_RIGHTVERTEX 2
#define MF_MIDDLEVERTEX 1
#define MF_LEFTVERTEX 0
#define MF_MAX_OUT 1000
#define MF_MIN_OUT 0
#define SPIN_CW 0
#define SPIN_NONE 1
#define SPIN_CCW 2
//#define SPIN_TARGET 437
#define SPIN_SATURATION_HIGH 30
#define SPIN_SATURATION_LOW  -30
#define HEIGHT_LOW 0
#define HEIGHT_RIGHT 1
#define HEIGHT_HIGH 2
#define HEIGHT_TARGET 80
#define HEIGHT_SATURATION_HIGH 30
#define HEIGHT_SATURATION_LOW -30
#define MOTOR_SLOW 0
#define MOTOR_NEUTRAL 1
#define MOTOR_FAST 2
#define MOTOR_CW 0
#define MOTOR_CCW 1

void set_spin_zero(uint32_t nospin);
void crispify(uint32_t height, uint32_t spin);
int32_t cw_load(void);
int32_t ccw_load(void);

#endif