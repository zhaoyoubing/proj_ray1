#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"
#include "bmp_writer.h"
#include <vector>
#include <cmath>


// Helper function for clamping values
inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

class camera {
private:
    int    image_height;   // Rendered image height
    point3 center;         // Camera center
    point3 pixel00_loc;    // Location of pixel 0, 0
    vec3   pixel_delta_u;  // Offset to pixel to the right
    vec3   pixel_delta_v;  // Offset to pixel below
    double pixel_samples_scale;


    void initialize() {
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        pixel_samples_scale = 1.0 / samples_per_pixel;

        center = point3(0, 0, 0);

        // Determine viewport dimensions.
        auto focal_length = 1.0;
        auto viewport_height = 2.0;
        auto viewport_width = viewport_height * (double(image_width)/image_height);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        auto viewport_u = vec3(viewport_width, 0, 0);
        auto viewport_v = vec3(0, -viewport_height, 0);

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left =
            center - vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    }


    color ray_color(const ray& r, int depth, const hittable& world) const {
        // If we've exceeded the ray bounce limit, no more light is gathered.
        if (depth <= 0)
            return color(0,0,0);

        hit_record rec;

        // choose a random direction to perform recursive ray tracing
        if (world.hit(r, interval(0.001, infinity), rec)) {
            vec3 direction = rec.normal + random_unit_vector();
            color bounced = ray_color(ray(rec.p, direction), depth-1, world);

            // Red ambient light: provides subtle red illumination in shadowed areas
            // Ambient intensity = 0.15 (15% of maximum brightness)
            // This creates a warm, reddish glow that affects all surfaces
            color red_ambient(0.15, 0, 0);  // Pure red: (R=0.15, G=0, B=0)

            // Combine: 50% of bounced light + red ambient light
            // The 0.5 factor reduces the strength of diffuse bounces
            return 0.5 * bounced + red_ambient;
        }

        // Sky gradient (no hit): blue-ish background
        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5*(unit_direction.y() + 1.0);
        return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
    }

    ray get_ray(int i, int j) const {
        // Construct a camera ray originating from the origin and directed at randomly sampled
        // point around the pixel location i, j.

        auto offset = sample_square();
        auto pixel_sample = pixel00_loc
                          + ((i + offset.x()) * pixel_delta_u)
                          + ((j + offset.y()) * pixel_delta_v);

        auto ray_origin = center;
        auto ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    vec3 sample_square() const {
        // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

public:
    double aspect_ratio = 1.0;  // Ratio of image width over height
    int    image_width  = 100;  // Rendered image width in pixel count
    int    samples_per_pixel = 10;
    int    max_depth = 10;   // Maximum number of ray bounces into scene


    void render(const hittable& world) {
        initialize();

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; j++) {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
            for (int i = 0; i < image_width; i++) {

                color pixel_color(0,0,0);
                for (int sample = 0; sample < samples_per_pixel; sample++) {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                }
                write_color(std::cout, pixel_samples_scale * pixel_color);
            }
        }

        std::clog << "\rDone.                 \n";
    }

    void render_to_bmp(const hittable& world, const std::string& filename) {
        initialize();

        std::vector<uint8_t> pixels(image_width * image_height * 3);

        for (int j = 0; j < image_height; j++) {
            if (j % 10 == 0)
                std::cout << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
            for (int i = 0; i < image_width; i++) {

                color pixel_color(0, 0, 0);
                for (int sample = 0; sample < samples_per_pixel; sample++) {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                }

                // Scale and clamp the color
                pixel_color = pixel_samples_scale * pixel_color;

                // Gamma correct (gamma = 2.0)
                double r_val = std::sqrt(pixel_color.x());
                double g_val = std::sqrt(pixel_color.y());
                double b_val = std::sqrt(pixel_color.z());

                // Clamp to [0, 1] and convert to [0, 255]
                int idx = (j * image_width + i) * 3;
                pixels[idx]     = (uint8_t)(256 * clamp(b_val, 0.0, 0.999));  // B
                pixels[idx + 1] = (uint8_t)(256 * clamp(g_val, 0.0, 0.999));  // G
                pixels[idx + 2] = (uint8_t)(256 * clamp(r_val, 0.0, 0.999));  // R
            }
        }

        std::cout << "\rDone.                 \n";
        BMPWriter::write(filename, image_width, image_height, pixels);
    }

};

#endif