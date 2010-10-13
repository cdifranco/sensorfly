#include "fuzzy_control.h"

/** Membership Functions*/

/**MF Declarations*/
/**There are two outputs to this fuzzy system:
   Clockwise(cw) and Counter-clockwise (ccw) motors.
   For the defuzzification, we are using TSK (Takagi-
   Sugeno, of order 1, in the form: ax+b
   where x is the input error.
   Right now each motor has three modes: slow, neutral, 
   and fast, determined the FIS rules.
   The arrays below determines the values to the 
   coefficients a and b to the equation above for the
   cw and ccw motors (e.g. cwA array contains the 
   'a' coefficients for the cw motor indexed by the 
   motor mode*/
int32_t cwA[] = {0, 0, 0}; //{slow, neutral, fast}
int32_t cwB[] = {-7, 1, 7};
int32_t ccwA[] = {0, 0, 0};
int32_t ccwB[] = {-9, 1, 9};

/** Below are listed the membership functions (mfs)
    for spin and height.
    There are currently only three membership functions
    for each linguistic variable and they are all triangular.
    Each mf is represented as an array with three numbers
    each number representing the x-coordinate of the vertices 
    of the triangle: array element 0 being the left-most
    vertex, element 1 is the middle, and the 2 element is 
    the right-most vertex. These indices are defined as 
    in the fuzzy_control.h header file.
    Also the right and left vertices are at the zero 
    y-coordinate and the middle is at the maximum y-coordinate
    (defined in fuzzy_control.h*/
int32_t mf_spin_cw[MF_TYPE] = {SPIN_SATURATION_LOW, SPIN_SATURATION_LOW, 0};
int32_t mf_spin_static[MF_TYPE] = {-4, 0, 4};
int32_t mf_spin_ccw[MF_TYPE] = {0, SPIN_SATURATION_HIGH,  SPIN_SATURATION_HIGH};

int32_t mf_height_low[MF_TYPE] = { HEIGHT_SATURATION_LOW, HEIGHT_SATURATION_LOW, 0};
int32_t mf_height_right[MF_TYPE] = {-3, 0, 3};
int32_t mf_height_high[MF_TYPE] = {0, HEIGHT_SATURATION_HIGH,HEIGHT_SATURATION_HIGH};

/**These are arrays of arrays that contain the mfs
   listed above for easy access of all in a loop*/
int32_t *mfs_spin[]   = {mf_spin_cw, mf_spin_static, mf_spin_ccw};
int32_t *mfs_height[] = {mf_height_low, mf_height_right, mf_height_high};

/** Function to retrieve the value of a membership function
    (its weight) given the x (error) value.
*/
int32_t mf_weight(int32_t x, int32_t mf[]){
  int32_t rightVertex, middleVertex, leftVertex, min = MF_MIN_OUT, weight = 0;
  
  rightVertex  = mf[MF_RIGHTVERTEX];
  middleVertex = mf[MF_MIDDLEVERTEX];
  leftVertex   = mf[MF_LEFTVERTEX];

  //if mf is 1st or 3rd of height mfs
  //and the error is past the saturation point
  //then return the max weight.
  if(mf == mf_height_low || mf == mf_height_high){
    if(x >= HEIGHT_SATURATION_HIGH || x <= HEIGHT_SATURATION_LOW)
      return weight = MF_MAX_OUT;
  }
  //if mf is 1st or 3rd of spin mfs
  //and the error is past the saturation point
  //then return the max weight.
  if(mf == mf_spin_cw || mf == mf_spin_ccw){
    if(x >= SPIN_SATURATION_HIGH || x <= SPIN_SATURATION_LOW)
      return weight = MF_MAX_OUT;
  }

  //if error is not element of membership set
  //return weight = 0;
  if(x > rightVertex || x < leftVertex)
    return weight = 0;

 //if x is to the left of middle vertex subtract left vertex
 //from middle, otherwise, subtract right (simple algebra
 //to find the weight given x
 //w - w0 = ((w1-w0)/(x1-x0))*(x - x0)
  if(x < middleVertex)
    weight = ((MF_MAX_OUT - MF_MIN_OUT)/(middleVertex - leftVertex))*(x - leftVertex);
  else
    weight = ((MF_MAX_OUT - MF_MIN_OUT)/(middleVertex - rightVertex))*(x - rightVertex);

  return weight;
}

/**Functions to find the cw and ccw motor output
   given the error and the motor mode(slow, neutral,fast).
   Used for the defuzzification, after each rule evaluation*/
int32_t cwY(int32_t x, uint32_t mode){
  return (cwA[mode]*x + cwB[mode]);  
}

int32_t ccwY(int32_t x, uint32_t mode){
  return (ccwA[mode]*x + ccwB[mode]);   
}

/*RULES**/
uint32_t spin_rule_output[2] = {0,0};

