/******************************************************************************
*  SensorFly, a controlled-mobile sensor networking platform.
*  Copyright (C) 2010, Carnegie Mellon University
*  All rights reserved.
*
*  This program is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, version 2.0 of the License.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/
#ifndef SF_MAPPING_H_
#define SF_MAPPING_H_

void find_theta(uint32_t lineOS, uint32_t lineDS, uint32_t lineOD, float *angleOS);
uint32_t removeHeightComponent(uint32_t line, uint32_t height);
void find_coods(uint32_t line, float *angle, int32_t *xCood, int32_t *yCood);

#endif /* SF_MAPPING_H_ */