#version 130
precision mediump float;

varying vec3 v_normal;
varying vec3 v_position;
varying vec2 v_texcoord;
varying vec3 v_ambient_color;
varying vec3 v_diffuse_color;

uniform vec3 u_camera;
uniform int u_fake_cam;
uniform float u_water_level;
uniform vec3 u_water_color;
uniform vec3 u_ambient_color;
uniform float u_specular;
uniform float u_shininess;
uniform int[10] u_light_types;
uniform vec3[10] u_light_vectors;
uniform vec3[10] u_light_colors;
uniform float[10] u_light_intensities;
uniform int u_nb_lights;
uniform sampler2D u_texture;

void main()
{
	// Lumière spéculaire

	vec3 specular_color = vec3(0., 0., 0.);
	vec3 camera_direction = normalize(u_camera - v_position);;

	for (int i = 0; i < u_nb_lights; i++)
	{
		vec3 light_direction = u_light_vectors[i];
		float intensity = u_light_intensities[i];

		if (u_light_types[i] == 2)
		{
			light_direction = normalize(v_position - u_light_vectors[i]);
			intensity = u_light_intensities[i] / max(pow(distance(v_position, u_light_vectors[i]), 2), 0.01);
		}

		if (u_light_types[i] != 0)
		{
			vec3 reflection = reflect(light_direction, v_normal);
			specular_color += u_specular * pow(max(0., dot(reflection, camera_direction)), u_shininess) * u_light_colors[i] * intensity;
		}
	}

	// Assemblage de la lumière
	vec3 color = (v_ambient_color + v_diffuse_color) * texture2D(u_texture, v_texcoord).rgb + specular_color;

	// Calcul de la quantité d'eau tranversée par la lumière

	float ray_water_length;

	if (u_camera.y >= u_water_level)
	{
		if (v_position.y >= u_water_level)
			ray_water_length = -30;

		else
			ray_water_length = distance(u_camera, v_position) * ((u_water_level - v_position.y) / (u_camera.y - v_position.y));
	}

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
