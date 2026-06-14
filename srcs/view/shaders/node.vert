#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;

uniform mat4 sceneMatrix;
uniform float pointSize;

out vec3 vertexColor;

void main()
{
    gl_Position = sceneMatrix * vec4(position, 0.0, 1.0);
    gl_PointSize = pointSize;
    vertexColor = color;
}
