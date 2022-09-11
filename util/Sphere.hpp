#include "Ray.hpp"

struct materials {
	float ka;
	float kd;
	float ks;
	float kr;
	float n;
};

class Sphere {
private:
	glm::vec3 position;
	glm::vec3 scaling;
	glm::vec3 colour;
	materials m;
	float Ka;
	float Kd;
	float Ks;
	float Kr;
	float n;
	glm::mat4 transform;
	glm::mat4 inv_transform;
	std::string name;
	bool inside;

public:
	Sphere(std::string spherename, glm::vec3 pos, glm::vec3 scl, glm::vec3 clr, float ka, float kd, float ks, float kr, float spec);
	bool intersect(Ray r, float& t0, float& t1);
	glm::vec3 get_colour();
	materials get_materials();
	glm::vec3 Sphere::compute_normal_at_point(Ray r, float t);
	std::string Sphere::get_name();
	bool Sphere::get_inside();
	void Sphere::set_inside();
};
