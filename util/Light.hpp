class Light {
public:
	Light::Light(std::string lightname, glm::vec3 pos, glm::vec3 i);
	glm::vec3 Light::get_intensity();
	glm::vec3 Light::get_position();
	
private:
	std::string name;
	glm::vec3 position;
	glm::vec3 intensity;
};
