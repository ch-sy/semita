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

#include "DebugSystem.h"
#include <fmt/format.h>

DebugSystem::DebugSystem(GLFWwindow * window, Graphic &g) : m_window(window), m_g(g){
}

void DebugSystem::update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) {
    m_g.drawText("Debug:", glm::vec2(1,1), fnt_h16_good_neighbor);
	m_g.drawText(fmt::format("Frame Time: {}ms", dt * 1000.0f), glm::vec2(1, 19), fnt_h9_default);
	m_g.drawText(fmt::format("{}",glGetString(GL_RENDERER)), glm::vec2(1, 28), fnt_h9_default);
    if(glfwJoystickPresent(GLFW_JOYSTICK_1) == GLFW_TRUE)
        m_g.drawText(fmt::format("{}",glfwGetJoystickName(GLFW_JOYSTICK_1)), glm::vec2(0, 37), fnt_h9_default);
}