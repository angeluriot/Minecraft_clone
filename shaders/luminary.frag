#version 130
precision mediump float;

varying vec3 v_position;
varying vec2 v_texcoord;

uniform vec3 u_camera;
uniform int u_fake_cam;
uniform float u_water_level;
uniform vec3 u_water_color;
uniform sampler2D u_texture;

void main()
{
	vec3 color = texture2D(u_texture, v_texcoord).rgb;

	// Calcul de la quantité d'eau tranversée par la lumière

	float ray_water_length;

	if (u_camera.y >= u_water_level)
			ray_water_length = -30;

	else
	{
		if (v_position.y >= u_water_level)
			ray_water_length = distance(u_camera, v_position) * ((u_water_level - u_camera.y) / (v_position.y - u_camera.y));

		else
			ray_water_length = distance(u_camera, v_position);
	}

	// Calcul de la profondeur de l'objet dans l'eau

	float depth;

	if (u_camera.y >= u_water_level)
		depth = max(u_water_level - v_position.y, 0.);

	else
		depth = max(u_water_level - u_camera.y, 0.);

	vec3 water_color = u_water_color;

	water_color.r /= max(depth, 4.) / 4.;
	water_color.g /= max(depth, 4.) / 4.;
	water_color.b /= max(depth, 10.) / 10.;

	if (v_position.y < u_water_level)
		ray_water_length *= max(u_water_level - v_position.y, 7.) / 7.;

	// Assemblage final

	if (u_fake_cam == 0)
		color = mix(color, water_color, clamp((ray_water_length + 30.) / 70., 0., 1.));

	gl_FragColor = vec4(color, texture2D(u_texture, v_texcoord).a);
}
