#version 130
precision mediump float;

attribute vec3 a_position;

varying vec3 v_position;
varying vec4 v_screen_position;

uniform mat4 u_model;
uniform mat4 u_mvp;

void main()
{
	v_position = vec3(u_model * vec4(a_position, 1.));
	v_screen_position = u_mvp * vec4(a_position, 1.);

	gl_Position = v_screen_position;
}
