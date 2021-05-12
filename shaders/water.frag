#version 130
precision mediump float;

varying vec3 v_position;
varying vec4 v_screen_position;

uniform float u_time;
uniform vec3 u_camera;
uniform vec3 u_light_direction;
uniform vec4 u_ambient_color;
uniform vec4 u_diffuse_color;
uniform vec4 u_specular_color;
uniform float u_shininess;
uniform sampler2D u_reflection;
uniform sampler2D u_refraction;
uniform sampler2D u_water_dudv;
uniform sampler2D u_water_normals;

const float waves_height = 0.01;
const float waves_size = 30.;
const float waves_speed = 1.;
const float water_alpha = 0.2;

void main()
{
	// Distortion des vagues

	vec2 distortion_1_texcoord = vec2(v_position.x + waves_speed * u_time, v_position.z + waves_speed * u_time) / waves_size;
	vec2 distortion_2_texcoord = vec2(v_position.x - waves_speed * u_time, v_position.z - waves_speed * u_time) / waves_size;

	vec2 distortion_1 = texture2D(u_water_dudv, distortion_1_texcoord).rg * 2. - 1.;
	vec2 distortion_2 = texture2D(u_water_dudv, distortion_2_texcoord).rg * 2. - 1.;
	vec2 distortion = (distortion_1 + distortion_2) * waves_height;

	// Réflection et réfraction

	vec2 texcoord = (v_screen_position.xy / v_screen_position.w) / 2. + 0.5;
	vec2 reflection_texcoord = vec2(texcoord.x, -texcoord.y) + distortion;
	vec2 refraction_texcoord = texcoord + distortion;

	reflection_texcoord.x = clamp(reflection_texcoord.x, 0.001, 0.999);
	reflection_texcoord.y = clamp(reflection_texcoord.y, -0.999, -0.001);

	refraction_texcoord.x = clamp(refraction_texcoord.x, 0.001, 0.999);
	refraction_texcoord.y = clamp(refraction_texcoord.y, 0.001, 0.999);

	// Coéfficient de Fresnel

	vec3 camera_direction = normalize(u_camera - v_position);
	float reflection_alpha = pow(dot(camera_direction, vec3(0., 1., 0.)), 0.75);

	vec3 color = mix(texture2D(u_reflection, reflection_texcoord), texture2D(u_refraction, refraction_texcoord), reflection_alpha).rgb;

	// Calcul des normales

	vec2 normal_texcoord = vec2(v_position.x + waves_speed * u_time, v_position.z - waves_speed * u_time) / waves_size + distortion;

	vec3 normal = texture2D(u_water_normals, normal_texcoord).yzx * 2. - 1.;
	normal = normalize(mix(normal, vec3(0., 1., 0.), 0.5));

	// Couleur diffuse

	vec4 diffuse_color = u_diffuse_color * max(0., dot(normal, -u_light_direction));

	// Couleur spéculaire

	vec3 light_reflection = reflect(u_light_direction, normal);
	vec4 specular_color = u_specular_color * pow(max(0., dot(light_reflection, camera_direction)), u_shininess);

	// Assemblage final

	gl_FragColor = vec4(mix(color, u_ambient_color.rgb, water_alpha) + diffuse_color.rgb + specular_color.rgb, 1.);
}
