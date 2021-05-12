#version 130
precision mediump float;

attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord;

varying vec3 v_position;
varying vec2 v_texcoord;
varying vec4 v_diffuse_color;

uniform mat4 u_mvp;
uniform vec4 u_diffuse_color;
uniform vec3 u_light_direction;

uniform vec4 u_clipping_plane;

void main()
{
	gl_ClipDistance[0] = dot(u_clipping_plane, vec4(a_position, 1.));

	v_position = a_position;
	v_texcoord = a_texcoord;
	v_diffuse_color = u_diffuse_color * max(0., dot(a_normal, -u_light_direction));

	gl_Position = u_mvp * vec4(a_position, 1.);
}
