// Constant declarations
const unsigned int MAX_DEPTH = 3;
const float MAX_DISTANCE_T = 100.0f;
const glm::vec3 BLACK = glm::vec3(0.0, 0.0, 0.0);

// Function declarations
int main(int argc, char** argv);
void clamp_colour(glm::vec3& col);
glm::vec3 raytrace(Ray r);
glm::vec3 shadow_ray(glm::vec4 point, Light source, Sphere* s_ptr, glm::vec3 n_vec);
