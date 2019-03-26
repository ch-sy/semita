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

#pragma once

#include "../glad/glad.h"
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <string>
#include "SpriteId.h"
#include "FontId.h"
#include "AnimationId.h"

struct SpriteCel {
	GLint vertex_start_id;
	GLuint texture;
};

struct SpriteFrame {
	std::vector<SpriteCel> cels;
};

struct SpriteAnimation {
	int start;
	int duration;
};

struct SpriteData {
	SpriteAnimation animations[ANIMATION_COUNT];
	// int points to the last frame of an animation
	std::map<int, SpriteFrame> frames;
};

struct FontChar {
	float extend;
	GLint vertex_start_id;
};

struct FontData {
	glm::vec2 space;
	GLuint texture;
	std::map<glm::u32, FontChar> chars;
};

class Graphic {
private:
	GLint		m_shader_default;
	GLint		m_uniform_position;
	GLint		m_uniform_projection_view;
	GLuint		m_vertexbuffer_sprites;
	GLuint		m_vao;
	SpriteData	m_sprites[SPRITE_COUNT];
	FontData	m_fonts[FONT_COUNT];
	glm::vec2	m_window_size;

public:
	bool loadResources();
	void drawSprite(SpriteId sprite, glm::vec2 position, AnimationId animation = ani_idle, int animation_progress = 0);
	void drawText(std::string text, glm::vec2 position);
	void setView(glm::vec2 position, glm::vec2 size);
	void setWindowSize(glm::vec2 size);
};