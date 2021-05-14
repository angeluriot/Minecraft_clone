#version 130
precision mediump float;

varying vec2 v_texcoord;

uniform int u_draw_cursor;
uniform sampler2D u_texture;
uniform sampler2D u_lens_flare;
uniform sampler2D u_cursor;

void main()
{
	// Rajoute le curseur en couleurs inversées

	vec3 color = texture2D(u_texture, v_texcoord).rgb + texture2D(u_lens_flare, v_texcoord).rgb;
	vec3 inverted_color = 1. - texture2D(u_texture, v_texcoord).rgb;
	float mask = texture2D(u_cursor, v_texcoord).r;

	gl_FragColor = vec4(mix(color, inverted_color, mask * u_draw_cursor), texture2D(u_texture, v_texcoord).a);
}
