#version 130
precision mediump float;

attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord;

varying vec3 v_normal;
varying vec3 v_position;
varying vec2 v_texcoord;
varying vec3 v_ambient_color;
varying vec3 v_diffuse_color;

uniform mat4 u_model;
uniform mat3 u_inverted_model;
uniform mat4 u_mvp;
uniform float u_ambient;
uniform float u_diffuse;
uniform int[10] u_light_types;
uniform vec3[10] u_light_vectors;
uniform vec3[10] u_light_colors;
uniform float[10] u_light_intensities;
uniform int u_nb_lights;
uniform vec4 u_clipping_plane;

void main()
{
	// Clipping plane
	gl_ClipDistance[0] = dot(u_clipping_plane, u_model * vec4(a_position, 1.));

	v_normal = normalize(u_inverted_model * a_normal);
	v_position = vec3(u_model * vec4(a_position, 1.));
	v_texcoord = a_texcoord;

	// Gestion de la lumière

	v_ambient_color = vec3(0., 0., 0.);
	v_diffuse_color = vec3(0., 0., 0.);

	for (int i = 0; i < u_nb_lights; i++)
	{
		vec3 light_direction = u_light_vectors[i];
		float intensity = u_light_intensities[i];

		if (u_light_types[i] == 2)
		{
			light_direction = normalize(v_position - u_light_vectors[i]);
			intensity = u_light_intensities[i] / max(pow(distance(v_position, u_light_vectors[i]), 2), 0.01);
		}

		// Ambiante
		v_ambient_color += u_ambient * u_light_colors[i] * intensity;

		if (u_light_types[i] != 0)
		{
			// Diffuse
			v_diffuse_color += u_diffuse * max(0., dot(v_normal, -light_direction)) * u_light_colors[i] * intensity;
		}
	}

	gl_Position = u_mvp * vec4(a_position, 1.);
}
