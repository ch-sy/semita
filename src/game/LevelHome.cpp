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

#include "LevelHome.h"
#include "../systems/RenderSystem.h"
#include "../systems/MovementSystem.h"
#include "../systems/ControlSystem.h"
#include "../systems/DebugSystem.h"

LevelHome::LevelHome(Graphic &g, GLFWwindow* window) : m_g(g), m_window(window) {
	systems.add<RenderSystem>(g);
	systems.add<MovementSystem>();
	systems.add<ControlSystem>(window);
	systems.add<DebugSystem>(window,g);
	systems.configure();

	// Create example entity
	for(int i = 0; i <= 5; i += 1){
		entityx::Entity example = entities.create();
		example.assign<PositionComponent>(glm::vec2(rand() % 256, rand() % 192));
		example.assign<SpriteComponent>(spr_snow_fox, 0);
		example.assign<PhysicComponent>( glm::vec2( (rand() % 30) - 10.0f, (rand() % 30) - 10.0f) );
	}
	entityx::Entity player = entities.create();
	player.assign<PositionComponent>(glm::vec2(rand() % 256, rand() % 192));
	player.assign<SpriteComponent>(spr_snow_fox, 0);
	player.assign<PhysicComponent>( glm::vec2( 0,0) );
	player.assign<PlayerControlComponent>();
}

void LevelHome::update(entityx::TimeDelta dt) {

	systems.update<ControlSystem>(dt);
	systems.update<MovementSystem>(dt);
	systems.update<RenderSystem>(dt);
	systems.update<DebugSystem>(dt);
}