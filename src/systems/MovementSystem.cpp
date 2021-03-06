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

#include "MovementSystem.h"


void MovementSystem::update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) {
	
	es.each<PositionComponent, PhysicComponent>([&](entityx::Entity entity, PositionComponent &position, PhysicComponent &phy) {
		if(position.pos.x <= 0 && phy.speed.x < 0)
			phy.speed.x = - phy.speed.x;
		if(position.pos.x >= 512 && phy.speed.x > 0)
			phy.speed.x = - phy.speed.x;
		if (position.pos.y >= 192 && phy.speed.y > 0) {
			phy.speed.y = 0;
			position.pos.y = 192;
		}
		else
			phy.speed.y += dt * 160;

		//phy.speed = phy.speed * 0.9 + glm::vec2(xpos, ypos) / 16.0;

		position.pos += phy.speed * float(dt);
	});
}
