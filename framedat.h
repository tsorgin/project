/*
   This file is part of the Mandelbox program developed for the course
    CS/SE  Distributed Computer Systems taught by N. Nedialkov in the
    Winter of 2015 at McMaster University.

    Copyright (C) 2015 T. Gwosdz and N. Nedialkov

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _FRAMEDAT_H
#define _FRAMEDAT_H

typedef struct
{
  double  camPos[3];
  double camTarget[3];
  double camUp[3];
  double fov;
  int width;
  int height;
 double detail;
  float scale;
  float rMin;
  float rFixed;
  int num_iter;
  float escape_time;
  int colourType;
  float brightness;
  int super_sampling;
  char file_name[80];

  /*
  All of the camera settings will be made zero in the getframedata
  function.
   */

} FrameData;

#endif
