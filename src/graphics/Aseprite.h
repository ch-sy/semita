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

#ifndef ASEPRITE_H
#define ASEPRITE_H

#include <glm/glm.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include "../glad/glad.h"

enum LoopAnimationDirection : glm::u8 {
	lad_forward,
	lad_reverse,
	lad_ping_pong
};

struct AsepriteFrame {
	glm::u16					duration;
	std::vector<glm::u16>		cel_ids;
};

struct AsepriteLayer {
	std::string					layer_name;
};

struct AsepriteCel {
	glm::u16					layer_id;
	glm::i16vec2				position;
	glm::u16vec2				dimension;
	glm::u16					vertex_id;
	std::vector<glm::u8vec4>	pixel_data;
	GLuint						texture_id;
};

struct AsepriteTag {
	std::string					name;
	glm::u16					from_frame_id;
	glm::u16					to_frame_id;
	LoopAnimationDirection		animation_dir;
};

struct AsepriteSlice {
	glm::i32vec2				origin;
	glm::u32vec2				dimension;
	glm::i32vec2				pivot;
	std::string					name;
	GLuint						vertex_id;
};

struct Aseprite {
	glm::u16vec2				dimension;
	std::vector<AsepriteFrame>	frames;
	std::vector<AsepriteLayer>	layers;
	std::vector<AsepriteCel>	cels;
	std::vector<AsepriteTag>	tags;
	std::unordered_map<std::string, AsepriteSlice>	slices;
	std::map<glm::u32, glm::u32> vertex_chars;
};


// Reference: https://github.com/aseprite/aseprite/blob/master/docs/ase-file-specs.md
bool decodeAseprite(Aseprite& aseprite, std::string file);

#endif ASEPRITE_H