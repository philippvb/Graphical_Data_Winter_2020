#ifndef RTSTRUCTS_H
#define RTSTRUCTS_H

#include "utils/vec.h"

#define RAY_MAX FLT_MAX
#define RAY_EPS 0.0001f

inline float minf(const float a, const float b)
{
	return a < b ? a : b;
}
inline float maxf(const float a, const float b)
{
	return a > b ? a : b;
}

/**
 * Ray containing an intersection interval.
 */
struct Ray
{ // 32 Byte
/// Origin of the ray.
	Vec3 origin;
	/// Direction of the ray
	Vec3 dir;
	/// Minimum distance for intersection (inclusive).
	float tmin;
	/// Maximum distance for intersection (inclusive).
	float tmax;

	/**
	 * Standard constructor which does not initialize the ray.
	 */
	inline Ray()
	{
	}

	/**
	 * Initializes the ray.
	 * @param origin See origin.
	 * @param dir See dir.
	 * @param tmin See tmin.
	 * @param tmax See tmax.
	 */
	inline Ray(const Vec3 &origin, const Vec3 &dir, const float tmin,
			const float tmax) :
			origin(origin), dir(dir), tmin(tmin), tmax(tmax)
	{
	}
};

/**
 * Stores information about a hit of a ray against a surface.
 */
struct HitRec
{
	/// Distance from the ray origin to the intersection point.
	float dist;
	/// Id of the hit surface (e.g. triangle).
	int id;

	/// Initializes the record as not hitting anything.
	inline HitRec()
	{
		dist = RAY_MAX;
		id = -1;
	}

	/** Initializes the record with given values.
	 * @param d See dist.
	 * @param i See id.
	 */
	inline HitRec(float d, int i)
	{
		dist = d;
		id = i;
	}
};

/**
 * Axis aligned bounding box.
 */
struct AABB
{
	/// Corners of the bounding box with the minimum and the maximum elements.
	Vec3 bounds[2];

	/**
	 * Initializes the bounding box with -max volume. This way, it can be
	 * extended by vertices or other AABBs easily.
	 */
	inline AABB()
	{
		bounds[0] = Vec3(FLT_MAX);
		bounds[1] = Vec3(-FLT_MAX);
	}

	/**
	 * Initializes the bounding box with given values.
	 * @param bmin Initializes bounds[0]
	 * @param bmax Initializes bounds[1]
	 */
	inline AABB(Vec3 bmin, Vec3 bmax)
	{
		bounds[0] = bmin;
		bounds[1] = bmax;
	}

	/**
	 * Intersects a ray with the bounding box.
	 * @param r Ray to intersect.
	 * @param intervalMin In/out parameter: Contains the minimum of the
	 * interval to check before the call and the minimum of the interval
	 * in which the ray is inside the check-interval and inside the AABB
	 * after the call.
	 * @param intervalMax In/out parameter: Contains the minimum of the
	 * interval to check before the call and the minimum of the interval
	 * in which the ray is inside the check-interval and inside the AABB
	 * after the call.
	 * @returns True if the ray intersects with the AABB in the given
	 * interval.
	 */
	inline bool intersect(const Ray &r, float &intervalMin,
			float &intervalMax) const
	{
		// TODO b) Ray Tracing

		return false;
	}
};

/**
 * Triangle.
 */
struct Triangle
{
	/// Vertices of the triangle (unordered).
	Vec3 v[3];

	/**
	 * Returns the AABB of the triangle.
	 * @returns AABB containing all vertices of the triangle.
	 */
	inline AABB getAABB() const
	{
		AABB bbox;

		// TODO a) Bounding Box Computation

		return bbox;
	}

	/**
	 * Returns the AABB of a set of triangles.
	 * @param tris Array containing all triangles which must be inside the
	 * AABB.
	 * @param nTris Number of triangles in tris.
	 * @returns AAB containing all vertices of all triangles.
	 */
	static AABB getAABB(const Triangle * const tris, const unsigned int nTris)
	{
		AABB bbox;

		// TODO a) Bounding Box Computation
		// Replace the following ground truth:
		// ====================================================================
		bbox.bounds[0] = Vec3(-177.535, -175.185, -509.155);
		bbox.bounds[1] = Vec3(179.165, 184.861, -88.5957);
		// ====================================================================

		return bbox;
	}

	/**
	 * Intersects a ray with the triangle.
	 * @param ray Ray to intersect with the triangle.
	 * @param rec Hit record containing the id of the intersected object and
	 * the distance to the hit. Should only be updated if the intersection
	 * point of this triangle is closer than the intersection that was stored
	 * in the hit record before.
	 * @param tri_id Triangle id of this triangle.
	 * @param True if the ray intersects with the triangle in the interval
	 * set in the ray.
	 */
	inline bool intersect(const Ray &ray, HitRec &rec, const int tri_id) const
	{
		// TODO b) Ray Tracing
		// Replace the following dummy code!
		// ============================================================
		// Just check if the ray goes somewhere close to a vertex:
		// Iterate over all vertices
		for (unsigned int i = 0; i < 3; i++)
		{
			Vec3 dirToVertex = v[i] - ray.origin;
			float distToVertex = dirToVertex.length();
			dirToVertex.normalize();
			if (dirToVertex * ray.dir > 0.9999f) // Vertex approximately in ray direction?
			{
				if (distToVertex < rec.dist) // Distance shorter than previous?
				{
					rec.id = tri_id;
					rec.dist = distToVertex;
					return true;
				}
			}
		}
		return false;
		// ============================================================
	}

	/**
	 * Intersects a ray with the triangle.
	 * @param ray Ray to intersect with the triangle.
	 * @returns True if the ray intersects with the triangle.
	 */
	inline bool intersectShadow(const Ray &ray) const
	{
		// same as above, but without saving the distance and ID
		return false;
	}

	/**
	 * Returns the normal of the triangle.
	 * @remarks It is undefined if the normal points to the front or the back
	 * side of the triangle because the vertex order is undefined.
	 */
	inline Vec3 getNormal() const
	{
		// TODO d) Diffuse Shading: Return the normal of the triangle.

		return Vec3(0.0f, 0.0f, 1.0f);
	}

};

#endif

