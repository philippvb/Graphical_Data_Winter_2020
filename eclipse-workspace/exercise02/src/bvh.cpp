#include "bvh.h"
#include <assert.h>
#include <iostream>
#include "utils/vec.h"
#include <list>
#define MAX_DEPTH 5
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

	// Pseudocode for logic:
	// check x > y:
	//     check x > z:
	//         x greatest
	//     else:
	//         z greatest
	// else:
	//     check y > z:
	//         y greatest
	//     else:
	//         z greatest

	float x_diff = abs(box.bounds[1].x-box.bounds[0].x);
	float y_diff = abs(box.bounds[1].y-box.bounds[0].y);
	float z_diff = abs(box.bounds[1].z-box.bounds[0].z);

//	std::cout << x_diff << " " << y_diff << " " <<  z_diff << std::endl;

	if (x_diff > y_diff) {
		if (x_diff > z_diff) {
			*position = (box.bounds[1].x+box.bounds[0].x)/2;
			*dimension=0;
		} else {
			*position = (box.bounds[1].z+box.bounds[0].z)/2;
			*dimension=2;
		}
	} else {
		if (y_diff > z_diff) {
			*position= (box.bounds[1].y+box.bounds[0].y)/2;
			*dimension=1;
		} else {
			*position = (box.bounds[1].z+box.bounds[0].z)/2;
			*dimension=2;
		}
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


	std::cout << "nodeIndex: " << nodeIndex << "; triIndex: " << triIndex << "; numTris: " << numTris << "; depth: " << depth << std::endl;
//	Triangle* node_tris=new Triangle[numTris];
//
//	// This should be equal
//	for(int index = 0; index < numTris; index ++){
//		node_tris[index] = tris[indices[triIndex + index]];
//	}

	// But actually we can just use the array from the correct location, because it is ordered
	const Triangle* node_tris = &tris[triIndex];

	//check if leaf node, if so add values
	if ((numTris<=1) || (depth>=MAX_DEPTH)){
		nodes[nodeIndex].bbox=Triangle::getAABB(node_tris, numTris);
		nodes[nodeIndex].left=-1;
		nodes[nodeIndex].right=-1;
		nodes[nodeIndex].triIndex=triIndex;
		nodes[nodeIndex].numTris=numTris;
		return;
		std::cout << "got 1 leaf" << std::endl;
	}



	nodes[nodeIndex].bbox = Triangle::getAABB(node_tris, numTris);
	unsigned int dimension;
	float position;
	findSplitPlane(&dimension, &position, nodes[nodeIndex].bbox);

	int left_upper=0;
	int right_lower=numTris-1;

	// Sort according to tris' center's location relative to the split plane, so we don't care about the tris being both, on the left and right side
	bool left_side;
	bool right_side;
	Vec3 center;
	for (int index = triIndex; index <= triIndex + right_lower; index++) {
		left_side = false;
		right_side = false;

//		// check for every tris in node on which side of split it is
//		for (int edge = 0; edge < 3; edge++) {
//			if (dimension == 0) {
//				if (tris[indices[index]].v[edge].x <= position) {
//					left_side = true;
//				}else{
//					right_side = true;
//				}
//			} else if (dimension == 1) {
//				if (tris[indices[index]].v[edge].y <= position) {
////					std::cout << "set_left_side_true" << std::endl;
//					left_side = true;
//				}else{
//					right_side = true;
//				}
//			} else {
//				if (tris[indices[index]].v[edge].z <= position) {
//					std::cout << "on_left_side" << std::endl;
//					left_side = true;
//				}else{
////					std::cout << "on_right_side" << std::endl;
//					right_side = true;
//				}
//			}
//		}


		// Just use center of BBox for now
		center = (tris[indices[index]].v[0] + tris[indices[index]].v[1] + tris[indices[index]].v[2]) / 3;
		if (dimension == 0) {
			if (center.x <= position) {
				left_side = true;
			}else{
				right_side = true;
			}
		} else if (dimension == 1) {
			if (center.y <= position) {
				left_side = true;
			}else{
				right_side = true;
			}
		} else {
			if (center.z <= position) {
				left_side = true;
			}else{
				right_side = true;
			}
		}


		// rearrange the indices array such that the childs of the current node are sorted by [left, right]
		// if it is to the left side, we keep it at its position, if it should go to the right side, we put it to the current end
		if (left_side) {
			left_upper++;
		}
		else{
			swap_indices(indices[index], indices[triIndex + right_lower]);
			// But now we have to still look at the current index, because we swapped a new item here
			index--;
			right_lower--;
		}
	}

	// build new left and right child node
	nodes[nodeIndex].left=addedNodes;
	addedNodes++;
	buildBVH(addedNodes-1, triIndex, left_upper, depth + 1 );
	nodes[nodeIndex].right=addedNodes;
	addedNodes++;
	buildBVH(addedNodes-1, triIndex+left_upper, numTris-left_upper, depth + 1);



	// current implementation just adds one leaf and exits
//	nodes[nodeIndex].bbox = Triangle::getAABB(tris, numTris);
//	nodes[nodeIndex].left = -1;
//	nodes[nodeIndex].right = -1;
//	nodes[nodeIndex].triIndex = 0;
//	nodes[nodeIndex].numTris = numTris;
}


void BVH::swap_indices(int &ind_a, int &ind_b){
	int save = ind_a;
	ind_a = ind_b;
	ind_b = save;
}



//void BVH::intersect_recursive(const Ray ray, const Vec3 invRayDir,  const unsigned int raySign, int node_index, HitRec &rec){
//
//	// is leaf  node, then intersect all tris, save as hit record
//	if (nodes[node_index].left ==-1 && nodes[node_index].right == -1){
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


void BVH::intersect_recursive(const Ray ray, const int node_index, HitRec &rec) const {
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

	if (nodes[node_index].left ==-1 && nodes[node_index].right == -1){
		for(int i = nodes[node_index].triIndex; i < nodes[node_index].triIndex+nodes[node_index].numTris; i++){
			tris[indices[i]].intersect(ray, rec, i);
		}
	}

	float dummy_interval_min = -FLT_MAX;
	float dummy_interval_max = FLT_MAX;

	if (nodes[node_index].bbox.intersect(ray,dummy_interval_min, dummy_interval_max, invRayDir, raySign)){
		intersect_recursive(ray, nodes[node_index].left, rec);
		intersect_recursive(ray, nodes[node_index].right, rec);
	}



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

	intersect_recursive(ray, 0, rec);



//	if (nodes[0].triIndex != -1)
//	{
//		for (int i = nodes[0].triIndex;
//				i < nodes[0].triIndex + nodes[0].numTris; i++)
//		{
//			tris[indices[i]].intersect(ray, rec, i);
//		}
//	}



	return rec;
}


// This returns the total number of tris in the BVH, it should be equal to the initial number
int BVH::sumNodeTris(){
	int sum = 0;
	for(int i=0; i < addedNodes; i++){
		if (nodes[i].left == -1 && nodes[i].right == -1){
			sum = sum + nodes[i].numTris;
		}
	}
	return sum;
}
