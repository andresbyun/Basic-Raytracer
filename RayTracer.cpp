// RayTracer.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Import all the necessary libraries
#include <iostream>
#include <string>
#include <string.h>
#include <fstream>
#include <vector>
#include <regex>
#include <cmath>
#include <algorithm>
#include "util/glm/vec3.hpp"
#include "util/glm/mat3x3.hpp"
#include "util/glm/vec4.hpp"
#include "util/glm/mat4x4.hpp"
#include "util/Light.hpp"
#include "util/Sphere.hpp"
#include "util/file_io.hpp"
#include "RayTracer.hpp"

// Shared variables (These are global to not pass them in the recursive raytrace function when they aren't needed)
std::vector<Sphere> spheres;
std::vector<Light> lights;
glm::vec3 background;
glm::vec3 ambient;
float near;

// Main function
int main(int argc, char** argv) {
    // RayTracer is only able to process a single file at the moment
    if (argc == 1 || argc > 2) {
        throw "ERROR: incorrect number of arguments. Please only write a single argument for this program.\n";
        std::exit(EXIT_FAILURE);
    }

    // Declaration of variables for image generation
    resolution res;
    img_plane img;
    std::string out_file;
    std::vector<std::vector<glm::vec3>> pixels;

    // Get the proper variables from the file
    read_from_file(argv[1], res, img, out_file, near, spheres, lights, background, ambient);
    pixels.resize(res.height);

    // Calculate the colour values for every pixel on the screen
    for (int i = 0; i < res.height; i++) {
        for (int j = 0; j < res.width; j++) {
            // Compute a ray from the eye to every pixel
            float uc = img.left + img.right * ((2.0f * j) / res.width); // uc = -W + W(2c/nCols)
            float vr = img.bottom + img.top * ((2.0f * i) / res.height); // vr = -H + H(2r/nRows) 

            // Shoot the rays from the bottom left corner
            glm::vec4 eye = glm::vec4(0.0, 0.0, 0.0, 1.0);
            glm::vec4 dir = glm::vec4(uc, vr, -img.near, 0.0);
            Ray r(eye, dir);

            // Set the colour for the pixel
            pixels.at(i).push_back(
                raytrace(r)
            );
        }
    }

    // Create the output file
    save_image(out_file, res.width, res.height, pixels);
    return 0;
}

// Recursive raytracing function
glm::vec3 raytrace(Ray r) {
    float t0 = MAX_DISTANCE_T, t1 = MAX_DISTANCE_T, t_nearest = MAX_DISTANCE_T;
    Sphere* curr = NULL;
    glm::vec3 colour;

    if (r.get_depth() > MAX_DEPTH)
        return BLACK;
    
    // Find the closest intersection
    for (unsigned int i = 0; i < spheres.size(); i++) {
        if (spheres.at(i).intersect(r, t0, t1)) {
            // Ignore the points that are behind the image plane
            if (r.point_at(t0).z >= near) {
                if (r.point_at(t1).z >= near) continue;
                t0 = t1;
                spheres.at(i).set_inside(); // This is a hollow sphere cut by the near plane
            }

            if (t0 < t_nearest) {
                t_nearest = t0;
                curr = &spheres.at(i);
            }
        }
    }

    // No intersections found
    if (curr == NULL) {
        // If depth is zero, set the background colour. Else don't contribute
        colour = r.get_depth() == 0 ? background : BLACK;
    }
    else {
        materials m = curr->get_materials();
        glm::vec3 obj_col = curr->get_colour();
        glm::vec3 c_local = glm::vec3(
            m.ka * ambient.r * obj_col.r,
            m.ka * ambient.g * obj_col.g,
            m.ka * ambient.b * obj_col.b
        );
        glm::vec4 point = r.point_at(t_nearest); // Get the point the ray is at the closest intersection
        glm::vec3 n_vec = (curr->compute_normal_at_point(r, t_nearest)); // Get the sphere normal

        // Add the shadow rays for every light source
        for (unsigned int i = 0; i < lights.size(); i++) {
            c_local += shadow_ray(point, lights.at(i), curr, n_vec);
        }

        clamp_colour(c_local);

        // Get the reflected ray
        Ray reflected = r.reflect(glm::vec4(n_vec, 0), t_nearest, r.get_depth() + 1);

        glm::vec3 c_reflect = raytrace(reflected);
        glm::vec3 c_refract = glm::vec3(0.0); // This is written here for completeness sake

        colour = c_local + (m.kr * c_reflect) + c_refract;
        clamp_colour(colour);
    }
    return colour;
}

// Return the colour value from the shadow ray
glm::vec3 shadow_ray(glm::vec4 point, Light source, Sphere* s_ptr, glm::vec3 norm) {
    glm::vec3 col = glm::vec3(0);
    float t0 = 0, t1 = 0;
    bool intersect = false;
    glm::vec3 n_vec = norm;

    // Construct the shadow ray
    glm::vec4 light_dir = glm::vec4(source.get_position(), 1.0f) - point;
    Ray shadow(point, light_dir);
    
    // Handle hollow sphere
    if (s_ptr->get_inside()) {
        // Check if there is a self-intersection
        if (!s_ptr->intersect(shadow, t0, t1)) return BLACK;
        // Check if the light is inside the sphere
        if (t1 < 1) return BLACK;
        n_vec = -n_vec; // Negate the normal since we are on the inside of the sphere
    }
    else {
        // Find if there is an intersection with the spheres the shadow ray
        for (unsigned int i = 0; i < spheres.size(); i++) {
            if (spheres.at(i).intersect(shadow, t0, t1)) {    
                return BLACK;
            }
        }
    }
    
    // Get the direction of the vector from the surface to the eye
    glm::vec4 eye_dir = glm::vec4(0, 0, 0, 1) - point;

    // Get the vectors for computing the value of the colour
    glm::vec3 v_vec = glm::normalize(eye_dir);
    glm::vec3 l_vec = glm::normalize(light_dir);
    glm::vec3 r_vec = glm::normalize((2.0f * glm::dot(n_vec, l_vec) * n_vec) - l_vec);

    // Obtain the spheres material properties
    glm::vec3 obj_col = s_ptr->get_colour();
    materials m = s_ptr->get_materials();
    glm::vec3 intensity = source.get_intensity();

    // Compute the dot products
    float n_dot_l = glm::dot(n_vec, l_vec) > 0 ? glm::dot(n_vec, l_vec) : 0;
    float r_dot_v = glm::dot(r_vec, v_vec) > 0 ? glm::dot(r_vec, v_vec) : 0;
    float spec = std::powf(r_dot_v, m.n);

    // Compute the colour values
    col = glm::vec3(
        (m.kd * intensity.r * n_dot_l * obj_col.r) + (m.ks * intensity.r * spec),
        (m.kd * intensity.g * n_dot_l * obj_col.g) + (m.ks * intensity.g * spec),
        (m.kd * intensity.b * n_dot_l * obj_col.b) + (m.ks * intensity.b * spec)
    );

    clamp_colour(col);
    return col;
}

// Function that clamps the value of a colour value to 1
void clamp_colour(glm::vec3& col) {
    col.r = std::min(col.r, 1.0f);
    col.g = std::min(col.g, 1.0f);
    col.b = std::min(col.b, 1.0f);
}
