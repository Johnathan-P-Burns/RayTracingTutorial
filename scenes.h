#ifndef SCENESH
#define SCENESH

#include "sphere.h"
#include "solid_list.h"
#include "rect.h"
#include "constant_medium.h"

solid_list *prismatic()
{
	int i = 0;
	solid **list = new solid*[1000];

	constant_texture *ct = new constant_texture(vec3(7,7,7));
	list[i++] = new sphere(vec3(500, 0, 0), 100, new diffuse_light(ct));
	list[i++] = new sphere(vec3(0, 500, 0), 100, new diffuse_light(ct));
	list[i++] = new sphere(vec3(0, 0, 500), 100, new diffuse_light(ct));
	list[i++] = new sphere(vec3(-500, 0, 0), 100, new diffuse_light(ct));
	list[i++] = new sphere(vec3(0, -500, 0), 100, new diffuse_light(ct));
	list[i++] = new sphere(vec3(0, 0, -500), 100, new diffuse_light(ct));

	dielectric *d = new dielectric(1.5);

	for(float j = 250.0; j > 0.0; j -= 5.0)
	{
		list[i++] = new sphere(vec3(0,0,0), j, d);
		list[i++] = new sphere(vec3(0,0,0), 0.1 - j, d);
	}

	return new solid_list(list, i);
}

solid_list *color_example()
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

solid_list *simple_lighting_and_reflections()
{
	int i = 0;
	solid **list = new solid*[30];
	diffuse_light *l = new diffuse_light(new constant_texture(vec3(7,7,7)));

	list[i++] = new sphere(vec3(10,0,0),  5, l);
	list[i++] = new sphere(vec3(-10,0,0), 5, l);
	list[i++] = new sphere(vec3(0,10,0),  5, l);
	list[i++] = new sphere(vec3(0,-10,0), 5, l);
	list[i++] = new sphere(vec3(0,0,10),  5, l);
	list[i++] = new sphere(vec3(0,0,-10), 5, l);

	list[i++] = new sphere(vec3(0,0,0),  1.0, new metal(vec3(0.1, 0.2, 0.3), 0.0));

	dielectric *d = new dielectric(1.5);

	list[i++] = new sphere(vec3(1,0,0),  0.5,  d);
	list[i++] = new sphere(vec3(-1,0,0), 0.5,  d);
	list[i++] = new sphere(vec3(0,1,0),  0.5,  d);
	list[i++] = new sphere(vec3(0,-1,0), 0.5,  d);
	list[i++] = new sphere(vec3(0,0,1),  0.5,  d);
	list[i++] = new sphere(vec3(0,0,-1), 0.5,  d);

	list[i++] = new sphere(vec3(1,0,0),  -0.4, d);
	list[i++] = new sphere(vec3(-1,0,0), -0.4, d);
	list[i++] = new sphere(vec3(0,1,0),  -0.4, d);
	list[i++] = new sphere(vec3(0,-1,0), -0.4, d);
	list[i++] = new sphere(vec3(0,0,1),  -0.4, d);
	list[i++] = new sphere(vec3(0,0,-1), -0.4, d);

	list[i++] = new sphere(vec3(0,0,0), 2.0, new dielectric(1.333));
	list[i++] = new sphere(vec3(0,0,0), -1.9, new dielectric(1.333));
	return new solid_list(list, i);
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

solid_list *cornell_smoke()
{
	solid **list = new solid*[8];
	int i = 0;
	material *red = new lambertian( new constant_texture(vec3(0.65, 0.05, 0.05)) );
	material *white = new lambertian( new constant_texture(vec3(0.73, 0.73, 0.73)) );
	material *green = new lambertian( new constant_texture(vec3(0.12, 0.45, 0.15)) );
	material *light = new diffuse_light( new constant_texture(vec3(7, 7, 7)) );

	list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
	list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
	list[i++] = new xz_rect(113, 443, 127, 432, 554, light);
	list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
	list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
	list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));

	solid *b1 = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18), vec3(130, 0, 65));
	solid *b2 = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white), 15), vec3(265, 0, 295));

	list[i++] = new constant_medium(b1, 0.01, new constant_texture(vec3(1.0, 1.0, 1.0)));
	list[i++] = new constant_medium(b2, 0.01, new constant_texture(vec3(0.0, 0.0, 0.0)));

	return new solid_list(list, i);
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

#endif
