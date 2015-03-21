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
#include "color.h"
#include "mandelbox.h"
#include "camera.h"
#include "vector3d.h"
#include "3d.h"

extern double getTime();
void colour_camBoundBox(unsigned char * image, double * x, int bounding_box_x, int bounding_box_y, int width, int height);
void choose_cam_zoom_point(unsigned char * image, double *x, int bounding_box_x, int bounding_box_y, double scalefactor, const RenderParams &render_params, int * ii, int *jj, double * max_distance);
void colour_close(unsigned char *image, double * x, int width, int height, double min_distance);
extern void   printProgress( double perc, double time );
int holesize (unsigned char * image, double * x, int i , int j,  double max_distance, int width, int height, int radius, int check);
void convert_to_distancemap (unsigned char *image, double * x, int width, int height,  double scalefactor);
void colour_focus_dir (unsigned char *image, int i, int j, int width, int height,int radius);
extern void rayMarch (const RenderParams &render_params, const vec3 &from, const vec3  &to, pixelData &pix_data);
extern vec3 getColour(const pixelData &pixData, const RenderParams &render_params,
		      const vec3 &from, const vec3  &direction, double max_distance);

void renderFractal(const CameraParams &camera_params,  RenderParams &renderer_params,
		   unsigned char* image, vec3 &max_dist)
{
  double farPoint[3];
  double scalefactor = 0.0;
  vec3 to, from;

  from.SetDoublePoint(camera_params.camPos);

  int height = renderer_params.height;
  int width  = renderer_params.width;
/*
We will estimate the max distance in this frame with max distance from the previous frame.
This is used for fogging.
 */
double prev_max_distance = renderer_params.old_max_distance; //Currently just manually put in

  pixelData pix_data;
	double * x = new double [height*width];
  double time = getTime();
 	 int bounding_box_x = (width - renderer_params.boundBoxx)/2 ;
	int bounding_box_y = (height - renderer_params.boundBoxy)/2;



  for(int j = 0; j < height; j++)
    {
      //for each column pixel in the row
      for(int i = 0; i <width; i++)
	{
		//printf("\n");
	  vec3 color;
	  if( renderer_params.super_sampling == 1 )
	    {
		  vec3 samples[9];
		  int idx = 0;
		  for(int ssj = -1; ssj < 2; ssj++){
			for(int ssi = -1; ssi< 2; ssi++){
				//unproject looks at centre of pixel...
			  UnProject(i+ssi*0.5, j+ssj*0.5, camera_params, farPoint);
				// printf("At pixel: %d %d SS at: %f %f Converts to: %f, %f, %f \n", i,j,i+ssi*0.5,j+ssj*0.5, farPoint[0],farPoint[1],farPoint[2]);

			  //printf("%f %f %f\n", farPoint[0],farPoint[1],farPoint[2]);
			  // to = farPoint - camera_params.camPos
			  to = SubtractDoubleDouble(farPoint,camera_params.camPos);
			  to.Normalize();

			  //render the pixel
			  rayMarch(renderer_params, from, to, pix_data);


			  //get the colour at this pixel
			  samples[idx] = getColour(pix_data, renderer_params, from, to,prev_max_distance);
			  //printf(" Sample colour: %f %f %f\n",samples[idx].x,samples[idx].y,samples[idx].z );
			  idx++;
			}
	      }

	      color = (samples[0]*0.05 + samples[1]*0.1 + samples[2]*0.05 +
		       samples[3]*0.1  + samples[4]*0.4 + samples[5]*0.1  +
		       samples[6]*0.05 + samples[7]*0.1 + samples[8]*0.05);

	    }
	  else
	    {
	      // get point on the 'far' plane
	      // since we render one frame only, we can use the more specialized method
	      UnProject(i, j, camera_params, farPoint);

	      // to = farPoint - camera_params.camPos
	      to = SubtractDoubleDouble(farPoint,camera_params.camPos);
	      to.Normalize();

	      //render the pixel
	      rayMarch(renderer_params, from, to, pix_data);

	      //get the colour at this pixel
	      color = getColour(pix_data, renderer_params, from, to,prev_max_distance);
	    }


	    x[j * width + i] = pix_data.distance;

			  if (pix_data.escaped)
			  {
			  	x[j * width + i] = 100;
			  }
			  if (pix_data.distance > scalefactor && pix_data.escaped == false )
			  {
			  	scalefactor = pix_data.distance;
			  }

	  //save colour into texture
	  int k = (j * width + i)*3;
	  image[k+2] = (unsigned char)(color.x * 255);
	  image[k+1] = (unsigned char)(color.y * 255);
	  image[k]   = (unsigned char)(color.z * 255);
	}
      printProgress((j+1)/(double)height,getTime()-time);
    }

    renderer_params.old_max_distance = scalefactor; //We use this inthe next frame as a guess when we do colouring

    printf("Max camera distance %f\n", scalefactor);
    if (renderer_params.enable == 1)
    {
    	/*
    	This section of code does camera planning
    	This involves writing a log file (done in main)
    	*/

	    int ii=0, jj=0;
	    double max_distance = 0.0;
	    double min_distance = 0.0000007;

	    colour_close(image, x, width, height,min_distance);
	    choose_cam_zoom_point(image, x, bounding_box_x, bounding_box_y, scalefactor, renderer_params, &ii, &jj, &max_distance);


	    /*
	    If we colour the distance map based off of the max the camera can see it will help
	    make the distacne map be more readable.
	     */
	    if (renderer_params.colourType == 2){
	    	/*
	    	convert the output image to a greyscale image that shows distance of objects
	    	from the camera. Sometiems this image will look really bad simply because of
	    	scaling issues (to get within the 255 rgb limit)
	    	 */
	    	convert_to_distancemap (image,  x,  width,  height, max_distance);
		}


		if (renderer_params.camBoundBox == 1){
			/*
			Displays all of the points where the camera is searching for a new
			max distance.
			 */
			colour_camBoundBox(image, x, bounding_box_x, bounding_box_y, height, width);
		}

	    if (renderer_params.distShower == 1)
	    {
	    	/*
	    	Will show were the camera wants look. makes a little blue dot
	    	 */
	    	colour_focus_dir(image, ii, jj, width, height,renderer_params.holeSize);
	    }

	    double xx [3];
	    UnProject(ii, jj, camera_params, xx);
	    max_dist.x = xx[0];
	    max_dist.y = xx[1];
	    max_dist.z = xx[2];
    }

    delete(x);
    //printf("%f %f %f\n", max_dist.x,max_dist.y,max_dist.z);
    //printf("%f max distance is.Min: %f\n",max_distance , min_distance);
}

