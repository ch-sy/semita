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
#include "../graphics/Aseprite.h"

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
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	Aseprite ase_cursor;
	CHECK(decodeAseprite(ase_cursor, "../sprites/spr_cursor.aseprite")) << "Cursor image not found";
	std::vector<unsigned char> pix_data;
	const int ase_width = ase_cursor.cels[0].dimension.x;
	const int ase_height = ase_cursor.cels[0].dimension.y;
	const int factor = 8;
	for(int iy = 0; iy < ase_height; iy++)
	for(int fy = 0; fy < factor; fy++)
	for(int ix = 0; ix < ase_width; ix++) 
	for(int fx = 0; fx < factor; fx++){
		pix_data.push_back((unsigned char)ase_cursor.cels[0].pixel_data[ix+iy*ase_width].r);
		pix_data.push_back((unsigned char)ase_cursor.cels[0].pixel_data[ix+iy*ase_width].g);
		pix_data.push_back((unsigned char)ase_cursor.cels[0].pixel_data[ix+iy*ase_width].b);
		pix_data.push_back((unsigned char)ase_cursor.cels[0].pixel_data[ix+iy*ase_width].a);
	}
	
	GLFWimage image;
	image.width = ase_width * factor;
	image.height = ase_height * factor;
	image.pixels = pix_data.data();
	GLFWcursor* cursor = glfwCreateCursor(&image, factor*1.5, factor*1.5);
	glfwSetCursor(window, cursor);

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
		graphic.setView(glm::vec2(0, 0), glm::vec2(512, 386));
		glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		level.update(dt);
		/*double cur_x, cur_y;
		glfwPollEvents();
		glfwGetCursorPos(window, &cur_x, &cur_y);
		graphic.drawSprite(spr_cursor, graphic.toPixelSpace(glm::vec2(cur_x, cur_y)));*/
		glfwSwapBuffers(window);

		// Measure delta time
		double new_time = glfwGetTime();
		dt = new_time - pre_time;
		pre_time = new_time;
	}

	return 0;
}
