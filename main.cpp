#include "camera.h"
#include "rtweekend.h"
#include "material.h"

#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "moving_sphere.h"
#include "aarect.h"
#include <memory>
#include <iostream>
#include "math.h"

color ray_color(const ray& r, const color& background, const hittable& world, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.

    if (depth <= 0)
        return color(0,0,0);

     if (!world.hit(r, 0.001, infinity, rec))
        return background;

    ray scattered;
    color attenuation;
    color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

    if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        return emitted;

    return (emitted + attenuation * ray_color(scattered, background, world, depth-1))*abs(dot(unit_vector(scattered.direction()),unit_vector( rec.normal)));
}
hittable_list random_scene() {
    hittable_list world;

    auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
    auto white = make_shared<solid_color>(color(1, 1, 1));
    auto green = make_shared<solid_color>(color(0, 1, 0));
    auto red = make_shared<solid_color>(color(1, 0, 0));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(white)));
    world.add(make_shared<sphere>(point3(-1000,0,0), 1000, make_shared<lambertian>(green)));
    world.add(make_shared<sphere>(point3(1000,0,0), 995, make_shared<lambertian>(red)));
    world.add(make_shared<sphere>(point3(0,0,1000), 990, make_shared<lambertian>(white)));
    world.add(make_shared<sphere>(point3(0,0,-1000), 1000, make_shared<lambertian>(white)));
    world.add(make_shared<sphere>(point3(0,1000,0), 995, make_shared<lambertian>(white)));
    // for (int a = -11; a < 11; a++) {
    //     for (int b = -11; b < 11; b++) {
    //         auto choose_mat = random_double();
    //         point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

    //         if ((center - point3(4, 0.2, 0)).length() > 0.9) {
    //             shared_ptr<material> sphere_material;

    //             if (choose_mat < 0.8) {
    //                 // diffuse
    //                 auto albedo = color::random() * color::random();
    //                 sphere_material = make_shared<lambertian>(albedo);
    //                 auto center2 = center + vec3(0, random_double(0,.5), 0);
    //                 world.add(make_shared<moving_sphere>(
    //                 center, center2, 0.0, 1.0, 0.2, sphere_material));
    //             } else if (choose_mat < 0.95) {
    //                 // metal
    //                 auto albedo = color::random(0.5, 1);
    //                 auto fuzz = random_double(0, 0.5);
    //                 sphere_material = make_shared<metal>(albedo, fuzz);
    //                 world.add(make_shared<sphere>(center, 0.2, sphere_material));
    //             } else {
    //                 // glass
    //                 sphere_material = make_shared<dielectric>(1.5);
    //                 world.add(make_shared<sphere>(center, 0.2, sphere_material));
    //             }
    //         }
    //     }
    // }

    // auto material1 = make_shared<diffuse_light>(color(4, 4, 4));
    // world.add(make_shared<sphere>(point3(0, 0, 0), 0.5, material1));

    auto material2 = make_shared<diffuse_light>(color(7, 6, 5));
    world.add(make_shared<sphere>(point3(2.5, 2.5, 1), 1.0, material2));

    // auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    // world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}

hittable_list cornell_box() {
    hittable_list objects;

    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(213, 343, 227, 332, 554, light));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));


    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    objects.add(make_shared<sphere>(point3(50,50, 50), 50.0, material3));


    return objects;
}


int main() {

    // Image

    auto aspect_ratio = 16.0 / 9.0;
    int image_width = 400;

    int samples_per_pixel = 10;
    const int max_depth = 50;

    hittable_list world;

    point3 lookfrom;
    point3 lookat;
    auto vfov = 40.0;
    auto aperture = 0.0;
    color background(0,0,0);

    switch (1)
    {
        case 1:
        {
            //Camera
            
            lookfrom = point3(2.5, 2.5, 9.9);
            lookat = point3(2.5, 2.5, 0);
            aperture = 0.1;
            image_width = 600;
            samples_per_pixel = 100;
            //int image_height = static_cast<int>(image_width / aspect_ratio);
            world = random_scene();
            break;
        }
        case 2:
        {
            // auto aperture = 0.1;
            //int image_height = static_cast<int>(image_width / aspect_ratio);

            world = cornell_box();
            aspect_ratio = 1.0;
            image_width = 600;
            samples_per_pixel = 200;
            lookfrom = point3(278, 278, -800);
            lookat = point3(278, 278, 0);
            vfov = 40.0;
            break;
        }
    }
    

    vec3 vup(0,1,0);
    auto dist_to_focus = 10.0;
    int image_height = static_cast<int>(image_width / aspect_ratio);


    camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 5.0);

    //camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);
    // Camera
    // camera cam(90.0, aspect_ratio);

    // Render

    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    for (int j = image_height-1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (image_width-1);
                auto v = (j + random_double()) / (image_height-1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, background,world, max_depth);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }

    std::cerr << "\nDone.\n";
}