int holesize (unsigned char * image, double * x,  int ii, int jj, double scalefactor, int width, int height, int radius, int camTestPoints){
	/*
	Checks to make sure that some of the surronding pixels are at least close (so we don't try and go through a
	really small hole)
	 */


	double mean = 0.0; //Temp varaible to calculate the average of distance in points.
	int counter = 0;
	// printf("Checking hole size at: %d, %d\n", ii, jj);


	// Calculate the mean of our values (inside the circle)
	for (int i = ii-radius-1; i < ii+radius+1; i++)
	{
		for (int j = jj-radius-1; j < jj+radius+1; j++)
		{
			double dx = i - ii;
			double dy = j - jj;
			double sqrdist = dx*dx + dy*dy;
			// printf("i %d, j%d\n", i,j);
			if (sqrdist <= radius*radius){
				int k = (i*width + j); //Had this as (j*width +i) which Ithink was backwards.
				mean = mean + x[k];
				counter ++;

			}
		}
	}
	mean = mean/counter;
	double sum =0.0;
	//Clauclate the standard deviation
	for (int i = ii-radius-1; i < ii+radius+1; i++)
	{
		for (int j = jj-radius-1; j < jj+radius+1; j++)
		{
			double dx = i - ii;
			double dy = j - jj;
			double sqrdist = dx*dx + dy*dy;
			// printf("i %d, j%d\n", i,j);
			if (sqrdist <= radius*radius){
				int k = (i*width + j); //Had this as (j*width +i) which Ithink was backwards.
				sum = sum + (x[k]-mean)*(x[k]-mean);
			}
		}
	}
	sum = sum/counter;
	double standard_dev = sqrt(sum); //The standard deviation of distance values in our circle

	// printf("Deviation is: %f\n",standard_dev );
	int active_pix = (ii*width+ jj);
	for (int i = ii-radius-1; i < ii+radius+1; i++)
	{
		for (int j = jj-radius-1; j < jj+radius+1; j++)
		{
			double dx = i - ii;
			double dy = j - jj;
			double sqrdist = dx*dx + dy*dy;

			if (sqrdist <= radius*radius){
				int k = (i*width+j);

				if (x[k] <x[active_pix] -standard_dev*1.5 ||  x[k] >x[active_pix] +standard_dev*1.5)
				{
					return 0;
				}
				// printf("Distance: %f \n", x[k]);
				if (camTestPoints==1)
				{
					//Shading to see where it is testing max distance
					 image[k*3+2] = 0;
					 image[k*3+1] = 255;
					 image[k*3]   = 0;
				}

			}
		}
	}
 //////////////////////////////////////////////////////////////////////////


	return 1;
}
void convert_to_distancemap (unsigned char *image, double *x, int width, int height,  double scalefactor){


	 for (int i = 0; i < height; i++)
	    {
	    	for (int j = 0; j < width; j++)
	    	{
	    		int k = (i * width + j)*3;
	    		 // printf("Point depth: %f Scale: %f\n", x[i * width + j],max_distance);
	    		// printf("%f  MAX %f\n", x[i * width + j], max_distance);
    			image[k+2] = (unsigned char)(x[i * width + j]/scalefactor * 255 );
	  			image[k+1] = (unsigned char)(x[i * width + j]/scalefactor  * 255);
	  			image[k]   = (unsigned char)(x[i * width + j]/scalefactor  * 255);



	    	}
	    }

}

