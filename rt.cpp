#include <iostream>
#include "sphere.h"
#include "moving_sphere.h"
#include "rect.h"
#include "box.h"
#include "constant_medium.h"
#include "solid_list.h"
#include "float.h"
#include "camera.h"
#include "material.h"

void ppm_headers(int& nx, int& ny);
vec3 color(const ray& r, solid *world, int depth);
solid_list *random_scene();
solid_list *two_spheres();
solid_list *two_perlin_spheres();
solid_list *simple_light();
solid_list *cornell_box();
solid_list *cornell_smoke();


int main()
{
	int nx = 800;
	int ny = 400;
	int ns = 500;

	/*
	int nx = 1280;
	int ny = 720;
	int ns = 2500;
	*/

	ppm_headers(nx, ny);

	/*
	const int world_size = 5;
	solid *list[world_size];

	list[0] = new sphere(vec3(0, 0, -1), 0.5,		new lambertian(vec3(0.8, 0.3, 0.3)));
	list[1] = new sphere(vec3(0, -100.5, -1), 100,	new lambertian(vec3(0.8, 0.8, 0.0)));
	list[2] = new sphere(vec3(1, 0, -1), 0.5,		new metal(vec3(0.8, 0.6, 0.2), 0.0));
	list[3] = new sphere(vec3(-1, 0, -1), 0.5,		new dielectric(1.5));
	list[4] = new sphere(vec3(-1, 0, -1), -0.45,	new dielectric(1.5));
	solid_list *world = new solid_list(list, world_size);
	*/

	solid_list *world = cornell_smoke();

	vec3 lookfrom(278, 278, -800);
	vec3 lookat(278,278,0);
	//float dist_to_focus = (lookfrom - lookat).length();
	float dist_to_focus = 10;
	float aperture = 0.0;
	float vfov = 40.0;

	camera cam(lookfrom, lookat, vec3(0,1,0), vfov, float(nx)/float(ny), aperture, dist_to_focus, 0.0, 1.0);

	for(int j = ny - 1;  j >= 0; j--)
	{
		for(int i = 0; i < nx; i++)
		{
			vec3 col(0,0,0);
			for(int s = 0; s < ns; s++)
			{
				float u = float(i + drand48()) / float(nx);
				float v = float(j + drand48()) / float(ny);

				ray r = cam.get_ray(u, v);
				vec3 p = r.point_at_parameter(2.0);
				col += color(r, world, 0);
			}
			
			col /= float(ns);
			col = vec3( sqrt(col.r()), sqrt(col.g()), sqrt(col.b()) );
			int ir = int(255.99 * col.r());
			int ig = int(255.99 * col.g());
			int ib = int(255.99 * col.b());

			std::cout << ir << " " << ig << " " << ib << "\n";
		}
	}
}

void ppm_headers(int& nx, int& ny)
{
	std::cout << "P3\n" << nx << " " << ny << "\n255\n";
}

vec3 color(const ray& r, solid *world, int depth)
{
	hit_record rec;
	if(world->hit(r, 0.001, MAXFLOAT, rec))
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
		/*
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5 * (unit_direction.y() + 1.0);
		return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
		*/
	}
}

