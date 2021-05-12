#version 130
precision mediump float;

varying vec3 v_normal;
varying vec3 v_position;
varying vec2 v_texcoord;
varying vec4 v_diffuse_color;

uniform vec3 u_camera;
uniform vec4 u_ambient_color;
uniform vec4 u_specular_color;
uniform float u_shininess;
uniform vec3 u_light_direction;
uniform sampler2D u_texture;

void main()
{
	// Specular
	vec3 reflection = reflect(u_light_direction, v_normal);
	vec3 camera_direction = normalize(u_camera - v_position);
	vec4 specular_color = u_specular_color * pow(max(0., dot(reflection, camera_direction)), u_shininess);

	gl_FragColor = vec4(vec3(u_ambient_color + v_diffuse_color) * texture2D(u_texture, v_texcoord).rgb + specular_color.rgb, 1.);
}
