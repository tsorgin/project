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

#ifndef _RENMandelBoxDERER_H
#define _RENMandelBoxDERER_H

typedef struct
{
  int fractalType;
  int colourType;
  int super_sampling;
  float brightness;
  int width;
  int height;
  float detail;
  int maxRaySteps;
  float maxDistance;
  char file_name[80];

  /*
  If we get more complicated we will have to start doing checks on which params are on and off
  based on other param. IE dependencies.
   */

  /*
  Params for CAmera pth planning
   */
  int enable; //Do we want Camera path planning enabled
  int distShower; //Shows a circle on the screen of the furthest point (in the viewing sqaure)
  int holeSize; //How big of a test circle to draw(in pixles)
  int camBoundBox; //Do we want to see the window where the camera is searching for new max depth
  int boundBoxx; //Bounding box x size
  int boundBoxy; //Bounding box y size
  int camTestPoints; //Should we show were the camera is testing for max depth (circles)
} RenderParams;

#endif