//The output rules of the spin determined by the function below
//rules are saved in the array above
//for easy access by other local functions (see crispify)
//(Sorry for the bad programming style, but time pressure
//was very tight)
void spin_rules(uint32_t lingVar){
 
   switch(lingVar){
    case SPIN_CW: //spinning cw
      spin_rule_output[MOTOR_CW] = MOTOR_NEUTRAL; // cwMotor neutral
      spin_rule_output[MOTOR_CCW] = MOTOR_FAST; // ccwMotor fast
      break;
    case SPIN_NONE: //no spin
      spin_rule_output[MOTOR_CW] = MOTOR_NEUTRAL; //neutral cwMotor
      spin_rule_output[MOTOR_CCW] = MOTOR_NEUTRAL; // neutral ccwMotor
      break;
    case SPIN_CCW: //spinning ccw
      spin_rule_output[MOTOR_CW] = MOTOR_FAST;
      spin_rule_output[MOTOR_CCW] = MOTOR_NEUTRAL;
      break;
    default:
      spin_rule_output[MOTOR_CW] = MOTOR_NEUTRAL;
      spin_rule_output[MOTOR_CCW] = MOTOR_NEUTRAL;
  }
}

uint32_t height_rule_output[2] = {0, 0};
//The output rules of the height determined by the function below
//rules are saved in the array above
//for easy access by other local functions (see crispify)

void height_rules(uint32_t lingVar){
 
  switch(lingVar){
    case HEIGHT_LOW: //flying low
      height_rule_output[MOTOR_CW] = MOTOR_FAST; // cwMotor neutral
      height_rule_output[MOTOR_CCW] = MOTOR_FAST; // ccwMotor fast
      break;
    case HEIGHT_RIGHT: //flying right
      height_rule_output[MOTOR_CW] = MOTOR_NEUTRAL; //neutral cwMotor
      height_rule_output[MOTOR_CCW] = MOTOR_NEUTRAL; // neutral ccwMotor
      break;
    case HEIGHT_HIGH: //flying high
      height_rule_output[MOTOR_CW] = MOTOR_SLOW;
      height_rule_output[MOTOR_CCW] = MOTOR_SLOW;
      break;
    default:
      height_rule_output[MOTOR_CW] = MOTOR_NEUTRAL;
      height_rule_output[MOTOR_CCW] = MOTOR_NEUTRAL;
  }
}


/**Defuzzification*/

//The final motor loads are put in the array below by
//function 'crispify' and are retrieved in sf_flightcontroller.c
//by the functions 'cw_load' and 'ccw_load' for feeding 
//the motors

int32_t load[2] = {0, 0};

int32_t cw_load(void){
  return load[MOTOR_CW];
}
int32_t ccw_load(void){
  return load[MOTOR_CCW];
}
//The variable below is used before take off
//to determine what number is recognized by the gyro
//as 'no spin' and is set in sf_flightcontroller.c
//using the function set_spin_zero below.

uint32_t spin_zero = 0;

void set_spin_zero(uint32_t nospin) {
  spin_zero = nospin;
}

/** The function below puts everything together.
Its goal is to set the 'load' array above so that
sf_flightcontroller can feed the motors. 
*/
void crispify(uint32_t height, uint32_t spin){
  uint32_t i = 0, weightsum = 0, cw_motor_sum = 0, ccw_motor_sum = 0;
  int32_t heightErr = height - HEIGHT_TARGET, spinErr = spin_zero - spin, currentWeight = 0;

 for(i = 0; i < MF_TOTAL; i++){
    /** Height is currently being controlled 
    by a PID controller in sf_flightcontroller
    and hence its contribution here is commented out*/
    /*height_rules(i);
    currentWeight = mf_weight(heightErr, mfs_height[i]);
    cw_motor_sum += currentWeight*cwY(heightErr,height_rule_output[MOTOR_CW]);
    ccw_motor_sum += currentWeight*cwY(heightErr,height_rule_output[MOTOR_CCW]);
    weightsum += currentWeight;*/

    //load the current spin rules into spin_rule_output
    spin_rules(i);

    //calculate weight based on current spin rule
    //and error
    currentWeight = mf_weight(spinErr, mfs_spin[i]);
    //TSK applied: please refer to Takagi-Sugeno defuzzification
    cw_motor_sum += currentWeight*cwY(spinErr,spin_rule_output[MOTOR_CW]);
    ccw_motor_sum += currentWeight*cwY(spinErr,spin_rule_output[MOTOR_CCW]);
    weightsum += currentWeight;
 }

 //Normalizing the sums
 cw_motor_sum /= weightsum;
 ccw_motor_sum /= weightsum;
 

 load[MOTOR_CW] = cw_motor_sum; 
 load[MOTOR_CCW] = ccw_motor_sum;
}