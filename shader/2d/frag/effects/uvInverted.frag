#version 460

layout(location = 0) out vec4 fragColor;

in vec2 v_uv;

void main(void)
{
    fragColor = vec4(1.0 - v_uv, 0.0, 1.0);
}
