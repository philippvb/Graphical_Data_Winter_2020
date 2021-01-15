/*
 * main.cpp
 *
 *  Created on: Dec 5, 2011
 *      Author: Benjamin Resch
 */

#include <math.h>
#include <stdio.h>
#include <iostream>

#ifdef OPENMP
#include <omp.h>
#endif

#include "utils/fileio.h"
#include "utils/vec.h"
#include "utils/MersenneTwister.h"

using namespace std;

/**
 * Evaluates the pixel brightness for a given function and position.
 * @param function Function to evaluate (compare to assignment sheet).
 * @param x Horizontal position. Is assumed to be in [0, 1).
 * @param y Vertical position. Is assumed to be in [0, 1).
 * @returns Function value in [0, 1].
 */
float evaluate(unsigned int function, float x, float y)
{
	// TODO 8.2 a) Evaluate the given test functions.
	// Watch the function documentation above for details.

	if(function == 1){
		return 0.5 * (1 + pow((1 - y), 3) * sin(2 * M_PI * x * exp(10 * x)));
	}else if (function == 2){
		return 0.5 * (1 + sin(1600 * (pow(x, 2) + pow(y, 2))));
	}else if (function == 3){
		return 0.5 * (1 + sin(60 * 4 * M_PI * atan(x / y)));
	}else{
		return 0.0f;
	}

}

/**
 * Main program function. Iterates over different sample counts and
 * the different functions and creates and saves an image for each
 * combination.
 * @return Return code. 0 if everything is fine.
 */
int main()
{
	MTRand **mtrand;
#ifndef OPENMP
	mtrand = new MTRand*[1];
	mtrand[0] = new MTRand(1337);
#else
	mtrand = new MTRand*[omp_get_max_threads()];
	for (int t = 0; t < omp_get_max_threads(); t++)
		mtrand[t] = new MTRand(1337 + t);
#endif

	const unsigned int widthHeight = 512;

	char filename[128];
	Vec3* image = new Vec3[widthHeight * widthHeight];

	for (unsigned int m = 1; m <= 16; m *= 2)
	{
		unsigned int samples = m * m;
		cout << samples << (samples == 1 ? " Sample..." : " Samples...")
				<< endl;

		for (unsigned int function = 1; function <= 3; function++)
		{
			cout << "  Function " << function << "..." << endl;

			// Regular Sampling
			// ========================================
			cout << "    Regular..." << endl;
#ifdef OPENMP
#pragma omp parallel for
#endif
			for (unsigned int j = 0; j < widthHeight; j++)
			{
				for (unsigned int i = 0; i < widthHeight; i++)
				{
					// TODO 8.2 b) Evaluate the color of pixel (i, j) by using m * m regular samples.

					Vec3 ret = Vec3(0.0f);
					for(int s_i = 0; s_i < m; s_i++){
						for (int s_j = 0; s_j < m; s_j++){
							ret += evaluate(function, (s_i + 0.5) / m + i, (s_j + 0.5) / m + j);
						}
					}
					ret = ret / (m * m);
					image[j * widthHeight + i] = ret;
				}
			}
			sprintf(filename, "function%d_%03dsamples_a-regular.ppm", function,
					samples);
			save_image_ppm(filename, (float*) image, widthHeight, widthHeight);
			// ========================================

			// Random Sampling
			// ========================================
			cout << "    Random..." << endl;
#ifdef OPENMP
#pragma omp parallel for
#endif
			for (unsigned int j = 0; j < widthHeight; j++)
			{
#ifndef OPENMP
				int thread = 0;
#else
				int thread = omp_get_thread_num();
#endif

				for (unsigned int i = 0; i < widthHeight; i++)
				{
					// TODO 8.2 b) Evaluate the color of pixel (i, j) by using m * m random samples.
					// Use mtrand[thread]->rand() to generate random numbers in [0, 1).

					Vec3 ret = Vec3(0.0f);
					for(int s_i = 0; s_i < m*m; s_i++){
						ret += evaluate(function, mtrand[thread]->rand() + i, mtrand[thread]->rand() + j);
					}
					ret = ret / (m*m);
					image[j * widthHeight + i] = ret;
				}
			}
			sprintf(filename, "function%d_%03dsamples_b-random.ppm", function,
					samples);
			save_image_ppm(filename, (float*) image, widthHeight, widthHeight);
			// ========================================

			// Stratified Sampling
			// ========================================
			cout << "    Stratified..." << endl;
#ifdef OPENMP
#pragma omp parallel for
#endif
			for (unsigned int j = 0; j < widthHeight; j++)
			{
#ifndef OPENMP
				int thread = 0;
#else
				int thread = omp_get_thread_num();
#endif

				for (unsigned int i = 0; i < widthHeight; i++)
				{
					// TODO 8.2 b) Evaluate the color of pixel (i, j) by using m * m stratified samples.
					// Use mtrand[thread]->rand() to generate random numbers in [0, 1).

					Vec3 ret = Vec3(0.0f);
					for(int s_i = 0; s_i < m; s_i++){
						for (int s_j = 0; s_j < m; s_j++){
							ret += evaluate(function, (s_i + mtrand[thread]->rand()) / m + i, (s_j + mtrand[thread]->rand()) / m + j);
						}
					}
					ret = ret / (m*m);
					image[j * widthHeight + i] = ret;
				}
			}
			sprintf(filename, "function%d_%03dsamples_c-stratified.ppm",
					function, samples);
			save_image_ppm(filename, (float*) image, widthHeight, widthHeight);
			// ========================================
		}
	}

	delete[] image;

#ifndef OPENMP
	delete mtrand[0];
#else
	for (int t = 0; t < omp_get_max_threads(); t++)
		delete mtrand[t];
#endif
	delete[] mtrand;

	cout << "Done." << endl;

	return 0;
}
