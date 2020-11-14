#ifndef CAM_H
#define CAM_H

#include "utils/vec.h"
#include "rtStructs.h"

/**
 * Orthographic camera watching a AABB into z direction.
 */
struct Cam
{
	/// Volume watched by the camera.
	AABB box;
	/// Resolution of the image plane in horizontal direction.
	int ResX;
	/// Resolution of the image plane in vertical direction.
	int ResY;
	/// Extent of the observed volume.
	Vec3 extent;
	/// Viewing direction of the camera (always (0,0,1)).
	Vec3 dir;

	/// Contains the step size per pixel in horizontal direction.
	float xstep;
	/// Contains the step size per pixel in vertical direction.
	float ystep;

	/**
	 * Initializes the camera.
	 * @param box See box.
	 * @param ResX See ResX.
	 * @param ResY See ResY.
	 */
	Cam(const AABB &box, const int ResX, const int ResY) :
			box(box), ResX(ResX), ResY(ResY)
	{
		extent = box.bounds[1] - box.bounds[0];

		xstep = extent[0] / (float) ResX;
		ystep = extent[1] / (float) ResY;

		dir = Vec3(0.0f, 0.0f, 1.0f);
	}

	/**
	 * Returns a gamera ray given pixel coordinates on the image plane.
	 * @param x X coordinate on the image plane in [0, ResX-1].
	 * @param y Y coordinate on the image plane in [0, ResY-1].
	 * @returns Ray according to the given pixel coordinates.
	 */
	Ray getRay(const float x, const float y) const;
};

inline Ray Cam::getRay(const float x, const float y) const
{
	Vec3 eye = box.bounds[0];
	eye[0] += x * xstep;
	eye[1] += y * ystep;
	return Ray(eye, dir, 0.0f, RAY_MAX);
}

// TODO c) Primary Ray Generation: Implement another camera model here!





/**
 * Perspective Camera Model
 */

struct MyCam
{

	// camera origin
	Vec3 o;
	/// Viewing direction of the camera (always (0,0,1)).
	Vec3 dir;
	// up vector
	Vec3 u;
	// focal length
	int focal_length;
	/// Resolution of the image plane in horizontal direction.
	int ResX;
	/// Resolution of the image plane in vertical direction.
	int ResY;



//	/// Volume watched by the camera.
//	AABB box;
//	/// Extent of the observed volume.
//	Vec3 extent;

//	/// Contains the step size per pixel in horizontal direction.
//	float xstep;
//	/// Contains the step size per pixel in vertical direction.
//	float ystep;

	/**
	 * Initializes the camera.
	 * @param box See box.
	 * @param ResX See ResX.
	 * @param ResY See ResY.
	 */

	MyCam(const Vec3 &o, const Vec3 &dir, const Vec3 u, const int focal_length, const int ResX,
			const int ResY) :
				o(o), dir(dir), u(u), focal_length(focal_length), ResX(ResX), ResY(ResY)
	{
//		dir = Vec3(0.0f, 0.0f, 1.0f);

	}

//	Cam(const AABB &box, const int ResX, const int ResY) :
//			box(box), ResX(ResX), ResY(ResY)
//	{
//		extent = box.bounds[1] - box.bounds[0];
//
//		xstep = extent[0] / (float) ResX;
//		ystep = extent[1] / (float) ResY;
//
//	}

	/**
	 * Returns a gamera ray given pixel coordinates on the image plane.
	 * @param x X coordinate on the image plane in [0, ResX-1].
	 * @param y Y coordinate on the image plane in [0, ResY-1].
	 * @returns Ray according to the given pixel coordinates.
	 */
	Ray getRay(const float x, const float y) const;
};

inline Ray MyCam::getRay(const float x, const float y) const
{
	Vec3 ray_dir = focal_length * dir + (x - (ResX / 2)) * Vec3::cross(dir, u) + (y - (ResY / 2)) * u;
	ray_dir.normalize();
	Ray ray = Ray(o, ray_dir, 0.0f, RAY_MAX);

	return ray;
}





#endif

