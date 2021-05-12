#version 130
precision mediump float;

attribute vec3 a_position;

varying vec3 v_position;

uniform mat4 u_mvp;

void main()
{
	v_position = a_position;

	gl_Position = u_mvp * vec4(a_position, 1.);
}
