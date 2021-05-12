#version 130
precision mediump float;

attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord;

uniform mat4 u_model;
uniform mat3 u_inverted_model;
uniform mat4 u_mvp;
uniform vec4 u_diffuse_color;
uniform vec3 u_light_direction;
uniform vec4 u_clipping_plane;

varying vec3 v_normal;
varying vec3 v_position;
varying vec2 v_texcoord;
varying vec4 v_diffuse_color;

void main()
{
	gl_ClipDistance[0] = dot(u_clipping_plane, u_model * vec4(a_position, 1.));

	v_normal = normalize(u_inverted_model * a_normal);
	v_position = vec3(u_model * vec4(a_position, 1.));
	v_texcoord = a_texcoord;
	v_diffuse_color = u_diffuse_color * max(0., dot(v_normal, -u_light_direction));

	gl_Position = u_mvp * vec4(a_position, 1.);
}
