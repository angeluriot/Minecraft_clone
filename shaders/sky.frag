#version 130
precision mediump float;

varying vec3 v_position;

uniform vec4 u_high_color;
uniform vec4 u_low_color;

void main()
{
	vec4 color = mix(u_low_color, u_high_color, v_position.y + 0.5);
	gl_FragColor = vec4(color.rgb, 1.);
}
