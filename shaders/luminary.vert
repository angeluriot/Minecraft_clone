#version 130
precision mediump float;

attribute vec3 a_position;
attribute vec2 a_texcoord;

varying vec3 v_position;
varying vec2 v_texcoord;

uniform mat4 u_model;
uniform mat4 u_mvp;

void main()
{
	v_position = vec3(u_model * vec4(a_position, 1.));
	v_texcoord = a_texcoord;

	gl_Position = u_mvp * vec4(a_position, 1.);
}
