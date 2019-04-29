#ifndef BOXH
#define BOXH

#include "solid.h"
#include "solid_list.h"
#include "rect.h"

class box: public solid
{
	public:
		box() {}
		box(const vec3& p0, const vec3& p1, material *mat);
		virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
		virtual bool bounding_box(float t0, float t1, aabb& box) const
		{
			box = aabb(pmin, pmax);
			return true;
		}
		vec3 pmin, pmax;
		solid *list_ptr;
};

box::box(const vec3& p0, const vec3& p1, material *mat)
{
	pmin = p0;
	pmax = p1;

	solid **list = new solid*[6];

	list[0] =					new xy_rect(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), mat);
	list[1] = new flip_normals(	new xy_rect(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), mat));
	list[2] =					new xz_rect(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), mat);
	list[3] = new flip_normals(	new xz_rect(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), mat));
	list[4] =					new yz_rect(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), mat);
	list[5] = new flip_normals(	new yz_rect(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), mat));

	list_ptr = new solid_list(list, 6);
}

bool box::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
	return list_ptr->hit(r, t_min, t_max, rec);
}

#endif
