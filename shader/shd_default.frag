#version 330 core

in vec2 texCoord;

out vec4 color;

uniform sampler2D u_sTexture;

void main()
{
    color = texture( u_sTexture, texCoord );
}