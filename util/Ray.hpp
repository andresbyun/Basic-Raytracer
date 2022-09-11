const float MIN_T = 0.0001f;

class Ray {
private:
	glm::vec4 direction;
	glm::vec4 start;
	unsigned int depth;

public:
	Ray(glm::vec4 s, glm::vec4 c);
	unsigned int get_depth();
	void Ray::set_depth(unsigned int d);
	glm::vec4 get_direction();
	glm::vec4 get_start();
	Ray Ray::reflect(glm::vec4 N, float t, unsigned int d);
	glm::vec4 Ray::point_at(float t);
};
