#include <string>
#include <iostream>
#include "glm/vec3.hpp"
#include "Light.hpp"

// Constructor
Light::Light(std::string lightname, glm::vec3 pos, glm::vec3 i) {
	name = lightname;
	position = pos;
	intensity = i;
}

// Intensity
glm::vec3 Light::get_intensity() {
	return intensity;
}

// Position
glm::vec3 Light::get_position() {
	return position;
}
