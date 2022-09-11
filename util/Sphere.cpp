#include <string>
#include <iostream>
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "Sphere.hpp"

// Constructor
Sphere::Sphere(std::string spherename, glm::vec3 pos, glm::vec3 scl, glm::vec3 clr, float ka, float kd, float ks, float kr, float spec) {
	name = spherename;
	position = pos;
	scaling = scl;
	colour = clr;
	Ka = ka;
	Kd = kd;
	Ks = ks;
	Kr = kr;
	n = spec;
	inside = false;

	glm::mat4 inv_translation = glm::mat4(
		glm::vec4(1, 0, 0, 0),
		glm::vec4(0, 1, 0, 0),
		glm::vec4(0, 0, 1, 0),
		glm::vec4(-position.x, -position.y, -position.z, 1)
	);
	
	glm::mat4 inv_scaling = glm::mat4(
		glm::vec4(1 / scaling.x, 0, 0, 0),
		glm::vec4(0, 1 / scaling.y, 0, 0),
		glm::vec4(0, 0, 1 / scaling.z, 0),
		glm::vec4(0, 0, 0, 1)
	);

	// Inverse of the translation and scaling
	inv_transform =  inv_scaling * inv_translation;
}

// Compute the intersection between ray and sphere
bool Sphere::intersect(Ray r, float& t0, float& t1) {
	// Transform the ray so it handles like the canonical sphere
	glm::vec3 start = inv_transform * r.get_start();
	glm::vec3 dir = inv_transform * r.get_direction();

	float a = glm::dot(dir, dir);
	float b = glm::dot(start, dir);
	float c = glm::dot(start, start) - 1.0f;

	// The discriminant of the quadratic formula
	float discriminant = (b * b) - (a * c);
	if (discriminant < 0) return false; // Quadratic has no solutions
	else if (discriminant == 0) { // Quadratic only has one solution
		t0 = (-b / a);
		t1 = t0;
	}
	else { // Quadratic has 2 solutions 
		t0 = (-b - sqrt(discriminant)) / (a);
		t1 = (-b + sqrt(discriminant)) / (a);

		// Ignore the values that are negative
		if (t0 < MIN_T && t1 < MIN_T) return false;
	}

	return true;
}

// Get the normal of the sphere
glm::vec3 Sphere::compute_normal_at_point(Ray r, float t) {
	// Transpose the inverse transformation matrix to get the transformed normal
	glm::mat4 transpose_mat = glm::transpose(inv_transform);

	// Get the point of the surface in the canonical sphere
	glm::vec4 start = inv_transform * r.get_start();
	glm::vec4 dir = inv_transform * r.get_direction();

	// Transform the normal of the canonical sphere to the transformed sphere
	glm::vec4 point = start + (t * dir);
	glm::vec4 norm = transpose_mat * (point - glm::vec4(0, 0, 0, 1));

	glm::vec3 n = norm;
	return glm::normalize(n);
}

// Return the sphere colour
glm::vec3 Sphere::get_colour() {
	return colour;
}

// Return the material properties
materials Sphere::get_materials() {
	materials m;
	m.ka = Ka;
	m.kd = Kd;
	m.kr = Kr;
	m.ks = Ks;
	m.n = n;
	return m;
}

// Return the name of the sphere
std::string Sphere::get_name() {
	return name;
}

// Return boolean that checks if sphere is cut by near plane
bool Sphere::get_inside() {
	return inside;
}

// Set the value for a sphere cut by the near plane
void Sphere::set_inside() {
	inside = true;
}
