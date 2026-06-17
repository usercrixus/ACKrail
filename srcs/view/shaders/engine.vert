#version 330 core

layout(location = 0) in vec2 localPosition;
layout(location = 1) in vec3 localColor;
layout(location = 2) in vec2 instancePosition;
layout(location = 3) in float instanceAngle;
layout(location = 4) in vec3 instanceBodyColor;

uniform mat4 sceneMatrix;
uniform vec2 viewportSize;

out vec3 vertexColor;

void main()
{
    float cosine = cos(instanceAngle);
    float sine = sin(instanceAngle);
    vec2 rotated = vec2(
        localPosition.x * cosine - localPosition.y * sine,
        localPosition.x * sine + localPosition.y * cosine);
    vec4 center = sceneMatrix * vec4(instancePosition, 0.0, 1.0);
    vec2 pixelOffset = vec2(
        rotated.x * 2.0 / viewportSize.x,
        -rotated.y * 2.0 / viewportSize.y);
    gl_Position = center + vec4(pixelOffset, 0.0, 0.0);
    float bodyMask = step(localColor.g, localColor.r);
    vertexColor = mix(localColor, instanceBodyColor, bodyMask);
}
