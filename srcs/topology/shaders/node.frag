#version 330 core

in vec3 vertexColor;

out vec4 fragmentColor;

void main()
{
    vec2 centered = gl_PointCoord * 2.0 - 1.0;
    if (dot(centered, centered) > 1.0)
        discard;
    fragmentColor = vec4(vertexColor, 1.0);
}
