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
#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>
#include "Graphic.h"
#include "Aseprite.h"
#include <glog/logging.h>

std::string GetAnimationName(AnimationId animation_id) {
	for (auto &relation : kAnimationNames) {
		if (relation.second == animation_id)
			return relation.first;
	}
	return "UNKNOWN";
};

void Graphic::drawSprite(SpriteId sprite, 
						 glm::vec2 position, 
						 AnimationId animation, 
						 int animation_progress) {
	glUniform2fv(m_uniform_position, 1, glm::value_ptr(position));

	const SpriteData& sprite_data = m_sprites[sprite];
	const SpriteAnimation& sprite_animation = sprite_data.animations[animation];
	CHECK(sprite_animation.duration > 0) << "Sprite \"" << kSpritePaths[sprite]
		<< "\" doesn't provide the Animation \"" << GetAnimationName(animation) << "\"";
	const int ani_pos = sprite_animation.start + (animation_progress % sprite_animation.duration);
	const SpriteFrame& sprite_frame = sprite_data.frames.lower_bound(ani_pos)->second;
	for (auto &cel : sprite_frame.cels) {
		glBindTexture(GL_TEXTURE_2D, cel.texture);
		glDrawArrays(GL_TRIANGLE_STRIP, cel.vertex_start_id, 4);
	}
}

void drawText(std::string text, glm::vec2 position) {

}

void initTexture(AsepriteCel &cel) {
	glGenTextures(1, &cel.texture_id);
	glBindTexture(GL_TEXTURE_2D, cel.texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cel.dimension.x, cel.dimension.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, cel.pixel_data.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

bool Graphic::loadResources() {
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	std::vector<glm::vec2> vertex_buffer_data;

	// Loading fonts
	for (size_t i = 0; i < FONT_COUNT; i++) {
		Aseprite aseprite;
		CHECK(decodeAseprite(aseprite, kFontPaths[i])) << "Can't load font " << kFontPaths[i];
		initTexture(aseprite.cels[0]);
		m_fonts[i].texture = aseprite.cels[0].texture_id;
		for (auto &slice : aseprite.slices) {
			if (slice.first == "space") {
				m_fonts[i].space = slice.second.dimension;
			}
			else {
				glm::u32 utf32char = slice.first.at(0);
				const glm::vec2 pos = -slice.second.pivot;
				const glm::vec2 dim = slice.second.dimension;
				glm::vec2 texpos = slice.second.origin;
				texpos -= aseprite.cels[0].position;
				glm::vec2 texdim = slice.second.dimension;
				texpos /= aseprite.cels[0].dimension;
				texdim /= aseprite.cels[0].dimension;

				// Save dimension and position in the vertex buffer
				m_fonts[i].chars[utf32char] = { float(slice.second.dimension.x - slice.second.pivot.x), GLint(vertex_buffer_data.size() / 2) };

				// Add char to the vertex buffer
				vertex_buffer_data.push_back(pos);
				vertex_buffer_data.push_back(texpos);
				vertex_buffer_data.push_back(pos + glm::vec2(dim.x, 0.0f));
				vertex_buffer_data.push_back(texpos + glm::vec2(texdim.x, 0.0f));
				vertex_buffer_data.push_back(pos + glm::vec2(0.0f, dim.y));
				vertex_buffer_data.push_back(texpos + glm::vec2(0.0f, texdim.y));
				vertex_buffer_data.push_back(pos + dim);
				vertex_buffer_data.push_back(texpos + texdim);
			}
		}
	}

	// Loading sprites
	for (size_t i = 0; i < SPRITE_COUNT; i++) {
		Aseprite aseprite;
		std::cout << kSpritePaths[i] << std::endl;
		
		CHECK(decodeAseprite(aseprite, kSpritePaths[i])) << "Can't load sprite" << kSpritePaths[i];
		
		for (AsepriteCel &cel : aseprite.cels) {
			initTexture(cel);
			const glm::vec2 pos = cel.position;
			const glm::vec2 dim = cel.dimension;
			cel.vertex_id = vertex_buffer_data.size() / 2;
			vertex_buffer_data.push_back(pos);
			vertex_buffer_data.push_back(glm::vec2(0.0f, 0.0f));
			vertex_buffer_data.push_back(pos + glm::vec2(dim.x, 0.0f));
			vertex_buffer_data.push_back(glm::vec2(1.0f, 0.0f));
			vertex_buffer_data.push_back(pos + glm::vec2(0.0f, dim.y));
			vertex_buffer_data.push_back(glm::vec2(0.0f, 1.0f));
			vertex_buffer_data.push_back(pos + dim);
			vertex_buffer_data.push_back(glm::vec2(1.0f, 1.0f));
		}

		// time specifies the last time a frame shows up
		int time = -1;
		for (AsepriteFrame ase_frame : aseprite.frames) {
			time += ase_frame.duration;
			for (size_t cel_id : ase_frame.cel_ids) {
				SpriteCel cel;
				cel.texture = aseprite.cels[cel_id].texture_id;
				cel.vertex_start_id = aseprite.cels[cel_id].vertex_id;
				m_sprites[i].frames[time].cels.push_back(cel);
			}
			
		}

		// If no tags provided, use idle
		if (aseprite.tags.empty())
			aseprite.tags.push_back({ "idle", 0, glm::u16(aseprite.frames.size()-1), lad_forward, });

		// Load animations
		for (AsepriteTag tag : aseprite.tags) {
			AnimationId animation_id = kAnimationNames.at(tag.name);
			SpriteAnimation &animation = m_sprites[i].animations[animation_id];
			
			//Calculate duration for one animation
			animation = { 0, 0 };
			int frame_id = 0;
			while (frame_id < tag.from_frame_id)
				animation.start += aseprite.frames[frame_id++].duration;
			while (frame_id <= tag.to_frame_id)
				animation.duration += aseprite.frames[frame_id++].duration;
		}
	}


	glGenBuffers(1, &m_vertexbuffer_sprites);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer_sprites);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*vertex_buffer_data.size(), vertex_buffer_data.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (GLvoid*)(2 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_shader_default = loadShader("../shader/shd_default.vert", "../shader/shd_default.frag");
	m_uniform_position = glGetUniformLocation(m_shader_default, "model");
	m_uniform_projection_view = glGetUniformLocation(m_shader_default, "proj_view");
	glUseProgram(m_shader_default);
}

void Graphic::setWindowSize(glm::vec2 size) {
	m_window_size = size;
	glViewport(0, 0, size.x, size.y);
}

void Graphic::setView(glm::vec2 position, glm::vec2 size) {
	float pixel_size = glm::round(m_window_size.x / size.x);
	if (pixel_size < 1)
		pixel_size = 1;
	float screen_width = float(m_window_size.x) / pixel_size;

	glm::mat4 projection_view = glm::ortho(position.x, position.x+screen_width, position.y+float(m_window_size.y) / pixel_size, position.y);
	glUniformMatrix4fv(m_uniform_projection_view, 1, false, glm::value_ptr(projection_view));
}