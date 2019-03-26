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

#include "Aseprite.h"
#include <map>
#include <string>
#include <zlib.h>

#define READ_NUMBER(val) is.read(reinterpret_cast<char*>(&val), sizeof(val))

void read_string(std::ifstream& is, std::string& str) {
	glm::u16 string_length;
	READ_NUMBER(string_length);
	str.resize(string_length);
	is.read(str.data(), string_length);
}

struct CelAppearance {
	glm::u16 frame_id;
	glm::u16 layer_id;
};

inline bool operator<(const CelAppearance& v1, const CelAppearance& v2) {
	if (v1.frame_id != v2.frame_id)
		return v1.frame_id < v2.frame_id;
	else
		return v1.layer_id < v2.layer_id;
}

// Reference: https://github.com/aseprite/aseprite/blob/master/docs/ase-file-specs.md
bool decodeAseprite(Aseprite& aseprite, std::string file) {
	std::ifstream is(file, std::ios::binary);
	if (is.fail()) {
		std::cout << "Failed to open aseprite file";
		is.close();
		return 0;
	}

	is.seekg(4, std::ios_base::cur);
	// Magic number check
	glm::u16 magic_number;
	READ_NUMBER(magic_number);
	if (magic_number != 0xA5E0) {
		std::cout << "Aseprite's magic number of the main header doesn't match!\n";
		is.close();
		return false;
	}
	glm::u16 num_frames;
	READ_NUMBER(num_frames);
	aseprite.frames.resize(num_frames);
	READ_NUMBER(aseprite.dimension);
	// Color depth check
	glm::u16 color_depth;
	READ_NUMBER(color_depth);
	if (color_depth != 32) {
		std::cout << "Colordepth of " << color_depth << "bpp isn't supported. Use 32 bpp (RGBA)\n";
		is.close();
		return false;
	}
	is.seekg(114, std::ios_base::cur);
	std::map<CelAppearance, glm::u16> cel_appearances;	// (frame_id | layer_id), cel_id
	std::vector<glm::u16vec3> cel_link_appearances;		// (frame_id | frame_to_link | layer)
														// Iterate through each frame in the file
	for (glm::u16 frame_id = 0; frame_id < num_frames; frame_id++) {
		//std::cout << "Frame: " << frame_id << "\n";
		is.seekg(4, std::ios_base::cur);
		// Magic number check
		glm::u16 magic_number;
		READ_NUMBER(magic_number);
		if (magic_number != 0xF1FA) {
			std::cout << "Aseprite's magic number of frame " << frame_id << " doesn't match!\n";
			is.close();
			return false;
		}
		glm::u16 num_chunks_old;
		READ_NUMBER(num_chunks_old);
		READ_NUMBER(aseprite.frames[frame_id].duration);
		is.seekg(2, std::ios_base::cur);
		glm::u32 num_chunks;
		READ_NUMBER(num_chunks);
		if (num_chunks == 0)
			num_chunks = num_chunks_old;

		// Iterate through each chunk in this frame
		for (glm::u32 chunk_id = 0; chunk_id < num_chunks; chunk_id++) {
			glm::u32 chunk_size;
			READ_NUMBER(chunk_size);
			glm::u16 chunk_type;
			READ_NUMBER(chunk_type);
			//std::cout << "Chunk: 0x" << std::hex << chunk_type << std::dec << "\n";
			switch (chunk_type) {
			case 0x2004: { // Layer Chunk
				is.seekg(16, std::ios_base::cur);
				std::string layer_name;
				read_string(is, layer_name);
				aseprite.layers.push_back({ layer_name });
			}
						 break;
			case 0x2005: { // Cel Chunk
				AsepriteCel cel;
				READ_NUMBER(cel.layer_id);
				READ_NUMBER(cel.position);
				is.seekg(1, std::ios_base::cur);
				glm::u16 cel_type;
				READ_NUMBER(cel_type);
				is.seekg(7, std::ios_base::cur);
				if (cel_type == 1) {
					// Linked Cel
					glm::u16 frame_position;
					READ_NUMBER(frame_position);
					// Search later on for this cell
					cel_link_appearances.push_back(glm::u16vec3(frame_id, frame_position, cel.layer_id));
					//std::cout << "Linked Cel for Layer " << aseprite.layers[cel.layer_id].layer_name << " on Frame " << frame_position << "\n";
				}
				else {
					READ_NUMBER(cel.dimension);
					size_t pixel_count = cel.dimension.x * cel.dimension.y;
					cel.pixel_data.resize(pixel_count);
					if (cel_type == 0) {
						// Raw Image
						is.read(reinterpret_cast<char*>(cel.pixel_data.data()), pixel_count * 4);
						//std::cout << "Raw Cel for Layer " << aseprite.layers[cel.layer_id].layer_name << "\n";
					}
					else {
						//std::cout << "Decompress...\n";
						// Compressed Image
						//is.seekg(2, std::ios_base::cur);
						size_t compressed_size = chunk_size - 26;
						std::vector<Bytef> compressed_image;
						compressed_image.resize(compressed_size);
						is.read(reinterpret_cast<char*>(compressed_image.data()), compressed_size);

						z_stream infstream;
						infstream.zalloc = Z_NULL;
						infstream.zfree = Z_NULL;
						infstream.opaque = Z_NULL;
						infstream.avail_in = (uInt)compressed_size;					// size of input
						infstream.next_in = (Bytef *)compressed_image.data();			// input
						infstream.avail_out = (uInt)pixel_count * 4;					// size of output
						infstream.next_out = (Bytef *)cel.pixel_data.data();		// output

																					// the actual DE-compression work.
						inflateInit(&infstream);
						inflate(&infstream, Z_NO_FLUSH);
						inflateEnd(&infstream);
						//is.seekg(chunk_size - 26, std::ios_base::cur);
						//std::cout << "Compressed Cel for Layer " << aseprite.layers[cel.layer_id].layer_name << "\n";
					}
					const glm::u16 cel_id = glm::u16(aseprite.cels.size());
					// Save the cel in this sprite
					aseprite.cels.push_back(cel);
					// Save where you can find the first cel
					cel_appearances[{frame_id, cel.layer_id}] = cel_id;
					// Link this frame with the new cel
					aseprite.frames[frame_id].cel_ids.push_back(cel_id);

				}
			}
			break;
			case 0x2022: {
				AsepriteSlice slice;
				glm::u32 slice_number;
				READ_NUMBER(slice_number);
				glm::u32 slice_flag;
				READ_NUMBER(slice_flag);
				is.seekg(4, std::ios_base::cur);
				read_string(is, slice.name);
				for (glm::u32 i = 0; i < slice_number; i++) {
					is.seekg(4, std::ios_base::cur);
					READ_NUMBER(slice.origin);
					READ_NUMBER(slice.dimension);
					if (slice_flag & 1)
						is.seekg(16, std::ios_base::cur);
					if (slice_flag & 2)
						READ_NUMBER(slice.pivot);
				}
				aseprite.slices[slice.name] = slice;
			}
			break;
			default:
				is.seekg(chunk_size - 6, std::ios_base::cur);
			}
		}
	}
	// Save the links in the frames with the cell id
	for (glm::u16vec3 &to_link : cel_link_appearances) {
		aseprite.frames[to_link.x].cel_ids.push_back(cel_appearances[{to_link.y, to_link.z}]);
	}
	is.close();
	return true;
}