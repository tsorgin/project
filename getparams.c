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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "renderer.h"
#include "mandelbox.h"
#include "camera.h"

#define BUF_SIZE 1024

static char buf[BUF_SIZE];

void getParameters(char *filename, CameraParams *camP, RenderParams *renP, MandelBoxParams *boxP)
{
  printf("getting parameters...\n");
  FILE *fp;
  int ret;
  double *d;

  renP->fractalType = 0;
  renP->maxRaySteps = 2000;
  renP->maxDistance = 100;

  fp = fopen(filename,"r");

  if( !fp )
    {
      printf(" *** File %s does not exist\n", filename);
      exit(1);
    }

  int count = 0;

  while (1)
    {
      memset(buf, 0, BUF_SIZE);

      ret = fscanf(fp, "%1023[^\n]\n", buf);
      if (ret == EOF) break;

      if(buf[0] == '#') // comment line
	continue;

      switch(count)
	{
	  // CAMERA
	  //camera position
	case 0:
	  d = camP->camPos;
	  sscanf(buf, "%lf %lf %lf", d, d+1, d+2);
	  break;
	case 1:
	  //camera target
	  d = camP->camTarget;
	  sscanf(buf, "%lf %lf %lf", d, d+1, d+2);
	  break;
	  //camera up
	case 2:
	  d = camP->camUp;
	  sscanf(buf, "%lf %lf %lf", d, d+1, d+2);
	  break;
	  //field of view
	case 3:
	  sscanf(buf, "%lf", &camP->fov);
	  break;

	  //IMAGE
	  //width, height
	case 4:
	  sscanf(buf, "%d %d", &renP->width, &renP->height);
	  break;
	  //detail
	case 5:
	  sscanf(buf, "%f", &renP->detail);
	  break;

	  //FRACTAL
	case 6:
	  sscanf(buf, "%f %f %f", &boxP->scale, &boxP->rMin, &boxP->rFixed);
 	  break;

	case 7:
	  sscanf(buf, "%d %f ", &boxP->num_iter, &boxP->escape_time);
	  break;

	  //COLORING
	case 8:
	  sscanf(buf, "%d", &renP->colourType);
	  break;
	case 9:
	  sscanf(buf, "%f ", &renP->brightness);
	  break;
	case 10:
	  sscanf(buf, "%d ", &renP->super_sampling);
	  break;

	  //FILENAME
	case 11:
	  strcpy(renP->file_name, buf);
	  break;
	case 12:
		sscanf(buf, "%d", &renP->enable);
	case 13:
		sscanf(buf, "%d %d %d", &renP->camBoundBox,&renP->boundBoxx,&renP->boundBoxy);
		/*
		Need to check to make sure we don't check outside of picture
		Assume radius of holeSize will be less than 20 pixels
		 */
		if (renP->boundBoxx%2 != 0) renP->boundBoxx ++;
		if (renP->boundBoxy%2 != 0) renP->boundBoxy ++;
		if (renP->boundBoxx > renP->width) renP->boundBoxx = renP->width - 20;
		if (renP->boundBoxy > renP->height) renP->boundBoxy = renP->height - 20;
		break;
	case 14:
		sscanf(buf, "%d",&renP->distShower);
		break;
		case 15:
		sscanf(buf, "%d", &renP->camTestPoints);
		break;
	case 16:
		sscanf(buf, "%d", &renP->holeSize);
		/*
		If holesize is greater than 20 pixels make sure to change the bounding
		box check (at the time of writing case 13)
		 */
		break;
	case 17:
		sscanf(buf, "%d", &renP->renderFromFile);
		break;
	case 18:
		strcpy(renP->frame_dat_file, buf);
		break;
	}

      count++;
    }
  fclose(fp);
}

