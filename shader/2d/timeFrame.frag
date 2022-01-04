#version 460

layout(location = 0) out vec4 fragColor;

in vec2 v_uv;

// TIME.FRAME
uniform uint u_frame;

void main(void)
{
    float color = mod(u_frame / 10, 2);
    fragColor = vec4(vec3(color), 1.0);
}
