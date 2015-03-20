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
#include "framedat.h"

#define BUF_SIZE 1024

static char buf[BUF_SIZE];

int getFrameData(char *filename, FrameData (**frame_params),CameraParams *camP, RenderParams *renP)
{
  printf("getting frame info from file...\n");
  FILE *fp;
  int ret;
  // double *d = 0;

  int numframes = 0;

  // renP.fractalType = 0;
  // renP.maxRaySteps = 2000;
  // renP.maxDistance = 100;

  fp = fopen(filename,"r");

  if( !fp )
    {
      printf(" *** File %s does not exist\n", filename);
      exit(1);
    }

  int count = 0;
  int index = 0;

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

	  sscanf(buf, "%d", &numframes);

	  *frame_params = (FrameData *)malloc(numframes * sizeof( FrameData));

	  break;
	case 1:
	count --;

		/*
		Read in an insane number of paramters.
		 */
		sscanf(buf, "%lf %lf %lf \
					 %lf %lf %lf \
					 %lf %lf %lf \
					 %lf\
					 %d\
					 %d\
					 %lf\
					 %f\
					 %f\
					 %f\
					 %d\
					 %f\
					 %d\
					 %f\
					 %d\
					 %s", \
			&(*frame_params)[index].camPos[0],\
			&(*frame_params)[index].camPos[1],\
		 &(*frame_params)[index].camPos[2],\
		&(*frame_params)[index].camTarget[0],\
		&(*frame_params)[index].camTarget[1],\
		&(*frame_params)[index].camTarget[2],\
		&(*frame_params)[index].camUp[0],\
		&(*frame_params)[index].camUp[1],\
		&(*frame_params)[index].camUp[2],\
		&(*frame_params)[index].fov,\
		&(*frame_params)[index].width,\
		&(*frame_params)[index].height,\
		&(*frame_params)[index].detail,\
		&(*frame_params)[index].scale,\
		&(*frame_params)[index].rMin,\
		&(*frame_params)[index].rFixed,\
		&(*frame_params)[index].num_iter,\
		&(*frame_params)[index].escape_time,\
		&(*frame_params)[index].colourType,\
		&(*frame_params)[index].brightness,\
		&(*frame_params)[index].super_sampling,\
		(*frame_params)[index].file_name);
		// printf("Hi %f\n",(*frame_params)[index].camPos[0] );
		index ++;
		break;

	}
count ++;
    }
  fclose(fp);
  /*
  Also turn off camera path planning in the redenderer. Ie we do not use any of the
  camera stuff
   */
  renP->enable = 0;
  renP->width= (*frame_params)[0].width;
  renP->height= (*frame_params)[0].height;
  return numframes;
}

void updateCamRenParams(int currFrame , FrameData (**fp),CameraParams *camP, RenderParams *renP, MandelBoxParams *mbP){
	// printf("Updating render and camera params\n");

	camP->camPos[0] = (*fp)[currFrame].camPos[0];
	camP->camPos[1] = (*fp)[currFrame].camPos[1];
	camP->camPos[2] = (*fp)[currFrame].camPos[2];
	camP->camTarget[0] = (*fp)[currFrame].camTarget[0];
	camP->camTarget[1] = (*fp)[currFrame].camTarget[1];
	camP->camTarget[2] = (*fp)[currFrame].camTarget[2];
	camP->camUp[0] = (*fp)[currFrame].camUp[0];
	camP->camUp[1] = (*fp)[currFrame].camUp[1];
	camP->camUp[2] = (*fp)[currFrame].camUp[2];
	camP->fov= (*fp)[currFrame].fov;
	renP->width= (*fp)[currFrame].width;
	renP->height= (*fp)[currFrame].height;
	renP->detail= (*fp)[currFrame].detail;
	mbP->scale= (*fp)[currFrame].scale;
	mbP->rMin= (*fp)[currFrame].rMin;
	mbP->rFixed= (*fp)[currFrame].rFixed;
	mbP->num_iter= (*fp)[currFrame].num_iter;
	mbP->escape_time= (*fp)[currFrame].escape_time;
	renP->colourType= (*fp)[currFrame].colourType;
	renP->brightness= (*fp)[currFrame].brightness;
	renP->super_sampling= (*fp)[currFrame].super_sampling;



}
