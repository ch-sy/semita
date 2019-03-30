// MIT License
// 
// Copyright(c) 2019 Christian Seybert
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <glog/logging.h>
#include <filesystem>
#include "../graphics/Graphic.h"
#include <GLFW/glfw3.h>
#include <entityx/entityx.h>
#include "LevelHome.h"

Graphic graphic;


void window_size_callback(GLFWwindow* window, int width, int height) {
	if (window == NULL)
		return;
	graphic.setWindowSize(glm::vec2(width, height));
}

int main(int argc, char* args[]) {
	// Init Logging
	std::filesystem::create_directory("../log");
	FLAGS_log_dir = "../log";
	google::InitGoogleLogging(args[0]);
	
	// Init GLFW
	CHECK(glfwInit()) << "Failed to init glfw";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window;
	CHECK(window = glfwCreateWindow(1024, 768, "semita", NULL, NULL)) << "Failed to create GLFW window";
	glfwMakeContextCurrent(window);

	// Init Graphic System
	CHECK(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) << "Failed to initialize GLAD";
	graphic.loadResources();
	graphic.setWindowSize(glm::vec2(1024, 768));

	glfwSetFramebufferSizeCallback(window, window_size_callback);
	LevelHome level(graphic, window);
	double dt = 0;
	double pre_time = glfwGetTime();
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		graphic.setView(glm::vec2(0, 0), glm::vec2(256, 192));
		glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		level.update(dt);
		glfwSwapBuffers(window);

		// Measure delta time
		double new_time = glfwGetTime();
		dt = new_time - pre_time;
		pre_time = new_time;
	}

	return 0;
}
