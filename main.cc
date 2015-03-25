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
#include "Quaternion.h"
#include "mpi.h"
using namespace std;

extern "C"{
void getParameters(char *filename, CameraParams *camera_params, RenderParams *renderer_params,
       MandelBoxParams *mandelBox_paramsP);
void init3D       (CameraParams *camera_params, const RenderParams *renderer_params);
void saveBMP      (const char* filename, const unsigned char* image, int width, int height);
int getFrameData(char *filename, FrameData (**frame_params),CameraParams *camera_params,
 RenderParams *renderer_params);
void updateCamRenParams(int currFrame,FrameData (**frame_params),CameraParams *camera_params,
 RenderParams *renderer_params ,MandelBoxParams *mandelBox_paramsP);
}

void camUP_Calc (CameraParams &camera_params, Quaternion &Orient);
void writeFrameData(int frame_num, CameraParams &camera_params, RenderParams &renderer_params, MandelBoxParams &mandelBox_paramsP);
vec3 Cross(vec3 V1, vec3 V2);

void renderFractal(CameraParams &camera_params,  RenderParams &renderer_params, unsigned char* image, vec3 &New_dir);

MandelBoxParams mandelBox_params;

void parallelRender(CameraParams &camera_params, RenderParams &renderer_params, unsigned char* image, vec3 &New_dir, MPI_Status status) {
  // MPI parallizing the renderFractal
  int my_rank;            // rank of process
  int numP;               // number of processes
  int tag = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &numP);
  int processChunk = renderer_params.height/numP; //decide on how much pixels each process is responsible
  unsigned char *processImage = (unsigned char*)malloc(3 * processChunk * renderer_params.width * sizeof(unsigned char)); 

	if (my_rank != numP-1)
  {
      renderer_params.height = processChunk; // the height relating to each P has changed
      renderFractal(camera_params, renderer_params, processImage, New_dir);
      // sending the image to last process
      printf("am I alive %d \n", my_rank);
      int numofSend = processChunk * renderer_params.width * 3;
      MPI_Send( processImage, numofSend, MPI_CHAR, numP-1, tag, MPI_COMM_WORLD);
    
  }
	else // this is the last process
  {
      int oriHeight = renderer_params.height;
      //Note: my_rank = numP - 1
      renderer_params.height = oriHeight - my_rank*processChunk;
      renderFractal(camera_params, renderer_params, &image[ my_rank * processChunk * renderer_params.width*3], New_dir);
      printf("am I alive %d \n", my_rank);
      //receiving image
      int numofRev = processChunk * renderer_params.width * 3;
      for (int revCount=0; revCount < numP-1; revCount++)
      {
          MPI_Recv(&image[revCount*numofRev], numofRev, MPI_CHAR, revCount, tag,MPI_COMM_WORLD, &status);
      }
        //saveBMP(renderer_params.file_name, image, renderer_params.width, oriHeight);
  }
	free(processImage);
}

int main(int argc, char** argv)
{
  ofstream logfile;
  logfile.open("log.dat");

  CameraParams    camera_params;
  RenderParams    renderer_params;
  FrameData    (*frame_params);

  vec3 New_dir;
  // vec3 New_dir, VectorTo, LocalVector, Axis, ThirdVect;
  // Quaternion AxisAngle;
  Quaternion Orient;

  // double Angle;

  int numframes = 1000; //Number of frames to render
  double newPos_resolution = 0.05;  // The influence of the new camera target on the old camera target (jitter) 0.05
  double max_TargetChange = 2.5;  // Max influence of the new camera target on the old camera target
  double max_FrameStep = 0.0001;  // Max distance travelled down vector between camera and camera target 0.00001
  double dist; // How far to move per frame
  logfile << "newPos_resolution: " << newPos_resolution << "\n max_TargetChange: " << max_TargetChange << "\n max_FrameStep: " << max_FrameStep << "\n";


  getParameters(argv[1], &camera_params, &renderer_params, &mandelBox_params);
  if (renderer_params.renderFromFile == 1)
  {
    numframes = getFrameData(renderer_params.frame_dat_file, &frame_params,&camera_params, &renderer_params);
  }

  printf("Number of frames: %d\n", numframes);


  //Init orientation quaternion
  Orient.x = camera_params.camUp[0];
  Orient.y = camera_params.camUp[1];
  Orient.z = camera_params.camUp[2];
  Orient.w = 0.0f;

  //CALCULATE NEW UP VECTOR
  camUP_Calc(camera_params, Orient);

  renderer_params.old_max_distance = 0.017113; //Take this out eventually
  int image_size = renderer_params.width * renderer_params.height;
  unsigned char *image = (unsigned char*)malloc(3*image_size*sizeof(unsigned char));
  logfile << "Resolution: " << renderer_params.width << "x" << renderer_params.height << "\n";

  init3D(&camera_params, &renderer_params);

	// MPI_init
  MPI_Status  status;
  MPI_Init(&argc, &argv);


  for (int i = 0; i < numframes; i++)
  {

    // SEND FRAME DATA TO FILE
    //writeFrameData(i, camera_params, renderer_params, mandelBox_params);

    printf("Frame %d\n", i);
    // RENDER FRAMES USING PATH PLANNING
    if (renderer_params.enable == 1)
    {
      // CAMERA LOCATION
      // CAMERA FRAME X Y Z
      printf("CAMERA LOCATION: %f, %f, %f\n",camera_params.camPos[0],camera_params.camPos[1],camera_params.camPos[2]);
      logfile << "C" << " " << i << " " << camera_params.camPos[0] << " " << camera_params.camPos[1] << " " << camera_params.camPos[2] << "\n";

      // CAMERA UP VECTOR
      // CAMERA UP VECTOR X Y Z
      printf("CAMERA UP VECTOR: %f, %f, %f\n",camera_params.camUp[0],camera_params.camUp[1],camera_params.camUp[2]);
      logfile << "U" << " " << i << " " << camera_params.camUp[0] << " " << camera_params.camUp[1] << " " << camera_params.camUp[2] << "\n";

      // TARGET LOCATION
      // TARGET FRAME X Y Z
      printf("CAMERA TARGET: %f, %f, %f\n",camera_params.camTarget[0],camera_params.camTarget[1],camera_params.camTarget[2]);
      logfile << "T" << " " << i << " " << camera_params.camTarget[0] << " " << camera_params.camTarget[1] << " " << camera_params.camTarget[2] << "\n";

      // RENDER THE FRAME
      //renderFractal(camera_params, renderer_params, image, New_dir);
			parallelRender(camera_params, renderer_params, image, New_dir, status);
      printf("NEW CAMERA DIRECTION: %f, %f, %f\n", New_dir.x,New_dir.y,New_dir.z);

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


      printf("**********************************************\n");
      printf("CAMERA LOCATION: %f, %f, %f\n",camera_params.camPos[0],camera_params.camPos[1],camera_params.camPos[2]);
      printf("CAMERA UP VECTOR: %f, %f, %f\n",camera_params.camUp[0],camera_params.camUp[1],camera_params.camUp[2]);
      printf("CAMERA TARGET: %f, %f, %f\n",camera_params.camTarget[0],camera_params.camTarget[1],camera_params.camTarget[2]);

      //CALCULATE NEW UP VECTOR
      camUP_Calc(camera_params, Orient);

      printf("New Up == %lf, %lf, %lf\n", camera_params.camUp[0], camera_params.camUp[1], camera_params.camUp[2]);

      printf("**********************************************\n");
      init3D(&camera_params, &renderer_params);
      printf("NEW CAMERA TARGET: %f, %f, %f\n",camera_params.camTarget[0],camera_params.camTarget[1],camera_params.camTarget[2]);
      printf("**********************************************\n");

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
      
			//renderFractal(camera_params, renderer_params, image, New_dir);
			parallelRender(camera_params, renderer_params, image, New_dir, status);
      saveBMP(frame_params[i].file_name, image, renderer_params.width, renderer_params.height);

    }

  }

  logfile.close();
  free(image);
  
	if (renderer_params.renderFromFile == 1)
  {
    free(frame_params);
  }
	

	MPI_Finalize();
  return 0;
}

