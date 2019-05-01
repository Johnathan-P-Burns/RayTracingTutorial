#include <thread>
#include <future>
#include <vector>
#include <float.h>
#include "sphere.h"
#include "moving_sphere.h"
#include "solid_list.h"
#include "camera.h"
#include "material.h"
#include "bvh_node.h"
#include "box.h"
#include "rect.h"
#include "constant_medium.h"
#include "texture.h"
#include "scenes.h"

// Necessary Evils
const std::size_t dimX = 400;
const std::size_t dimY = 400;
const std::size_t dimTotal = dimX * dimY;
const std::size_t samples = 100;
const vec3 lookfrom(5, 5, 5);
const vec3 lookat(0, 0, 0);
//const vec3 lookfrom(478, 278, -800);
//const vec3 lookat(278, 278, 0);
const float aperture = 0.0;
const float dist_to_focus = 10.0;
const float vfov = 15.0;


vec3 color(const ray& r, solid *world, int depth);

int main()
{
	std::size_t cores = std::thread::hardware_concurrency();

	volatile std::atomic<std::size_t> count(0);
	std::vector< std::future< void > > future_vector;

	std::vector< vec3 * > image; 

	image.resize(dimTotal);
	solid_list *world = color_example();

	// Split the computation
	while(cores--)
	{
		future_vector.emplace_back(
			std::async([=, &count, &image]()
			{

				camera cam(lookfrom, lookat, vec3(0,1,0), vfov, float(dimX)/float(dimY), aperture, dist_to_focus, 0.0, 1.0);
				while(1)
				{
					std::size_t index = count++;
					if(index > dimTotal) break;

					int i = index % dimX;
					int j = index / dimY;

					vec3 col(0,0,0);

					for(int sample = 0; sample < samples; sample++)
					{
						float u = float(i + drand48()) / float(dimX);
						float v = float(j + drand48()) / float(dimY);

						ray r = cam.get_ray(u, v);
						vec3 p = r.point_at_parameter(2.0);
						col += color(r, world, 0);
					}

					col /= float(samples);
					col = vec3(sqrt(col.r()), sqrt(col.g()), sqrt(col.b()));

					int r = int(255.99 * col.r());
					int g = int(255.99 * col.g());
					int b = int(255.99 * col.b());

					image.at(index) = new vec3(r, g, b);

					if(index % 10000 == 0)
						std::cerr << "Rendered pixel " << index << std::endl;
				}
			}));
	}

	// Print PPM headers
	std::cout << "P3\n" << dimX << " " << dimY << "\n255" << std::endl;

	// Await the future(s)
	for(int i = 0; i < std::thread::hardware_concurrency(); i++)
	{
		future_vector[i].wait();
	}

	// Print the vector
	for(int j = dimY - 1; j >= 0; j--)
	{
		for(int i = 0; i < dimX; i++)
		{
		std::cout << (int)image[i + dimX * j]->r() << " " << (int)image[i + dimX * j]->g() << " " << (int)image[i + dimX * j]->b() << std::endl;
		}
	}

	return 0;
}

vec3 color(const ray& r, solid *world, int depth)
{
	hit_record rec;
	if(world->hit(r, 0.001, FLT_MAX, rec))
	{
		ray scattered;
		vec3 attenuation;
		vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
		if(depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered))
		{
			return emitted + attenuation * color(scattered, world, depth + 1);
		}
		else
		{
			return emitted;
		}
	}
	else
	{
		return vec3(0, 0, 0);
	}
}
