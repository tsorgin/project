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

// ls -1v | grep bmp > files.txt && mencoder -nosound -ovc lavc -lavcopts vcodec=mpeg4:vbitrate=21600000 -o out.avi -mf type=bmp:fps=30 mf://@files.txt -vf scale=1080:720;


#include <stdlib.h>
#include <stdio.h>
#include "vector3d.h"
#include "camera.h"
#include "framedat.h"
#include "renderer.h"
#include "mandelbox.h"
#include "math.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

void getParameters(char *filename, CameraParams *camera_params, RenderParams *renderer_params,
       MandelBoxParams *mandelBox_paramsP);
int getFrameData(char *filename, FrameData (**frame_params),CameraParams *camera_params,
 RenderParams *renderer_params);
void updateCamRenParams(int currFrame,FrameData (**frame_params),CameraParams *camera_params,
 RenderParams *renderer_params ,MandelBoxParams *mandelBox_paramsP);
void init3D       (CameraParams *camera_params, const RenderParams *renderer_params);
void renderFractal(const CameraParams &camera_params,  RenderParams &renderer_params, unsigned char* image, vec3 &New_dir);
void saveBMP      (const char* filename, const unsigned char* image, int width, int height);

MandelBoxParams mandelBox_params;

int main(int argc, char** argv)
{
  ofstream logfile;
  logfile.open("log.dat");


  CameraParams    camera_params;
  RenderParams    renderer_params;
  FrameData    (*frame_params);
  int numframes = 1; //Number of frames to render
  vec3 New_dir;

  float newPos_resolution = 0.009;  // The influence of the new camera target on the old camera target (jitter)
  float max_TargetChange = 0.5;  // Max influence of the new camera target on the old camera target
  float max_FrameStep = 0.0001;  // Max distance travelled down vector between camera and camera target
  logfile << "newPos_resolution: " << newPos_resolution << "\nmax_TargetChange: " << max_TargetChange << "\nmax_FrameStep: " << max_FrameStep << "\n";


  getParameters(argv[1], &camera_params, &renderer_params, &mandelBox_params);
  if (renderer_params.renderFromFile == 1)
  {
    numframes = getFrameData(renderer_params.frame_dat_file, &frame_params,&camera_params, &renderer_params);
  }

  printf("Number of frames: %d\n", numframes);
  // printf("file name 2 %s\n",frame_params[0].file_name);
  // printf("file name 2 %s\n",frame_params[1].file_name);




  renderer_params.old_max_distance = 0.017113; //Take this out eventually

  int image_size = renderer_params.width * renderer_params.height;
  unsigned char *image = (unsigned char*)malloc(3*image_size*sizeof(unsigned char));
  logfile << "Resolution: " << renderer_params.width << "x" << renderer_params.height << "\n";

  init3D(&camera_params, &renderer_params);

  for (int i = 0; i < numframes; i++)
  {

    printf("Frame %d\n", i);
    if (renderer_params.enable == 1)
    {
      printf("CAMERA TARGET: %f, %f, %f\n",camera_params.camTarget[0],camera_params.camTarget[1],camera_params.camTarget[2]);

      // CAMERA LOCATION
      // CAMERA FRAME X Y Z
      logfile << "C" << " " << i << " " << camera_params.camPos[0] << " " << camera_params.camPos[1] << " " << camera_params.camPos[2] << "\n";

      // TARGET LOCATION
      // TARGET FRAME X Y Z
      logfile << "T" << " " << i << " " << camera_params.camTarget[0] << " " << camera_params.camTarget[1] << " " << camera_params.camTarget[2] << "\n";

      //////////////////////////////////////////////////////////////////////////////////////
      ///Taylor working here
      //fabs(target.Dot(axis))> max_axis_step ? max_axis_step:target.Dot(axis) is the right idea. BUt if one fails,
      //you must scale all of them by the same amount.... also have to make sure to keep sign.

      double max_axis_step = .005;
      vec3 axis, target;
      target.x = camera_params.camTarget[0];
      target.y = camera_params.camTarget[1];
      target.z = camera_params.camTarget[2];
      target.Normalize();

      axis.x = 1;

      camera_params.camPos[0] = camera_params.camPos[0] + (target.Dot(axis)*max_axis_step );

      axis.x = 0;
      axis.y = 1;

      camera_params.camPos[1] = camera_params.camPos[1] + (target.Dot(axis)*max_axis_step );

      axis.y = 0;
      axis.z = 1;

      camera_params.camPos[2] = camera_params.camPos[2] + (target.Dot(axis)*max_axis_step );

      printf("%f %f %f \n",camera_params.camPos[0],camera_params.camPos[1],camera_params.camPos[2] );
      // camera_params.camPos[0] = camera_params.camPos[0] - .025;
      // camera_params.camPos[1] = camera_params.camPos[1] - .025;
      // camera_params.camPos[2] = camera_params.camPos[2] - .025;
      ///////////////////////////////////////////////////////////////////////////////////////////


      renderFractal(camera_params, renderer_params, image, New_dir);

      // printf("NEW CAMERA DIRECTION: %f, %f, %f\n", New_dir.x,New_dir.y,New_dir.z);
      //   if (1){
      //       // CALCULATE THE NEW CAMERA TARGET
      //     if(fabs((New_dir.x*newPos_resolution)) < max_TargetChange){
      //       camera_params.camTarget[0] = camera_params.camTarget[0] + New_dir.x*newPos_resolution;
      //       printf("Adding %lf to x\n", New_dir.x*newPos_resolution);
      //     }

      //     // else
      //     //   camera_params.camTarget[0] = camera_params.camTarget[0] + (New_dir.x < 0) ? -1 : (New_dir.x > 0)*max_TargetChange;

      //     if(fabs((New_dir.y*newPos_resolution)) < max_TargetChange){
      //       camera_params.camTarget[1] = camera_params.camTarget[1] + New_dir.y*newPos_resolution;
      //       printf("Adding %lf to y\n", New_dir.y*newPos_resolution);
      //     }
      //     // else
      //     //   camera_params.camTarget[1] = camera_params.camTarget[1] + (New_dir.y < 0) ? -1 : (New_dir.y > 0)*max_TargetChange;

      //     if(fabs((New_dir.z*newPos_resolution)) < max_TargetChange){
      //       camera_params.camTarget[2] = camera_params.camTarget[2] + New_dir.z*newPos_resolution;
      //       printf("Adding %lf to z\n", New_dir.z*newPos_resolution);
      //     }
      //     // else
      //     //   camera_params.camTarget[2] = camera_params.camTarget[2] + (New_dir.z < 0) ? -1 : (New_dir.z > 0)*max_TargetChange;

      // }
      double dist = 0;

      if (i ==  0)
      {
      camera_params.camTarget[0] = New_dir.x;
      camera_params.camTarget[1] = New_dir.y;
      camera_params.camTarget[2] = New_dir.z;
      }




      // CALCULATE THE PATH ALONG WHICH THE CAMERA MOVES
      // dist = camera_params.camTarget[0] - camera_params.camPos[0];
      // camera_params.camPos[0] = dist*max_FrameStep+  camera_params.camPos[0];

      // dist = camera_params.camTarget[1] - camera_params.camPos[1];
      // camera_params.camPos[1] = dist*max_FrameStep +  camera_params.camPos[1];

      // dist = camera_params.camTarget[2] - camera_params.camPos[2];
      // camera_params.camPos[2] = dist*max_FrameStep +  camera_params.camPos[2];


      // MAP TO 3D AND FIND NEW CAMERA TARGET
      init3D(&camera_params, &renderer_params);
      printf("NEW CAMERA TARGET: %f, %f, %f\n",camera_params.camTarget[0],camera_params.camTarget[1],camera_params.camTarget[2]);
      printf("**********************************************\n");

      char buf[10];
      sprintf(buf,"%d.bmp", i);
      saveBMP(buf, image, renderer_params.width, renderer_params.height);
    }

    if (renderer_params.renderFromFile == 1)
    {
      /*
      Load next frame into camera and renderer
       */
      updateCamRenParams(i ,&frame_params,&camera_params, &renderer_params,&mandelBox_params);
      init3D(&camera_params, &renderer_params);
      renderFractal(camera_params, renderer_params, image, New_dir);

      saveBMP(frame_params[i].file_name, image, renderer_params.width, renderer_params.height);

    }

  }



  free(image);
  free(frame_params);
  logfile.close();

  return 0;
}