vec3 Cross(vec3 V1, vec3 V2){
  return vec3(V1.y*V2.z-V2.y*V1.z,V1.z*V2.x-V2.z*V1.x,V1.x*V2.y-V2.x*V1.y);
}


void camUP_Calc (CameraParams &camera_params, Quaternion &Orient){
  Quaternion AxisAngle;
  vec3 New_dir, VectorTo, LocalVector, Axis, ThirdVect, direction;
  double Angle;

  // CALCULATE THE NEW UP VECTOR
  printf("Orientation == %lf, %lf, %lf, %lf\n", Orient.x, Orient.y, Orient.z, Orient.w);

  // VectorTo = cameraTarget - cameraPosition -- then normalize
  VectorTo = SubtractDoubleDouble(camera_params.camTarget,camera_params.camPos);
  VectorTo.Normalize();
  printf("VectorTo == %lf, %lf, %lf\n", VectorTo.x, VectorTo.y, VectorTo.z);

  // Straight-ahead vector
  direction = vec3(0,0,-1);
  LocalVector = Quaternion_multiplyVector(Orient, direction);
  LocalVector.Normalize();
  printf("LocalVector == %lf, %lf, %lf\n", LocalVector.x, LocalVector.y, LocalVector.z);

  //Get the cross product as the axis of rotation
  Axis = Cross(VectorTo,LocalVector);
  Axis.Normalize();
  printf("Axis == %lf, %lf, %lf\n", Axis.x, Axis.y, Axis.z);

  //Get the dot product to find the angle
  Angle = acos(VectorTo.x*LocalVector.x + VectorTo.y*LocalVector.y + VectorTo.z*LocalVector.z);
  printf("Angle == %lf \n",Angle);

  //Determine whether or not the angle is positive
  //Get the cross product of the axis and the local vector
  ThirdVect = Cross(Axis,LocalVector);
  ThirdVect.Normalize();
  printf("ThirdVect == %lf, %lf, %lf\n", ThirdVect.x, ThirdVect.y, ThirdVect.z);

  //If the dot product of that and the local vector is negative, so is the angle
  if (ThirdVect.x*VectorTo.x + ThirdVect.y*VectorTo.y + ThirdVect.z*VectorTo.z < 0)
  {
      Angle = -Angle;
  }

  //Finally, create a quaternion
  AxisAngle = Quaternion_fromAxisAngle(Angle, Axis);

  //And multiply it into the current orientation
  Orient = Quaternion_Mul_OrientationAxis(AxisAngle, Orient);
  printf("New Orientation == %lf, %lf, %lf, %lf \n", Orient.x, Orient.y, Orient.z, Orient.w);

  // Grab the new up vector from quaternion
  camera_params.camUp[0] = 2 * (Orient.x * Orient.y - Orient.w * Orient.z);
  camera_params.camUp[1] = 1 - 2 * (Orient.x * Orient.x + Orient.z * Orient.z);
  camera_params.camUp[2] = 2 * (Orient.y * Orient.z + Orient.w * Orient.x);
}
