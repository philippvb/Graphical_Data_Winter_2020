/**
 * Creator: Manuel Finckh
 * Email:   manuel.finckh@uni-ulm.de
 */

#include <iostream>
#include <fstream>
#include <omp.h>
#include "cam.h"
#include "rtStructs.h"
#include "bvh.h"
#include "utils/vec.h"
#include "utils/fileio.h"
#include "utils/MersenneTwister.h"

Cam *cam;

#ifdef INTERACTIVE
#include <SDL/SDL.h>
#include <SDL_opengl.h>

bool finished = false;

SDL_Surface *screen;
void initScreen(int ResX, int ResY)
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_SetVideoMode(ResX, ResY, 32, SDL_OPENGL | SDL_GL_DOUBLEBUFFER);
}

void eventHandling()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				finished = true;
				break;
			case SDLK_q:
				finished = true;
				break;
			case SDLK_w:
				cam->cam_w(true);
				break;
			case SDLK_a:
				cam->cam_a(true);
				break;
			case SDLK_s:
				cam->cam_s(true);
				break;
			case SDLK_d:
				cam->cam_d(true);
				break;
			default:
				break;
			}
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym)
			{
			case SDLK_w:
				cam->cam_w(false);
				break;
			case SDLK_a:
				cam->cam_a(false);
				break;
			case SDLK_s:
				cam->cam_s(false);
				break;
			case SDLK_d:
				cam->cam_d(false);
				break;
			default:
				break;
			}
			break;
		case SDL_MOUSEMOTION:
			if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(1))
			{
				cam->cam_rx(event.motion.yrel);
				cam->cam_ry(event.motion.xrel);
				break;
			}
			break;
		case SDL_QUIT:
			finished = true;
			break;
		default:
			break;
		}
	}
}

#endif

/// Random number generator.
MTRand *drand;

/// Width of the rendered image.
int ResX = 300;
/// Height of the rendered image.
int ResY = 300;

/**
 * Buffer containing the rendered images.
 * @remarks Pixels are stored rowwise, so pixel positions can be calculated by
 * y*ResX+x.
 */
Vec3 *buffer;
/// BVH tree used for accelerating the rendering.
BVH *bvh;

/// For pointlights to light the scene
Pointlight pointlights[4];
/// Area light used to light the scene
AreaLight areaLight;
/// Index of the first triangle that should be rendered as mirroring instead of diffuse.
int firstMirrorTriangle;

// Uncomment exactly one of the following lines!
//#define FLAT_SHADER
//#define DEBUG_SHADER
//#define SMOOTH_SHADER
//#define POINTLIGHT_SHADER
#define AREALIGHT_SHADER

#define MAX_MIRROR_HITS 10
#define TOTAL_SHADOW_RAYS 1000
// Computes light intensity between source and intersection point of Tri, set to 0 if Tri is occluded
Vec3 compute_light_intensity( Ray ray, HitRec rec, bool use_point_light){
	Vec3 normal = bvh->tris[rec.id].getInterpolatedNormal(ray);
	Vec3 hit_point=ray.origin + ray.dir * rec.dist;


	// keep reflecting if mirror surface
	if ((rec.id >= firstMirrorTriangle)&&(use_point_light)){
		Ray mirror_ray=Ray(hit_point, -((2 * (normal * ray.dir)) * normal - ray.dir), RAY_EPS, RAY_MAX);
		HitRec new_rec=bvh->intersect(mirror_ray);
		Vec3 total_intensity=Vec3(0,0,0);
		total_intensity=compute_light_intensity(mirror_ray, new_rec, use_point_light);
		return total_intensity;
	}

	if (use_point_light){
		Vec3 total_intensity=Vec3(0,0,0);
		for(int i=0; i<=3;i++){
			Pointlight pointlight=pointlights[i];
			Vec3 dir=pointlight.pos-hit_point;
			float distance=dir.length();
			dir.normalize();
			Ray shadow_ray=Ray(hit_point, dir, RAY_EPS, distance);
			HitRec shadow_rec = bvh->intersect(shadow_ray);
			if(shadow_rec.id==-1){
				total_intensity+=fabsf(dir * normal)/(distance*distance)*pointlight.color;
			}
		}
		return total_intensity;
	}
	else{
		Vec3 total_intensity=Vec3(0,0,0);
		for(int i = 1; i<=TOTAL_SHADOW_RAYS; i++){
			Pointlight pointlight;
			pointlight.pos=areaLight.pos + areaLight.extent1*drand->rand() + areaLight.extent2*drand->rand();
			pointlight.color=areaLight.getArea()*areaLight.radiance;
			Vec3 dir=pointlight.pos-hit_point;
			float distance=dir.length();
			dir.normalize();
			Ray shadow_ray=Ray(hit_point, dir, RAY_EPS, distance);
			HitRec shadow_rec = bvh->intersect(shadow_ray);
			if(shadow_rec.id==-1){
				total_intensity+=fabsf(dir * normal)/(distance*distance)*pointlight.color;
			}
		}
		return total_intensity/TOTAL_SHADOW_RAYS;

	}
};


