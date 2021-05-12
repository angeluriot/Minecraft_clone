#version 130
precision mediump float;

varying vec3 v_position;
varying vec2 v_texcoord;
varying vec4 v_diffuse_color;

uniform vec4 u_ambient_color;
uniform vec3 u_camera;
uniform float u_water_level;
uniform vec4 u_water_color;
uniform sampler2D u_texture;

void main()
{
	if (texture2D(u_texture, v_texcoord).a < 0.1)
		discard;

	vec4 color = (u_ambient_color + v_diffuse_color) * texture2D(u_texture, v_texcoord);

	float ray_water_length;

	ray_water_length = distance(u_camera, v_position) * ((u_water_level - v_position.y) / (u_camera.y - v_position.y));
	ray_water_length *= (u_water_level - v_position.y) / 5.;

	if (v_position.y >= u_water_level)
			ray_water_length = -30;

	color = mix(color, u_water_color, clamp((ray_water_length + 30.) / 70., 0., 1.));

	gl_FragColor = vec4(color.rgb, 1.);
}
