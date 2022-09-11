#include <cstdio>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <regex>
#include "glm/vec3.hpp"
#include "glm/mat3x3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "Light.hpp"
#include "Sphere.hpp"
#include "file_io.hpp"

// Function that reads the input and sets the respective variables
void read_from_file(std::string filename, resolution& res, img_plane& img, std::string& out, float& near, std::vector<Sphere>& spheres, std::vector<Light>& lights, glm::vec3& background, glm::vec3& ambient) {
    // Open the file
    std::ifstream f(filename);
    std::string line;

    std::vector<std::string> tokens;

    if (f.is_open()) {
        while (std::getline(f, line)) {
            //Get the tokens
            tokenize_line(line, tokens);
            if (tokens.size() == 0) continue;

            // Get NEAR, LEFT, RIGHT, BOTTOM, TOP lines
            if (tokens.at(0).compare("NEAR") == 0)
                img.near = get_float_from_toks(tokens);
            if (tokens.at(0).compare("LEFT") == 0)
                img.left = get_float_from_toks(tokens);
            if (tokens.at(0).compare("RIGHT") == 0)
                img.right = get_float_from_toks(tokens);
            if (tokens.at(0).compare("BOTTOM") == 0)
                img.bottom = get_float_from_toks(tokens);
            if (tokens.at(0).compare("TOP") == 0)
                img.top = get_float_from_toks(tokens);
            
            near = -img.near;

            // Get RES line
            if (tokens.at(0).compare("RES") == 0) {
                res.width = std::stoi(tokens.at(1));
                res.height = std::stoi(tokens.at(2));
            }

            // Iterate through the spheres and add them to the sphere vector
            if (tokens.at(0).compare("SPHERE") == 0) {
                std::string name = tokens.at(1);
                // Transform position into camera coordinates
                glm::vec3 pos = glm::vec4(
                    std::stof(tokens.at(2)),
                    std::stof(tokens.at(3)),
                    std::stof(tokens.at(4)),
                    1.0
                );
                glm::vec3 scl = glm::vec3(
                    std::stof(tokens.at(5)),
                    std::stof(tokens.at(6)),
                    std::stof(tokens.at(7))
                );
                glm::vec3 clr = glm::vec3(
                    std::stof(tokens.at(8)),
                    std::stof(tokens.at(9)),
                    std::stof(tokens.at(10))
                );
                float ka = std::stof(tokens.at(11));
                float kd = std::stof(tokens.at(12));
                float ks = std::stof(tokens.at(13));
                float kr = std::stof(tokens.at(14));
                float n = std::stof(tokens.at(15));

                Sphere s(name, pos, scl, clr, ka, kd, ks, kr, n);

                spheres.push_back(s);
            }

            // Iterate through the lights and add them to the light vector
            if (tokens.at(0).compare("LIGHT") == 0) {
                std::string name = tokens.at(1);
                // Transform position into camera coordinates
                glm::vec3 pos = glm::vec4(
                    std::stof(tokens.at(2)),
                    std::stof(tokens.at(3)),
                    std::stof(tokens.at(4)),
                    1.0
                );
                glm::vec3 intense = glm::vec3(
                    std::stof(tokens.at(5)),
                    std::stof(tokens.at(6)),
                    std::stof(tokens.at(7))
                );

                Light l(name, pos, intense);

                lights.push_back(l);
            }

            // Get the background color
            if (tokens.at(0).compare("BACK") == 0)
                set_vec3_with_tokens(tokens, background);

            // Get the ambient intensity
            if (tokens.at(0).compare("AMBIENT") == 0)
                set_vec3_with_tokens(tokens, ambient);

            // Get the name of the output file
            if (tokens.at(0).compare("OUTPUT") == 0) {
                out = tokens.at(1);
            }
        }

        f.close();
    }
    else {
        std::cout << "ERROR: cannot find file " << filename << '\n';
        std::exit(EXIT_FAILURE);
    }
}

// Set a vector from the tokens (only for background and ambient)
void set_vec3_with_tokens(std::vector<std::string> tokens, glm::vec3& v) {
    v.x = std::stof(tokens.at(1));
    v.y = std::stof(tokens.at(2));
    v.z = std::stof(tokens.at(3));
}

// Function that gets the float for the next line in the file
// Returns the float after the command
// Throws an error if the command is not in the expected order
float get_float_from_toks(std::vector<std::string> tokens) {
    float val;
    val = std::stof(tokens.at(1));
    return val;
}

// Function that takes in a string, tokenizes the strings with white spaces as delimiters
// Returns a vector containing the tokens
// Code skeleton found in https://www.geeksforgeeks.org/tokenizing-a-string-cpp/
void tokenize_line(std::string str, std::vector<std::string>& tok) {
    const std::regex re(R"(\s+)");

    std::sregex_token_iterator it{ str.begin(), str.end(), re, -1 };
    std::vector<std::string> tokens{ it, {} };

    // Delete empty strings
    tokens.erase(
        std::remove_if(
            tokens.begin(),
            tokens.end(),
            [](std::string const& s) {
                return s.size() == 0;
            }),
        tokens.end());

    tok = tokens;
}

// Function that creates the final output file
void save_image(std::string filename, int width, int height, std::vector<std::vector<glm::vec3>> pixels) {
    std::ofstream file(filename, std::ios_base::out | std::ios_base::binary);
    std::cout << "Generating file " << filename << '\n';

    file << "P6\n";
    file << width << ' ' << height << '\n';
    file << MAX_COLOUR_VAL << "\n";

    for (int i = height - 1; i >= 0; i--) {
        for (int j = 0; j < width; j++) {
            glm::vec3 curr_pxl = pixels.at(i).at(j) * 255.0f;
            file << (char) curr_pxl.x << (char) curr_pxl.y << (char) curr_pxl.z;
        }
    }

    std::cout << "File generation complete!\n";
    file.close();
}
