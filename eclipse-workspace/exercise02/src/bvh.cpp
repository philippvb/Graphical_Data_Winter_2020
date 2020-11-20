#include "bvh.h"
#include <assert.h>
#include <iostream>
#include "utils/vec.h"
#include <list>
#define MAX_DEPTH 10
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
	if(abs(box.bounds[1].x-box.bounds[0].x) > abs(box.bounds[1].y-box.bounds[0].y)){
		if(abs(box.bounds[1].x-box.bounds[0].x) > abs(box.bounds[1].z-box.bounds[0].z)){
			*position = box.bounds[1].x + (box.bounds[1].x-box.bounds[0].x)/2;
			*dimension=0;
		}
		else if (abs(box.bounds[1].y-box.bounds[0].y) > abs(box.bounds[1].z-box.bounds[0].z)){
			*position= box.bounds[1].y + (box.bounds[1].y-box.bounds[0].y)/2;
			*dimension=1;
		}
	}
	else{
		*position= box.bounds[1].z + (box.bounds[1].z-box.bounds[0].z)/2;
		*dimension=2;
	}

}

void BVH::buildBVH(int nodeIndex, int triIndex, int numTris, int depth)
{
	// TODO c) Implement the method by replacing the code below.
	// Rough Steps:
	// - Find the split plane using the method from b)
	// - Sort all triangles belonging to the currently processed node depending on their center's location relative to the split plane.
	// - Recursively call this method to process subnodes.
	// Don't forget about the base case (no split needed)!#
	// build bounding box
	Triangle* node_tris=new Triangle[numTris];
	for(int index=triIndex; index < triIndex + numTris; index++){
		node_tris[index - triIndex]=tris[indices[index]];
	}

	//check if leaf node, if so add values
	if ((numTris<=1)|| (depth>MAX_DEPTH)){
		nodes[nodeIndex].bbox=Triangle::getAABB(node_tris, numTris);
		nodes[nodeIndex].left=-1;
		nodes[nodeIndex].right=-1;
		nodes[nodeIndex].triIndex=triIndex;
		nodes[nodeIndex].numTris=numTris;
		return;
	}



	nodes[nodeIndex].bbox = Triangle::getAABB(node_tris, numTris);
	unsigned int dimension;
	float position;
	findSplitPlane(&dimension, &position, nodes[nodeIndex].bbox);

	int left_upper=0;
	int right_lower=numTris-1;


	for (int index = triIndex; index < triIndex + numTris; index++) {
		// check for every tris in node on which side of split it is
		bool left_side = false;
		bool right_side = false;
		for (int edge = 0; edge < 3; edge++) {
			if (position == 0) {
				if (tris[indices[index]].v[edge].x <= position) {
					left_side = true;
				}
				else{
					right_side = true;
				}
			} else if (position == 1) {
				if (tris[indices[index]].v[edge].y <= position) {
					left_side = true;
				}
				else{
					right_side = true;
								}
			} else {
				if (tris[indices[index]].v[edge].z <= position) {
					left_side = true;
				}
				else{
					right_side = true;
								}
			}
		}
		// rearrange the indices array such that the childs of the current node are sorted by [left, right]
		if (left_side) {
			int to_swap = indices[triIndex + left_upper];
			indices[triIndex + left_upper] = indices[index];
			indices[index]=to_swap;
			left_upper++;
		}
		if (right_side){
			int to_swap = indices[triIndex + right_lower];
			indices[triIndex + right_lower] = indices[index];
			indices[index]=to_swap;
			right_lower--;
		}
	}

	// add left and right child


	// build new left and right child node
	nodes[nodeIndex].left=addedNodes;
	addedNodes++;
	buildBVH(addedNodes-1, triIndex, left_upper, depth++);
	nodes[nodeIndex].right=addedNodes;
	addedNodes++;
	buildBVH(addedNodes-1, triIndex+right_lower, numTris-1-right_lower, depth++);

	//Triangle left_tris[numTris];
	//Triangle right_tris[numTris];


	// current implementation just adds one leaf and exits
//	nodes[nodeIndex].bbox = Triangle::getAABB(tris, numTris);
//	nodes[nodeIndex].left = -1;
//	nodes[nodeIndex].right = -1;
//	nodes[nodeIndex].triIndex = 0;
//	nodes[nodeIndex].numTris = numTris;
}

//void BVH::intersect_recursive(const Ray &ray, const Vec3 &invRayDir,  const unsigned int raySign[3][2], int node_index, HitRec &rec) const{
//
//	// is leaf  node, then intersect all tris, save as hit record
//	if (nodes[node_index].left ==-1){
//		for(int i = nodes[node_index].triIndex; i < nodes[node_index].triIndex+nodes[node_index].numTris; i++){
//			tris[indices[i]].intersect(ray, rec, i);
//		}
//	}
//
//	float dummy_interval_min;
//	float dummy_interval_max;
//
//	if (nodes[node_index].bbox.intersect(ray,dummy_interval_min, dummy_interval_max,invRayDir, raySign)){
//		intersect_recursive(ray, invRayDir, raySign, nodes[node_index].left, rec);
//		intersect_recursive(ray, invRayDir, raySign, nodes[node_index].right, rec);
//	}
//}


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

	//intersect_recursive(ray, invRayDir, raySign, 0, rec);

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

