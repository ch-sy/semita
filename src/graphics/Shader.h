#ifndef SHADER_H
#define SHADER_H

#include "../glad/glad.h" // include glad to get all the required OpenGL headers

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>

GLint loadShader(std::string vertexPath, std::string fragmentPath);

#endif SHADER_H