#include <iostream>
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "Ray.hpp"

// Constructor
Ray::Ray(glm::vec4 s, glm::vec4 c) {
	start = s;
	direction = c;
	depth = 0;
}

// Reflect the ray with the Normal and some t
Ray Ray::reflect(glm::vec4 N, float t, unsigned int d) {
	glm::vec4 P = this->point_at(t);
	glm::vec4 v = (-2 * (glm::dot(N, direction)) * N) + direction;

	Ray rf(P, v);
	rf.set_depth(d);
	
	return rf;
}

// Get the point a ray is at given some t
glm::vec4 Ray::point_at(float t) {
	glm::vec4 r = start + (t * direction);
	return r;
}

// Return the current depth
unsigned int Ray::get_depth() {
	return depth;
}

// Return c
glm::vec4 Ray::get_direction() {
	return direction;
}

// Return S
glm::vec4 Ray::get_start() {
	return start;
}

// Set t
void Ray::set_depth(unsigned int d) {
	depth = d;
}