#include <stdlib.h>
#include <stdio.h>
#include "vector3d.h"
#include "camera.h"
#include "framedat.h"
#include "renderer.h"
#include "mandelbox.h"
#include "math.h"
#include "3d.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

void writeFrameData(int frame_num, CameraParams &camera_params, RenderParams &renderer_params, MandelBoxParams &mandelBox_params)
{

  ofstream frame_dat;
  frame_dat.open("frames.dat", ofstream::out | ofstream::app); //complete list of all parameters

  frame_dat<<frame_num<<" "
  <<camera_params.camPos[0]<<" "<<camera_params.camPos[1]<<" "<<camera_params.camPos[2]<<" "
  <<camera_params.camTarget[0]<<" "<<camera_params.camTarget[1]<<" "<<camera_params.camTarget[2]<<" "
  <<camera_params.camUp[0]<<" "<<camera_params.camUp[1]<<" "<<camera_params.camUp[2]<<" "
  <<camera_params.fov<<" "
  <<renderer_params.width<<" "<<renderer_params.height<<" "
  <<renderer_params.detail<<" "
  <<mandelBox_params.scale<<" "<<mandelBox_params.rMin<<" "<<mandelBox_params.rFixed<<" "
  <<mandelBox_params.num_iter<<" "<<mandelBox_params.escape_time<<" "
  <<renderer_params.colourType<<" "
  <<renderer_params.brightness<<" "
  <<renderer_params.super_sampling<<" "
  <<frame_num<<".bmp"
  <<"\n";

  frame_dat.close();

}
