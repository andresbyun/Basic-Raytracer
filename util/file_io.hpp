// Constant declarations
const int MAX_COLOUR_VAL = 255;

// Data structures
struct resolution {
	int width;
	int height;
};
struct img_plane {
	float near;
	float left;
	float right;
	float bottom;
	float top;
};

// Function declarations
float get_float_from_toks(std::vector<std::string> tokens);
void read_from_file(std::string filename, resolution& res, img_plane& img, std::string& out, float& near, std::vector<Sphere>& spheres, std::vector<Light>& lights, glm::vec3& background, glm::vec3& ambient);
void save_image(std::string filename, int width, int height, std::vector<std::vector<glm::vec3>> pixels);
void set_vec3_with_tokens(std::vector<std::string> tokens, glm::vec3& v);
void tokenize_line(std::string str, std::vector<std::string>& tok);
