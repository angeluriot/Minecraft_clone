#version 130
precision mediump float;

varying vec2 v_texcoord;

uniform vec3 u_light_color;
uniform float u_brightness;
uniform sampler2D u_texture;

void main()
{
	gl_FragColor = vec4(texture2D(u_texture, v_texcoord).rgb * u_light_color, texture2D(u_texture, v_texcoord).a * u_brightness);
}