solid_list *random_scene()
{
	const int n = 50000;
	solid **list = new solid*[n + 1];

	texture *checker = new checker_texture(new constant_texture(vec3(0.2, 0.3, 0.1)), new constant_texture(vec3(0.9, 0.9, 0.9)));

	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(checker));
	int i = 1;
	for(int a = -10; a < 10; a++)
	{
		for(int b = -10; b < 10; b++)
		{
			float choose_mat = drand48();
			vec3 center(a + 0.9*drand48(),0.2,b+0.9*drand48());

			if((center-vec3(4,0.2,0)).length() > 0.9)
			{
				if(choose_mat < 0.5)
				{
					list[i++] = new sphere(center, 0.2, new lambertian(new constant_texture(vec3(drand48()*drand48(), drand48()*drand48(), drand48()*drand48()))));
					//list[i++] = new moving_sphere(center, center+vec3(0, 0.5*drand48(), 0), 0.0, 1.0, 0.2, new lambertian(new constant_texture(vec3(drand48()*drand48(), drand48()*drand48(), drand48()*drand48()))));
				}
				else if(choose_mat < 0.8)
				{
					list[i++] = new sphere(center, 0.2, new metal(vec3(0.5*(1+drand48()), 0.5*(1+drand48()), 0.5*(1+drand48())), 0.5*drand48()));
				}
				else
				{
					list[i++] = new sphere(center, 0.2, new dielectric(1.0 + drand48()));
				}
			}
		}
	}

	list[i++] = new sphere(vec3(0,1,0), 1.0, new dielectric(1.5));
	list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(new constant_texture(vec3(0.4, 0.2, 0.1))));
	list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

	return new solid_list(list, i);
}

solid_list *two_spheres()
{
	const int size = 2;
	texture *checker = new checker_texture(new constant_texture(vec3(0.2, 0.4, 0.05)), new constant_texture(vec3(0.9, 0.9, 0.9)));
	solid **list = new solid*[size];
	list[0] = new sphere(vec3(0, -10, 0), 10, new lambertian(checker));
	list[1] = new sphere(vec3(0, 10, 0), 10, new lambertian(checker));
	return new solid_list(list, size);
}

solid_list *two_perlin_spheres()
{
	const int size = 2;
	texture *pertext = new noise_texture(5.0);
	solid **list = new solid*[size];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(pertext));
	list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
	return new solid_list(list, size);
}

solid_list *simple_light()
{
	texture *pertext = new noise_texture(4);
	solid **list = new solid*[4];

	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(pertext));
	list[1] = new sphere(vec3(0, 2, 0), 2, new metal(vec3(0.9, 0.9, 0.1), 0.0));
	list[2] = new sphere(vec3(0, 7, 0), 2, new diffuse_light(new constant_texture(vec3(4,4,4))));
	list[3] = new xy_rect(3, 5, 1, 3, -2, new diffuse_light(new constant_texture(vec3(4,4,4))));
	return new solid_list(list, 4);
}

solid_list *cornell_box() {
	solid **list = new solid*[8];
	int i = 0;
	material *red = new lambertian( new constant_texture(vec3(0.65, 0.05, 0.05)) );
	material *white = new lambertian( new constant_texture(vec3(0.73, 0.73, 0.73)) );
	material *green = new lambertian( new constant_texture(vec3(0.12, 0.45, 0.15)) );
	material *light = new diffuse_light( new constant_texture(vec3(15, 15, 15)) );

	list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
	list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
	list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
	list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
	list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
	list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
	list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18), vec3(130,0,65));
	list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white),  15), vec3(265,0,295));
	return new solid_list(list,i);
}

solid_list *cornell_smoke()
{
	solid **list = new solid*[8];
	int i = 0;
	material *red = new lambertian( new constant_texture(vec3(0.65, 0.05, 0.05)) );
	material *white = new lambertian( new constant_texture(vec3(0.73, 0.73, 0.73)) );
	material *green = new lambertian( new constant_texture(vec3(0.12, 0.45, 0.15)) );
	material *light = new diffuse_light( new constant_texture(vec3(15, 15, 15)) );

	list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
	list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
	list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
	list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
	list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
	list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));

	solid *b1 = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18), vec3(130, 0, 65));
	solid *b2 = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white), 15), vec3(265, 0, 295));

	list[i++] = new constant_medium(b1, 0.01, new constant_texture(vec3(1.0, 1.0, 1.0)));
	list[i++] = new constant_medium(b2, 0.01, new constant_texture(vec3(0.0, 0.0, 0.0)));

	return new solid_list(list, i);
}
