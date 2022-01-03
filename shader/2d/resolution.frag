#version 460

layout(location = 0) out vec4 fragColor;

in vec2 v_uv;

// RESOLUTION
uniform vec2 u_resolution;

void main(void)
{
    vec2 pos = v_uv * u_resolution;
    float color = mod(pos.x + pos.y, 2.0);
    fragColor = vec4(vec3(color), 1.0);
}
