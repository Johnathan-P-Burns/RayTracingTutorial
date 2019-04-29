#ifndef SOLIDLISTH
#define SOLIDLISTH

#include "solid.h"
#include "aabb.h"

class solid_list: public solid
{
	public:
		solid_list();
		solid_list(solid **l, int n) { list = l; list_size = n; }
		virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
		virtual bool bounding_box(float t0, float t1, aabb& box) const;

		solid **list;
		int list_size;
};

bool solid_list::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
	hit_record temp_rec;
	bool hit_anything = false;
	double closest_so_far = t_max;
	for(int i = 0; i < list_size; i++)
	{
		if(list[i]->hit(r, t_min, closest_so_far, temp_rec))
		{
			hit_anything = true;
			closest_so_far = temp_rec.t;
			rec = temp_rec;
		}
	}
	return hit_anything;
}

bool solid_list::bounding_box(float t0, float t1, aabb& box) const
{
	if(list_size < 1) return false;

	aabb tmp_box;
	bool first_true = list[0]->bounding_box(t0, t1, tmp_box);

	if(!first_true)
		return false;
	else
		box = tmp_box;

	for(int i = 0; i < list_size; i++)
	{
		if(list[i]->bounding_box(t0, t1, tmp_box))
		{
			box = surrounding_box(box, tmp_box);
		}
		else
		{
			return false;
		}
	}
	return true;
}

#endif
