#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../common/sensorfly.h"
#include "sf_mapping.h"


/*! \fn void find_theta(uint32_t lineOS, uint32_t lineDS, uint32_t lineOD, float *angleOS)
    \brief Calculates the angle OSD
    \param lineOS the distance of the line OS, lineDS the distance of the line DS, lineDS the distance of the line OD, *angleOS a pointer to a float where the angle value is to be stored
    \return none
*/
void find_theta(uint32_t lineOS, uint32_t lineDS, uint32_t lineOD, float *angleOS) {
      //length of 3 lines between 3 points of a triangle
      //S-Point on which the angle falls, O, D are the two other corners of the triangle
      *angleOS = acos((pow(lineOS,2) + pow(lineDS,2) - pow(lineOD,2))/(2 * lineOS * lineDS));
    //  return angleOS;
}

/*! \fn uint32_t removeHeightComponent(uint32_t line, uint32_t height)
    \brief Remove the height vector from the distance returned from the radio
    \param line the distance the radio returns, height the distance the sonic sensor returns
    \return Distance from node to reference point along the ground
*/
uint32_t removeHeightComponent(uint32_t line, uint32_t height){
  return sqrt(pow(line, 2) - pow(height, 2));
}

/*! \fn find_coods(uint32_t line, float *angle, int32_t *xCood, int32_t *yCood)
    \brief Find the x and y coordinates of a point, based on the angle and a distance
    \param line the distance between the point and the origin, *angle pointer to the angle formed by the axis and the line from the origin to the point, *xcood a pointer to where the x coordinate should be stored, *ycood a pointer to where the y coordinate should be stored
    \return none
*/
void find_coods(uint32_t line, float *angle, int32_t *xCood, int32_t *yCood){
    
    
    float theta=0.0;
    static float pi=3.1415926535897932384;
    theta=*angle;
    short  neg=0; 
    if(theta==0){ //if theta is 0 line falls on y-axis
        *xCood=0;
        *yCood=line;
     }else if(theta==pi/2){//if theta is 90 degrees (pi/2) line falls on x axis
        *yCood=0;
        *xCood=line;
     }else{
       if(theta>pi/2){//if theta if greater then 90 degrees (pi/2) use reflection over x axis
          neg=1;
          theta=theta-pi/2;
       }
       *xCood=cos(theta)*line;
       if(neg==1)
         *yCood=-sin(theta)*line;
       else
         *yCood=sin(theta)*line;
    }
}
