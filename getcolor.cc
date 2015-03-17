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
#include "color.h"
#include "renderer.h"
#include "vector3d.h"
#include <cmath>
#include <algorithm>

using namespace std;

//---lightning and colouring---------
static vec3 CamLight(.15,.15,.15);
static double CamLightW = 1.8;// 1.27536;
static double CamLightMin = 0.3;// 0.48193;
//-----------------------------------
static const vec3 Shade_origin(0.0, 0.0, 0.0);

static const vec3 baseColor(1, .91, 1.0);
static const vec3 backColor(.1,.3,.20);
//-----------------------------------

void lighting(const vec3 &n, const vec3 &color, const vec3 &pos, const vec3 &direction,  vec3 &outV)
{
  vec3 nn = n -1.0;
  double ambient = max( CamLightMin, nn.Dot(direction) )*CamLightW;
  outV = CamLight*ambient*color;
}

vec3 getColour(const pixelData &pixData, const RenderParams &render_params,
	       const vec3 &from, const vec3  &direction)
{
  //colouring and lightning
  vec3 hitColor = baseColor;

  if (pixData.escaped == false)
    {
      //apply lighting
      //printf("Distance: %f\n", pixData.distance);
      lighting(pixData.normal, hitColor, pixData.hit, direction, hitColor);
      //printf("Before\t\t%f %f %f\n", hitColor.x,hitColor.y,hitColor.z);
      //add normal based colouring

      //printf("%f\n",pixData.distance );

      if(render_params.colourType == 0 || render_params.colourType == 1)
	{
	  hitColor = hitColor * pixData.normal;
	  hitColor = (hitColor + 1.0)/2.0;
	  hitColor = hitColor*render_params.brightness;

	  // //gamma correction
	  clamp(hitColor, 0.0, 1.0);
	  hitColor = hitColor*hitColor;
   //  printf("Corrected\t%f %f %f\n", hitColor.x,hitColor.y,hitColor.z);

	}
      if(render_params.colourType == 1)
	{
	  //"swap" colors
	  double t = hitColor.x;
	  hitColor.x = hitColor.z;
	  hitColor.z = t;
	}

  //Fogging
  double total = hitColor.x - backColor.x + hitColor.y - backColor.y +hitColor.z- backColor.z;
  double xscale = (hitColor.x - backColor.x)/total;
  double yscale = (hitColor.y - backColor.y)/total;
  double zscale = (hitColor.z - backColor.z)/total;


  // hitColor.x = hitColor.x/1.1 - (backColor.x*pixData.distance*4000*xscale)/3.9 ;
  // hitColor.y = hitColor.y/1.1 - (backColor.y*pixData.distance*4000*yscale)/3.9 ;
  // hitColor.z = hitColor.z/1.1 - (backColor.z*pixData.distance*4000*zscale)/3.9;
  // clamp(hitColor, 0.0,1.0); //Not what I want to clamp. Should clamp each component

    }
  else{
      //Draw a circular shadow in the background.
      vec3 temp, temp2, bottom;
      temp = Shade_origin - direction;
      temp2 = Shade_origin - from;

      temp.Cross(temp2);
      double top = temp.Magnitude();
      bottom = from-direction;
      double party = bottom.Magnitude();
      double answer = top/party;
      answer = answer;

      hitColor = backColor;
      hitColor.z = hitColor.z*answer;
      hitColor.x = hitColor.x*answer;
      hitColor.y = hitColor.y*answer;
    }
  return hitColor;
}
