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

#include "ControlSystem.h"

ControlSystem::ControlSystem(GLFWwindow * window) : m_window(window){

}

void ControlSystem::update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) {
	

    es.each<PositionComponent, PlayerControlComponent, PhysicComponent>([&](entityx::Entity entity, PositionComponent &position, PlayerControlComponent &player, PhysicComponent &physic) {
        
		if(glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) {
            physic.speed.x = 120.0f;
        }else if(glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) {
            physic.speed.x = -120.0f;
        } else {
            physic.speed.x = 0.0f;
        }
        if(glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS) {
            physic.speed.y = -120.0f;
        }
		
        if(glfwJoystickPresent(GLFW_JOYSTICK_1) == GLFW_TRUE) {
            int count;
            const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);
            position.pos += glm::vec2(120, 120) * glm::vec2(axes[0], axes[1]) * float(dt);
        }
		    
	});
}