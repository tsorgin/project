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
void writeFrameData(int frame_num, CameraParams &camera_params, RenderParams &renderer_params, MandelBoxParams &mandelBox_paramsP);

MandelBoxParams mandelBox_params;

int main(int argc, char** argv)
{
  ofstream logfile;
  logfile.open("log.dat");

  CameraParams    camera_params;
  RenderParams    renderer_params;
  FrameData    (*frame_params);
  vec3 New_dir;

  int numframes = 200; //Number of frames to render
  double newPos_resolution = 0.05;  // The influence of the new camera target on the old camera target (jitter)
  double max_TargetChange = 2.5;  // Max influence of the new camera target on the old camera target
  double max_FrameStep = 0.000001;  // Max distance travelled down vector between camera and camera target
  double dist; // How far to move per frame
  logfile << "newPos_resolution: " << newPos_resolution << "\nmax_TargetChange: " << max_TargetChange << "\nmax_FrameStep: " << max_FrameStep << "\n";


  getParameters(argv[1], &camera_params, &renderer_params, &mandelBox_params);
  if (renderer_params.renderFromFile == 1)
  {
    numframes = getFrameData(renderer_params.frame_dat_file, &frame_params,&camera_params, &renderer_params);
  }

  printf("Number of frames: %d\n", numframes);

  renderer_params.old_max_distance = 0.017113; //Take this out eventually
  int image_size = renderer_params.width * renderer_params.height;
  unsigned char *image = (unsigned char*)malloc(3*image_size*sizeof(unsigned char));
  logfile << "Resolution: " << renderer_params.width << "x" << renderer_params.height << "\n";

  init3D(&camera_params, &renderer_params);

  for (int i = 0; i < numframes; i++)
  {

    // SEND FRAME DATA TO FILE
    writeFrameData(i, camera_params, renderer_params, mandelBox_params);

    printf("Frame %d\n", i);
    // RENDER FRAMES USING PATH PLANNING
    if (renderer_params.enable == 1)
    {
      // CAMERA LOCATION
      // CAMERA FRAME X Y Z
      printf("CAMERA LOCATION: %f, %f, %f\n",camera_params.camPos[0],camera_params.camPos[1],camera_params.camPos[2]);
      logfile << "C" << " " << i << " " << camera_params.camPos[0] << " " << camera_params.camPos[1] << " " << camera_params.camPos[2] << "\n";

      // TARGET LOCATION
      // TARGET FRAME X Y Z
      printf("CAMERA TARGET: %f, %f, %f\n",camera_params.camTarget[0],camera_params.camTarget[1],camera_params.camTarget[2]);
      logfile << "T" << " " << i << " " << camera_params.camTarget[0] << " " << camera_params.camTarget[1] << " " << camera_params.camTarget[2] << "\n";

      // RENDER THE FRAME
      renderFractal(camera_params, renderer_params, image, New_dir);
      printf("NEW CAMERA DIRECTION: %f, %f, %f\n", New_dir.x,New_dir.y,New_dir.z);


      //         x -= 1
      // lookat[1] -= .5
      // pos = np.array([9.5 ,7.5 ,6.5])
      // up = np.array([0,1,0])
      // at = lookat - pos
      // at/= np.linalg.norm(at)
      // right = np.cross(at, up)
      // right/= np.linalg.norm(right)
      // up = np.cross(at,right)
      
      
      // CALCULATE THE NEW CAMERA TARGET (with LOW-PASS FILTER)
      // if the target is within some range of the last target, change the target.
      // Else target is unchanged.
      if(fabs((New_dir.x*newPos_resolution)) < max_TargetChange){
        camera_params.camTarget[0] = camera_params.camTarget[0] + New_dir.x*newPos_resolution;
        printf("Adding %lf to x\n", New_dir.x*newPos_resolution);
      }

      if(fabs((New_dir.y*newPos_resolution)) < max_TargetChange){
        camera_params.camTarget[1] = camera_params.camTarget[1] + New_dir.y*newPos_resolution;
        printf("Adding %lf to y\n", New_dir.y*newPos_resolution);
      }

      if(fabs((New_dir.z*newPos_resolution)) < max_TargetChange){
        camera_params.camTarget[2] = camera_params.camTarget[2] + New_dir.z*newPos_resolution;
        printf("Adding %lf to z\n", New_dir.z*newPos_resolution);
      }

      // CALCULATE THE PATH ALONG WHICH THE CAMERA MOVES
      // Move the camera max_FrameStep along the path
      dist = camera_params.camTarget[0] - camera_params.camPos[0];
      camera_params.camPos[0] = dist*max_FrameStep + camera_params.camPos[0];

      dist = camera_params.camTarget[1] - camera_params.camPos[1];
      camera_params.camPos[1] = dist*max_FrameStep +  camera_params.camPos[1];

      dist = camera_params.camTarget[2] - camera_params.camPos[2];
      camera_params.camPos[2] = dist*max_FrameStep +  camera_params.camPos[2];


      // MAP TO 3D AND FIND NEW CAMERA TARGET
      init3D(&camera_params, &renderer_params);
      printf("NEW CAMERA TARGET: %f, %f, %f\n",camera_params.camTarget[0],camera_params.camTarget[1],camera_params.camTarget[2]);
      printf("**********************************************\n");
      // FILE NAME FOR IMAGE
      char buf[10];
      sprintf(buf,"%d.bmp", i);
      saveBMP(buf, image, renderer_params.width, renderer_params.height);
    }

    // RENDER FRAMES USING DATA FILE
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

  logfile.close();
  free(image);

  if (renderer_params.renderFromFile == 1)
  {
    free(frame_params);
  }


  return 0;
}