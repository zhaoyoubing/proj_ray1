#ifndef CUBE_H
#define CUBE_H

#include "hittable.h"
#include <algorithm>

class cube : public hittable {
private:
    point3 min_corner;  // Lower-left-back corner
    point3 max_corner;  // Upper-right-front corner

public:
    cube(const point3& a, const point3& b) {
        // Ensure min_corner is actually the minimum and max_corner is the maximum
        min_corner = point3(std::fmin(a.x(), b.x()),
                            std::fmin(a.y(), b.y()),
                            std::fmin(a.z(), b.z()));
        max_corner = point3(std::fmax(a.x(), b.x()),
                            std::fmax(a.y(), b.y()),
                            std::fmax(a.z(), b.z()));
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        // AABB (Axis-Aligned Bounding Box) ray intersection
        auto t_min = ray_t.min;
        auto t_max = ray_t.max;

        for (int axis = 0; axis < 3; axis++) {
            auto origin = (axis == 0) ? r.origin().x() : (axis == 1) ? r.origin().y() : r.origin().z();
            auto direction = (axis == 0) ? r.direction().x() : (axis == 1) ? r.direction().y() : r.direction().z();
            auto min_val = (axis == 0) ? min_corner.x() : (axis == 1) ? min_corner.y() : min_corner.z();
            auto max_val = (axis == 0) ? max_corner.x() : (axis == 1) ? max_corner.y() : max_corner.z();

            auto t0 = (min_val - origin) / direction;
            auto t1 = (max_val - origin) / direction;

            if (direction < 0) std::swap(t0, t1);

            t_min = std::fmax(t0, t_min);
            t_max = std::fmin(t1, t_max);

            if (t_max <= t_min) return false;
        }

        // Find which face was hit and calculate normal
        rec.t = t_min;
        rec.p = r.at(rec.t);

        // Determine which face was hit based on which coordinate is closest to boundary
        vec3 outward_normal(0, 0, 0);
        double epsilon = 1e-4;

        if (std::fabs(rec.p.x() - min_corner.x()) < epsilon) outward_normal = vec3(-1, 0, 0);
        else if (std::fabs(rec.p.x() - max_corner.x()) < epsilon) outward_normal = vec3(1, 0, 0);
        else if (std::fabs(rec.p.y() - min_corner.y()) < epsilon) outward_normal = vec3(0, -1, 0);
        else if (std::fabs(rec.p.y() - max_corner.y()) < epsilon) outward_normal = vec3(0, 1, 0);
        else if (std::fabs(rec.p.z() - min_corner.z()) < epsilon) outward_normal = vec3(0, 0, -1);
        else if (std::fabs(rec.p.z() - max_corner.z()) < epsilon) outward_normal = vec3(0, 0, 1);

        rec.set_face_normal(r, outward_normal);
        return true;
    }
};

#endif