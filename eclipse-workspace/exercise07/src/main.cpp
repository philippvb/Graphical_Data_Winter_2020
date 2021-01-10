#include <iostream>
#include "fileio.h"
#include "vec.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>

// HINT:
// Build with "scons openmp=1" for performance.
// Placing the following block directly before for-loops whose iterations
// can run independently will distribute them to all available processor
// cores then. Don't nest loops preceeded by this block.
// -> Place this block in front of the most outer loop iterating over
// pixels (lines or columns) as each pixel can be calculated
// independently, but DON'T place it before the loop iterating over
// A-Trous Wavelet Transformation levels!

//#ifdef OPENMP
//#pragma omp parallel for
//#endif

using namespace std;

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

inline float sq(float v)
{
	return v * v;
}

/**
 * Evaluates the gauss function.
 * @param d Distance to the peak of the gauss function.
 * @param sigma Sigma.
 * @returns Gauss function with the given sigma evaluated at d.
 */
inline float gauss(const float d, const float sigma)
{
	return expf(-0.5f * sq(d / sigma));
}

/**
 * Filters an image with a gauss filter.
 * @param out Output parameter. Contains the filtered image.
 * @param in Image to process.
 * @param resX Width of the images in in/out in pixels.
 * @param resY Height of the images in in/out in pixels.
 * @param sigma Sigma of the gauss kernel to use.
 */
void GaussianFilter(Vec3* out, const Vec3* in, const int resX, const int resY,
		float sigma)
{
	// TODO 7.1 a) Implement a gaussian filter.
}

/**
 * Filters an image with a median filter.
 * @param out Output parameter. Contains the filtered image.
 * @param in Image to process.
 * @param resX Width of the images in in/out in pixels.
 * @param resY Height of the images in in/out in pixels.
 * @param widthHeight Width/Height of the box for which the median will be calculated.
 */
void MedianFilter(Vec3* out, const Vec3* in, const int resX, const int resY,
		const int widthHeight)
{
	// TODO 7.1 b) Implement a median filter.
	// You may want to utilize the MIN/MAX macros for looping over the filter area.
}

/**
 * Filters an image with a bilateral filter.
 * @param out Output parameter. Contains the filtered image.
 * @param in Image to process.
 * @param resX Width of the images in in/out in pixels.
 * @param resY Height of the images in in/out in pixels.
 * @param sigmaG Sigma of the gauss kernel for weighting by spatial distance.
 * @param sigmaB Sigma of the gauss kernel for weighting by intensity difference.
 */
void BilateralFilter(Vec3* out, const Vec3* in, const int resX, const int resY,
		const float sigmaG, const float sigmaB)
{
	// TODO 7.1 c) Implement a bilateral filter.
}

/**
 * Disassembles an image to several detail levels by using the a-trous transformation.
 * @param out Output parameter. Contains the resulting levels of the a-trous
 * transformation. out[n] contains the level n. Out corresponds to
 * {d_1, d_2, ..., d_(N-1), c_N}.
 * @param in Image to process.
 * @param resX Width of the images in in/out in pixels.
 * @param resY Height of the images in in/out in pixels.
 * @param n Number of levels to create.
 * @param sigmaG Sigma of the gauss kernel for weighting by spatial distance.
 * @param sigmaB Sigma of the gauss kernel for weighting by intensity difference.
 */
void aTrousTransformation(Vec3** out, const Vec3* in, const int resX,
		const int resY, const int n, const float sigmaG, const float sigmaB)
{
	// TODO 7.2 a) Implement the A-Trous Wavelet transform.
	// Ignore the sigmaB parameter for part a) of this exercise.

	// TODO 7.2 c) Include the edge-stopping function into your A-Trous Wavelet transform implementation.
	// This part uses sigmaB similar to the bilateral filter
	// -> sigmaB = inf should lead to the same result as before implementing 7.2 c)!
}

/**
 * Disassembles an image to several detail levels by using the a-trous transformation.
 * @param out Contains the reconstructed image.
 * @param in  Output parameter. Contains the a-trous wavelet levels.
 * in[n] contains the level n. In corresponds to {d_1, d_2, ..., d_(N-1), c_N}.
 * @param resX Width of the images in in/out in pixels.
 * @param resY Height of the images in in/out in pixels.
 * @param n Number of levels to create.
 * @param Coefficients for weighting the levels during reconstruction.
 * alpha[l] should weight the level in in[l].
 */
void inverseATrousTransformation(Vec3* out, Vec3** in, const int resX,
		const int resY, const int n, const float* alpha)
{
	// TODO 7.2 b) Implement the reconstruction from wavelet layers.
}

/**
 * Main function. Loads the image whose filename is given as parameter
 * and processes it with
 * <ul>
 *  <li>Gauss filtering,</li>
 *  <li>median filtering,</li>
 *  <li>bilateral filtering and</li>
 *  <li>a-trous transformation (forward + inverse).</li>
 * </ul>
 * @param argc Number of program parameters.
 * @param argv Array of program parameters.
 */
int main(int argc, char **argv)
{
	if (argc < 2)
	{
		cout << "Usage:   filter image.ppm";
		return 0;
	}

	// Read the original image
	float* fImage;
	int resX, resY;
	load_image_ppm(argv[1], fImage, resX, resY);

	// Prepare for filtering
	Vec3* image = (Vec3*) fImage;
	Vec3* filteredImage = new Vec3[resX * resY];

	// Apply gaussian filter and save.
	GaussianFilter(filteredImage, image, resX, resY, 5.0f);
	save_image_ppm("gaussFilteredImage.ppm", (float*) filteredImage, resX,
			resY);

	// Apply median filter and save.
	MedianFilter(filteredImage, image, resX, resY, 5);
	save_image_ppm("medianFilteredImage.ppm", (float*) filteredImage, resX,
			resY);

	// Apply bilateral filter and save.
	BilateralFilter(filteredImage, image, resX, resY, 5.0f, 0.1f);
	save_image_ppm("bilateralFilteredImage.ppm", (float*) filteredImage, resX,
			resY);

	// Prepare for a-trous transformation
	const int N = 5;

	float alpha[N] =
	{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };

	// TODO 7.2 b) Experiment with different values for alpha.
	// Generate exemplary images for submission.

	// TODO 7.2 d) Again experiment with different values for alpha and for sigmaB
	// (last parameter in aTrousTransformation(...)) in the edge-stopping function.
	// Generate exemplary images for submission.

	Vec3** aTrousLevels = new Vec3*[N];
	for (int n = 0; n < N; n++)
		aTrousLevels[n] = new Vec3[resX * resY];

	// Apply a-trous transformation and save.
	aTrousTransformation(aTrousLevels, image, resX, resY, N, 1.0f, 0.1f);
	for (int n = 0; n < N; n++)
	{
		char filename[128];
		sprintf(filename, "aTrousLevel%02d.ppm", n);
		save_image_ppm(filename, (float*) aTrousLevels[n], resX, resY);
	}

	// Apply inverse a-trous transformation and save.
	inverseATrousTransformation(filteredImage, aTrousLevels, resX, resY, N,
			alpha);
	for (int p = 0; p < resX * resY; p++)
		filteredImage[p].clamp();
	save_image_ppm("aTrousTransformedImage.ppm", (float*) filteredImage, resX,
			resY);

	// Cleanup
	for (int n = 0; n < N; n++)
		delete[] aTrousLevels[n];
	delete[] aTrousLevels;
	delete[] filteredImage;
	delete[] image;

	return 0;
}

