#version 130
precision mediump float;

varying vec3 v_normal;
varying vec3 v_position;
varying vec2 v_texcoord;
varying vec3 v_ambient_color;
varying vec3 v_diffuse_color;

uniform vec3 u_camera;
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
			// Specular
			vec3 reflection = reflect(light_direction, v_normal);
			specular_color += u_specular * pow(max(0., dot(reflection, camera_direction)), u_shininess) * u_light_colors[i] * intensity;
		}
	}

	vec3 color = (v_ambient_color + v_diffuse_color) * texture2D(u_texture, v_texcoord).rgb + specular_color;
	gl_FragColor = vec4(color, texture2D(u_texture, v_texcoord).a);
}
