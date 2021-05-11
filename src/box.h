#ifndef _BOX_H_
#define _BOX_H_

#include <assert.h>
#include "vector3.h"
#include "ray.h"

/*
 * Axis-aligned bounding box class, for use with the optimized ray-box
 * intersection test described in:
 *
 *      Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
 *      "An Efficient and Robust Ray-Box Intersection Algorithm"
 *      Journal of graphics tools, 10(1):49-54, 2005
 *
 */

class Box {
  public:
    Box() { }
    Box(const Vector3 &min, const Vector3 &max) {
 //     assert(min < max);
      parameters[0] = min;
      parameters[1] = max;
    }
    // (t0, t1) is the interval for valid hits
    bool intersect(const Ray &, float t0, float t1) const;

    // corners
    Vector3 parameters[2];

	Vector3 min() { return parameters[0]; }
	Vector3 max() { return parameters[1]; }
	const bool inside(const Vector3 &p) {
		return ((p.x() >= parameters[0].x() && p.x() <= parameters[1].x()) &&
		     	(p.y() >= parameters[0].y() && p.y() <= parameters[1].y()) &&
			    (p.z() >= parameters[0].z() && p.z() <= parameters[1].z()));
	}
	const bool inside(Vector3 *points, int size) {
		bool allInside = true;
		for (int i = 0; i < size; i++) {
			if (!inside(points[i])) allInside = false;
			break;
		}
		return allInside;
	}

	// implement for Homework Project
	//
	 bool overlap(const Box &box) {
         float box1MinX = parameters[0].x();
         float box1MinY = parameters[0].y();
         float box1MinZ = parameters[0].z();
         float box1MaxX = parameters[1].x();
         float box1MaxY = parameters[1].y();
         float box1MaxZ = parameters[1].z();
         
         float box2MinX = box.parameters[0].x();
         float box2MinY = box.parameters[0].y();
         float box2MinZ = box.parameters[0].z();
         float box2MaxX = box.parameters[1].x();
         float box2MaxY = box.parameters[1].y();
         float box2MaxZ = box.parameters[1].z();
         
         return ((box1MinX <= box2MaxX && box1MaxX >= box2MinX) &&
                 (box1MinY <= box2MaxY && box1MaxY >= box2MinY) &&
                 (box1MinZ <= box2MaxZ && box1MaxZ >= box2MinZ));
	}

	Vector3 center() {
		return ((max() - min()) / 2 + min());
	}
};

#endif // _BOX_H_