/**
 * Determines the color seen along a certain ray.
 * @param ray This ray is used for RayTracing.
 * @returns Color seen along the given ray.
 */
Vec3 rayTrace(const Ray &ray)
{
	HitRec rec = bvh->intersect(ray);


	if (rec.id != -1)
	{
#ifdef FLAT_SHADER
		Vec3 normal = bvh->tris[rec.id].getNormal();
		return fabsf(ray.dir * normal);
#endif

#ifdef DEBUG_SHADER
		// TODO [DONE] 3.3 d) Implement a debug shader here.
		Vec3 normal = bvh->tris[rec.id].getInterpolatedNormal(ray);

		// Uncomment "#define DEBUG_SHADER" for testing.
		return Vec3(fabsf(normal.x), fabsf(normal.y), fabsf(normal.z));
#endif

#ifdef SMOOTH_SHADER
		// TODO [DONE] 3.3 e) Implement diffuse shading using the interpolated vertex normal here.
		Vec3 normal = bvh->tris[rec.id].getInterpolatedNormal(ray);

		// Uncomment "#define SMOOTH_SHADER" for testing.
		return fabsf(ray.dir * normal);
#endif

#ifdef POINTLIGHT_SHADER

		// TODO [DONE] 3.4 a) Calculate the shade by summing up the contributions of all four pointlights.
		// Take the direction towards the light source, the surface normal and the distance into account.


		// TODO [DONE] 3.4 b) Check if each lightsource is visible from the shaded surface at all.
		// Only add its contribution if it is.

		// TODO [ALMOST DONE] (mirror reflection is too bright) 3.5 Shade all triangles with rec.id >= firstMirrorTriangle with mirror shading instead of pointlight shading.

		// Uncomment "#define POINTLIGHT_SHADER" for testing.
		return compute_light_intensity(ray, rec, true);// Replace with point light / mirror shader shade color.
#endif

#ifdef AREALIGHT_SHADER
		// TODO 3.6 b) Implement area light shading by picking a random point light on the area light's surface.

		// TODO 3.6 c) Implement shading by averaging the shade from 1000 random point lights from the area light's surface.

		// Uncomment "#define AREALIGHT_SHADER" for testing.
		return compute_light_intensity(ray, rec, false);
#endif
	}

	return Vec3(0.0f, 0.0f, 0.0f);
}

/**
 * Renders an image of ResX * ResY size into buffer.
 */
void render()
{
	for (int y = 0; y < ResY; y++)
	{
		for (int x = 0; x < ResX; x++)
		{
			Ray ray = cam->getRay(x, y);

			buffer[x + y * ResX] = rayTrace(ray);
		}
	}
}

/**
 * Main program routine.
 */
