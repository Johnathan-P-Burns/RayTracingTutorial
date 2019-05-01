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

// Necessary Evils
const std::size_t dimX = 1000;
const std::size_t dimY = 1000;
const std::size_t dimTotal = dimX * dimY;
const std::size_t samples = 10000;
const vec3 lookfrom(5, 5, 5);
const vec3 lookat(0, 0, 0);
//const vec3 lookfrom(478, 278, -800);
//const vec3 lookat(278, 278, 0);
const float aperture = 0.0;
const float dist_to_focus = 10.0;
const float vfov = 15.0;


solid_list *final_scene();
solid_list *test_scene();
solid_list *random_scene();
vec3 color(const ray& r, solid *world, int depth);

int main()
{
	std::size_t cores = std::thread::hardware_concurrency();

	volatile std::atomic<std::size_t> count(0);
	std::vector< std::future< void > > future_vector;

	std::vector< vec3 * > image; 

	image.resize(dimTotal);
	solid_list *world = test_scene();

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

solid_list *final_scene()
{
	int nb = 20;
	solid **list = new solid*[30];
	solid **boxlist = new solid*[10000];
	solid **boxlist2 = new solid*[10000];
	material *white = new lambertian( new constant_texture(vec3(0.73, 0.73, 0.73)) );
	material *ground = new lambertian( new constant_texture(vec3(0.48, 0.83, 0.53)) );
	int b = 0;
	for (int i = 0; i < nb; i++)
	{
		for (int j = 0; j < nb; j++)
		{
			float w = 100;
			float x0 = -1000 + i*w;
			float z0 = -1000 + j*w;
			float y0 = 0;
			float x1 = x0 + w;
			float y1 = 100*(drand48()+0.01);
			float z1 = z0 + w;
			boxlist[b++] = new box(vec3(x0,y0,z0), vec3(x1,y1,z1), ground);
		}
	}

	int l = 0;
	list[l++] = new bvh_node(boxlist, b, 0, 1);
	material *light = new diffuse_light( new constant_texture(vec3(7, 7, 7)) );
	list[l++] = new xz_rect(123, 423, 147, 412, 554, light);
	vec3 center(400, 400, 200);
	list[l++] = new moving_sphere(center, center+vec3(30, 0, 0), 0, 1, 50, new lambertian(new constant_texture(vec3(0.7, 0.3, 0.1))));
	list[l++] = new sphere(vec3(260, 150, 45), 50, new dielectric(1.5));
	list[l++] = new sphere(vec3(0, 150, 145), 50, new metal(vec3(0.8, 0.8, 0.9), 10.0));
	solid *boundary = new sphere(vec3(360, 150, 145), 70, new dielectric(1.5));
	list[l++] = boundary;
	list[l++] = new constant_medium(boundary, 0.2, new constant_texture(vec3(0.2, 0.4, 0.9)));
	boundary = new sphere(vec3(0, 0, 0), 5000, new dielectric(1.5));
	list[l++] = new constant_medium(boundary, 0.0001, new constant_texture(vec3(1.0, 1.0, 1.0)));
	texture *pertext = new noise_texture(0.1);
	list[l++] =  new sphere(vec3(220,280, 300), 80, new lambertian( pertext ));
	int ns = 1000;
	for (int j = 0; j < ns; j++)
	{
		boxlist2[j] = new sphere(vec3(165*drand48(), 165*drand48(), 165*drand48()), 10, white);
	}
	list[l++] =   new translate(new rotate_y(new bvh_node(boxlist2,ns, 0.0, 1.0), 15), vec3(-100,270,395));
	return new solid_list(list,l);
}

solid_list *test_scene()
{
	int i, j;
	const int MAX_OBJECTS = 100;

   	i = 0;
	j = 0;

	solid **list = new solid*[MAX_OBJECTS];
	metal *met = new metal(vec3(0.2, 0.2, 0.4), 0.001);

	list[i++] = new xy_rect(-5, 5, -5, 5, -5, new diffuse_light(new constant_texture(vec3(7,0,0))));
	list[i++] = new flip_normals(new yz_rect(-5, 5, -5, 5, -5, new diffuse_light(new constant_texture(vec3(0,7,0)))));
	list[i++] = new xz_rect(-5, 5, -5, 5, -5, new diffuse_light(new constant_texture(vec3(0,0,7))));
	//list[i++] = new flip_normals(new xz_rect(-5, 5, -5, 5, -5, new diffuse_light(new constant_texture(vec3(0,0,7)))));
	list[i++] = new flip_normals(new sphere(vec3(0,0,0), 50, met));
	list[i++] = new sphere(vec3(0,0,0), 2.01, new dielectric(1.5));
	list[i++] = new sphere(vec3(0,0,0), -2.009, new dielectric(1.5));
	list[i++] = new sphere(vec3(0,0,0), 2.01, new dielectric(1.5));
	list[i++] = new sphere(vec3(0,0,0), -2.009, new dielectric(1.5));

	list[i++] = new sphere(vec3(0,0,0),  1.0, met);

	return new solid_list(list, i + j);
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
