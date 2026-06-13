#version 330 core

layout(location = 0) in vec2 localPosition;
layout(location = 1) in vec2 startPosition;
layout(location = 2) in vec2 endPosition;
layout(location = 3) in vec3 color;

uniform mat4 sceneMatrix;
uniform vec2 viewportSize;

out vec3 vertexColor;

void main()
{
    vec4 startClip = sceneMatrix * vec4(startPosition, 0.0, 1.0);
    vec4 endClip = sceneMatrix * vec4(endPosition, 0.0, 1.0);
    vec2 startPixels = startClip.xy * viewportSize * 0.5;
    vec2 endPixels = endClip.xy * viewportSize * 0.5;
    vec2 direction = normalize(endPixels - startPixels);
    vec2 normal = vec2(-direction.y, direction.x);
    float longitudinalOffset =
        abs(localPosition.x) < 0.001 ? 0.0 : localPosition.x;
    vec2 pixelOffset =
        direction * longitudinalOffset + normal * localPosition.y;
    vec4 center = mix(
        startClip,
        endClip,
        localPosition.x > 0.0 ? 1.0 : 0.0);
    vec2 clipOffset = pixelOffset * 2.0 / viewportSize;
    gl_Position = center + vec4(clipOffset, 0.0, 0.0);
    vertexColor = color;
}
