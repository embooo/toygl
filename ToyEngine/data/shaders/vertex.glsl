#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 3) in vec3 aColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 color;

void main()
{
    color = vec4(aColor, 1.0);
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
}