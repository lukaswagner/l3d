#version 460

layout(location = 0) out vec4 fragColor;

in vec2 v_uv;

uniform sampler2D u_texture0;
uniform sampler2D u_texture1;
uniform float u_mixFactor;

void main(void)
{
    vec4 c0 = texture(u_texture0, v_uv);
    vec4 c1 = texture(u_texture1, v_uv);
    fragColor = mix(c0, c1, u_mixFactor);
}
