/*
 * main.cpp
 *
 *  Created on: Dec 5, 2011
 *      Author: Benjamin Resch
 */

#include <fstream>
#include <map>
#include <assert.h>

#include "utils/fileio.h"
#include "utils/vec.h"


using namespace std;

map<int, Vec3> loadCMF()
{
	map<int, Vec3> result;


	FILE * file = fopen("lin2012xyz2e_5_7sf.csv", "r");


	int wavelength;
	float r, g, b;

	while(fscanf(file, "%i,%f,%f,%f\n", &wavelength, &r, &g, &b) == 4){
		result[wavelength] = Vec3(r,g,b);
	}

	fclose(file);


	// TODO 8.1 a) Load the CMF data or hardcode it here.
	// Feel free to modify the return type and parameters if you need.

	return result;
}

int main(int argc, char** argv)
{
//	Vec3* result = new Vec3[512 * 256];
//	for (unsigned int p = 0; p < 512 * 256; p++)
//		result[p] = Vec3(0.0f, 0.0f, 0.0f);

	map<int, Vec3> cmf = loadCMF();


	// TODO 8.1 b) Load all env_roof_l???.pfm images and add the response that is caused by them to the result image.

	int ResX = 512;
	int ResY = 256;

	float * resultImage = new float[ResX * ResY * 3];

	for(int i = 400; i <= 720; i = i + 10){
		char filename[100];
		sprintf(filename, "spectral_roof/env_roof_l%i.pfm", i);

		float * image;

		load_image_pfm(filename, image, ResX, ResY);

		for(int idx = 0; idx < ResX * ResY; idx++){
			resultImage[idx * 3 + 0] = image[idx * 3 + 0] * cmf[i].x;
			resultImage[idx * 3 + 1] = image[idx * 3 + 1] * cmf[i].y;
			resultImage[idx * 3 + 2] = image[idx * 3 + 2] * cmf[i].z;
		}
	}
	save_image_pfm("output_image.pfm", resultImage, ResX, ResY);

	// TODO 8.1 c) Save the resulting image.

	delete[] resultImage;

//	delete[] result;
}