void colour_focus_dir (unsigned char *image, int ii, int jj, int width, int height,int radius){
	/*
	Draws a circle on the point that is farthest away from the camera
	 */

	for (int i = ii-radius-1; i < ii+radius+1; i++)
	{
		for (int j = jj-radius-1; j < jj+radius+1; j++)
		{

			double dx = i - ii;
			double dy = j - jj;
			double sqrdist = dx*dx + dy*dy;
			if (sqrdist <= radius*radius){

				int k = (i*width + j);
				image[k*3+2] = 0;
				image[k*3+1] = 0;
				image[k*3]   = 255;
			}


		}
	}
}
void colour_close(unsigned char *image, double * x, int width, int height, double min_distance){
	/*
	Colours all things in the picture yellow that are closer than min_distance
	 */
	// TURN CLOSE STUFF YELLOW
	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < width; j++)
		{
			int k = (i *width+ j);
			if(x[i * width + j] < min_distance)
			{
				// Yellow
				image[k*3+2] = 255;
				image[k*3+1] = 255;
				image[k*3]   = 0;
			}
		}
	}
}

void choose_cam_zoom_point(unsigned char * image, double *x, int bounding_box_x, int bounding_box_y, double scalefactor, const RenderParams &render_params, int * ii, int * jj, double * max_distance){
	/*
	This function looks inside a specified region on the screen named camBoundBox.
	It searches for a group of points the size of holeSize and then remembers this point
	so that the camera can move in this direction in the future
	 */
	int height = render_params.height;
	int width = render_params.width;


	for (int i = bounding_box_y; i < height-bounding_box_y; i++)
	{
    	for (int j = bounding_box_x; j < width-bounding_box_x; j++)
    	{

     			int k = (i *width+ j);
     			if (render_params.camBoundBox == 1)
     			{
     				// Shadding to see the bounding box (where we search for max distance)
					image[k*3+2] = 255;
					image[k*3+1] = 0;
					image[k*3]   = 0;
     			}


    		if ( x[i * width + j] > *max_distance && holesize (image, x,i,j,scalefactor, width ,height,render_params.holeSize, render_params.camTestPoints)){

    			*max_distance = x[i * width + j];
    			*ii =i;
    			*jj =j;
    		}
    	}
	}

	if ((*ii+*jj) == 0) {
    		/*
    		If algortithm gets confused and can't find a new point just take
    		the centre of the screen
    		 */
    		*ii = width/2 + width%2;
    		*jj = height/2 + width%2;
    	}
}

void colour_camBoundBox(unsigned char * image, double * x, int bounding_box_x, int bounding_box_y, int height , int width){
	/*
	Displays all of the points where the camera is searching for a new
	max distance.
	 */
	for (int i = bounding_box_y; i < height-bounding_box_y; i++)
	{
		for (int j = bounding_box_x; j < width-bounding_box_x; j++)
		{
			int k = (i *width+ j);
			image[k*3+2] = 255;
			image[k*3+1] = 0;
			image[k*3]   = 0;

		}
	}
}