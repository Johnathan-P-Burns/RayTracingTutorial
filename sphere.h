#ifndef SPHEREH
#define SPHEREH

#include "solid.h"

class sphere: public solid
{
	public:
		sphere();
		sphere(vec3 cen, float r, material *m) : center(cen), radius(r), mat_ptr(m) {};
		virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
		virtual bool bounding_box(float t0, float t1, aabb& box) const;
		vec3 center;
		float radius;
		material *mat_ptr;
};

bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
	vec3 oc = r.origin() - center;
	float a = dot(r.direction());
	float b = dot(oc, r.direction());
	float c = dot(oc) - radius * radius;
	float d = b * b - a * c;

	if(d > 0)
	{
		float tmp = (-b - sqrt(d))/a;
		if(tmp < t_max && tmp > t_min)
		{
			rec.t = tmp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - center) / radius;
			rec.mat_ptr = mat_ptr;
			return true;
		}
		tmp = (-b + sqrt(d))/a;
		if(tmp < t_max && tmp > t_min)
		{
			rec.t = tmp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - center) / radius;
			rec.mat_ptr = mat_ptr;
			return true;
		}
	}
	return false;
}

bool sphere::bounding_box(float t0, float t1, aabb& box) const
{
	box = aabb(center - vec3(radius, radius, radius), center + vec3(radius, radius, radius));
	return true;
}

#endif
