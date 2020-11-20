#include "bvh.h"
#include <assert.h>
#include <iostream>

using namespace std;

BVH::BVH(const Triangle * const tris, const int nTris) :
		tris(tris), nTris(nTris)
{
	nodes = new Node[nTris * 2]; // a bvh has at most 2 * n - 1 nodes

	indices = new int[nTris];
	for (int i = 0; i < nTris; i++)
		indices[i] = i;

	addedNodes = 1; // the root node
	buildBVH(0, 0, nTris, 0); // recursive construct BVH
}

BVH::~BVH()
{
}

void BVH::findSplitPlane(unsigned int* dimension, float* position, const AABB &box)
{
	// TODO b) Implement the method.
}

void BVH::buildBVH(int nodeIndex, int triIndex, int numTris, int depth)
{
	// TODO c) Implement the method by replacing the code below.
	// Rough Steps:
	// - Find the split plane using the method from b)
	// - Sort all triangles belonging to the currently processed node depending on their center's location relative to the split plane.
	// - Recursively call this method to process subnodes.
	// Don't forget about the base case (no split needed)!

	// current implementation just adds one leaf and exits
	nodes[nodeIndex].bbox = Triangle::getAABB(tris, numTris);
	nodes[nodeIndex].left = -1;
	nodes[nodeIndex].right = -1;
	nodes[nodeIndex].triIndex = 0;
	nodes[nodeIndex].numTris = numTris;
}

HitRec BVH::intersect(const Ray &ray) const
{
	// precompute inverse ray direction for bounding box intersection
	// -> compute only once for bvh travesal
	Vec3 invRayDir(1.0f / ray.dir.x, 1.0f / ray.dir.y, 1.0f / ray.dir.z);

	// index array for ray signs (direction of the ray)
	// -> saving costly if-operations during box intersection and traversal
	unsigned int raySign[3][2];
	raySign[0][0] = invRayDir[0] < 0;
	raySign[1][0] = invRayDir[1] < 0;
	raySign[2][0] = invRayDir[2] < 0;

	raySign[0][1] = invRayDir[0] >= 0;
	raySign[1][1] = invRayDir[1] >= 0;
	raySign[2][1] = invRayDir[2] >= 0;

	HitRec rec;

	// TODO d) instead of intersecting all triangles, traverse the bvh
	// and intersect only the triangles in the leave nodes.
	// Attention: Make sure that hit records and tmin/tmax values are not modified by node intersections!

	// currently there is only one leaf ...
	if (nodes[0].triIndex != -1)
	{
		for (int i = nodes[0].triIndex;
				i < nodes[0].triIndex + nodes[0].numTris; i++)
		{
			tris[indices[i]].intersect(ray, rec, i);
		}
	}

	return rec;
}

