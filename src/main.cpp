#include "rtweekend.h"

// ================================
#include "camera.h"
// ================================

#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "cube.h"


int main() {


    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;

    //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    cam.max_depth         = 50;
    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


    // =====================================================
    // World
    // adding spheres and a cube to the world

    hittable_list world;

    world.add(make_shared<sphere>(point3(   0,   0,-1), 0.5));
    world.add(make_shared<sphere>(point3(   0,-100.5,-1), 100));
    world.add(make_shared<sphere>(point3(-0.6,    0, -1.2), 0.3));

    // Add a cube: from (-0.3, -0.3, -1.8) to (0.3, 0.3, -1.3)
    world.add(make_shared<cube>(point3(0.5, -0.2, -1.5), point3(1.2, 0.5, -0.8)));
    // =====================================================

    // Render to BMP file
    cam.render_to_bmp(world, "render.bmp");

    return 0;
}