int main(int argc, char **argv)
{
	// TODO [Done] 3.3 a) Also load the normal file.


	float *meshData;
	float *normalData;
	int num_data;
	int num_data_2;

	const char* meshFilename = argc >= 2 ? argv[1] : "bunny.ra2";
	const char* normalFilename = argc >= 2 ? argv[2] : "bunny.n";
	num_data = load_float_data(meshFilename, meshData);
	num_data_2 = load_float_data(normalFilename, normalData);


	// The two additional triangles are for the ground...
	int num_tris = num_data / 9 + 2;
	Triangle *tris = new Triangle[num_tris];

	for (int t = 0; t < num_tris; t++)
	{
		for (unsigned int v = 0; v < 3; v++)
		{
			for (int d = 0; d < 3; d++)
			{
				tris[t].v[v][d] = meshData[t * 9 + v * 3 + d];
				tris[t].n[v][d]=normalData[t * 9 + v * 3 + d];
			}
		}
	}
	delete[] meshData;
	delete[] normalData;

	// Bounding box around the loaded part of the scene.
	AABB sceneBox = Triangle::getAABB(tris, num_tris - 2);

	// Initialization of the point lights.
	pointlights[0].pos = Vec3(sceneBox.bounds[0].x, sceneBox.bounds[1].y,
			sceneBox.bounds[0].z);
	pointlights[0].color = Vec3(1500.0f, 1500.0f, 1500.0f);
	pointlights[1].pos = Vec3(sceneBox.bounds[1].x, sceneBox.bounds[1].y,
			sceneBox.bounds[0].z);
	pointlights[1].color = Vec3(1500.0f, 0.0f, 0.0f);
	pointlights[2].pos = Vec3(sceneBox.bounds[0].x, sceneBox.bounds[1].y,
			sceneBox.bounds[1].z);
	pointlights[2].color = Vec3(0.0f, 1500.0f, 0.0f);
	pointlights[3].pos = Vec3(sceneBox.bounds[1].x, sceneBox.bounds[1].y,
			sceneBox.bounds[1].z);
	pointlights[3].color = Vec3(0.0f, 0.0f, 1500.0f);

	// Initialization of the area light.
	areaLight.pos = Vec3(sceneBox.bounds[0].x, sceneBox.bounds[1].y,
			sceneBox.bounds[0].z);
	areaLight.extent1 = Vec3(sceneBox.bounds[1].x - sceneBox.bounds[0].x, 0.0f,
			0.0f);
	areaLight.extent2 = Vec3(0.0f, 0.0f,
			sceneBox.bounds[1].z - sceneBox.bounds[0].z);
	areaLight.radiance = Vec3(1.0f, 1.0f, 1.0f);

	// Adding hardcoded ground plane which should be rendered as mirror in 3.5
	float boxWidth = sceneBox.bounds[1].x - sceneBox.bounds[0].x;
	float boxHeight = sceneBox.bounds[1].z - sceneBox.bounds[0].z;
	sceneBox.bounds[0].x -= boxWidth;
	sceneBox.bounds[1].x += boxWidth;
	sceneBox.bounds[0].z -= boxHeight;
	sceneBox.bounds[1].z += boxHeight;
	tris[num_tris - 2].v[0] = Vec3(sceneBox.bounds[0].x, sceneBox.bounds[0].y,
			sceneBox.bounds[1].z);
	tris[num_tris - 2].v[1] = Vec3(sceneBox.bounds[1].x, sceneBox.bounds[0].y,
			sceneBox.bounds[1].z);
	tris[num_tris - 2].v[2] = Vec3(sceneBox.bounds[0].x, sceneBox.bounds[0].y,
			sceneBox.bounds[0].z);
	tris[num_tris - 1].v[0] = Vec3(sceneBox.bounds[1].x, sceneBox.bounds[0].y,
			sceneBox.bounds[1].z);
	tris[num_tris - 1].v[1] = Vec3(sceneBox.bounds[1].x, sceneBox.bounds[0].y,
			sceneBox.bounds[0].z);
	tris[num_tris - 1].v[2] = Vec3(sceneBox.bounds[0].x, sceneBox.bounds[0].y,
			sceneBox.bounds[0].z);

	// TODO [Done] 3.3 a) Initialize the normals of the ground triangles with (0, 1, 0).
	// If your normals in "Vec3 Triangle::n", you may just uncomment the following lines.

	tris[num_tris - 2].n[0] = Vec3(0.0f, 1.0f, 0.0f);
	tris[num_tris - 2].n[1] = Vec3(0.0f, 1.0f, 0.0f);
	tris[num_tris - 2].n[2] = Vec3(0.0f, 1.0f, 0.0f);
	tris[num_tris - 1].n[0] = Vec3(0.0f, 1.0f, 0.0f);
	tris[num_tris - 1].n[1] = Vec3(0.0f, 1.0f, 0.0f);
	tris[num_tris - 1].n[2] = Vec3(0.0f, 1.0f, 0.0f);

	firstMirrorTriangle = num_tris - 2;

	std::cout << "#Triangles " << num_tris << std::endl;

	bvh = new BVH(tris, num_tris);

	std::cout << bvh->bbox.bounds[0][0] << " " << bvh->bbox.bounds[0][1] << " "
			<< bvh->bbox.bounds[0][2] << std::endl;
	std::cout << bvh->bbox.bounds[1][0] << " " << bvh->bbox.bounds[1][1] << " "
			<< bvh->bbox.bounds[1][2] << std::endl << std::endl;

	buffer = new Vec3[ResX * ResY];

	cam = new Cam(bvh->bbox, ResX, ResY);

	drand = new MTRand(1337); // the initialisation is arbitrary, but never initialize randomly (i.e. time or /dev/ramdom), this would make
							  // debugging much more complicated (i.e. during rendering a scertain sequence of random numbers generates
							  // a ray which results in a segmentation fault during ray traversal -> this can not be debugged with
							  // a randomly initialized PRNG.
	std::cout << "Pseudo random number generation example, see main.cpp"
			<< std::endl << std::endl;
	std::cout << "10 real pseudo random numbers in [0,1]: " << std::endl;
	for (int i = 0; i < 10; i++)
	{
		std::cout << drand->rand() << " ";
	}
	std::cout << std::endl;
	std::cout << "10 integer pseudo random numbers in [0,2^32-1]: "
			<< std::endl;
	for (int i = 0; i < 10; i++)
	{
		std::cout << drand->randInt() << " ";
	}
	std::cout << std::endl;
	std::cout << "10 integer pseudo random numbers in [0,777]: " << std::endl;
	for (int i = 0; i < 10; i++)
	{
		std::cout << drand->randInt(777) << " ";
	}
	std::cout << std::endl;

#ifdef INTERACTIVE
	initScreen(ResX, ResY);
	char title[256];
	float fps = 0.0f;
	int frame = 0;
	unsigned int t0 = SDL_GetTicks();

	while (!finished)
	{
		eventHandling();
		cam->cam_move();

		render();

		glDrawPixels(ResX, ResY, GL_RGB, GL_FLOAT, (float*) buffer);
		SDL_GL_SwapBuffers();
		sprintf(title, "%g fps", fps);
		SDL_WM_SetCaption(title, NULL);
		unsigned int t1 = SDL_GetTicks();
		frame++;
		if (t1 - t0 > 500.0f)
		{
			fps = 1000.0f / (t1 - t0) * frame;
			t0 = t1;
			frame = 0;
		}
	}
#else
	render();
#endif
	save_image_ppm("image.ppm", (float*) buffer, ResX, ResY);

	return 0;
}

