#include "Sun.h"
#include "Game.h"

// Initie le soleil

void Sun::init()
{
	texture = &Texture::sun;

	Luminary::init();

	glm::mat4 player_matrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, Chunk::height + 3.f, 0.f));
	glm::mat4 time_matrix = glm::rotate(glm::mat4(1.f), -Game::time * speed, glm::vec3(1.f, 0.f, 0.f));
	model = player_matrix * rotation_matrix * time_matrix * translation_matrix * scale_matrix;

	light.set_vector(glm::normalize(-get_position()));
}

// Met à jour le soleil

void Sun::update(const glm::vec3& player_pos)
{
	glm::mat4 player_matrix = glm::translate(glm::mat4(1.f), player_pos);
	glm::mat4 time_matrix = glm::rotate(glm::mat4(1.f), -Game::time * speed, glm::vec3(1.f, 0.f, 0.f));
	model = player_matrix * rotation_matrix * time_matrix * translation_matrix * scale_matrix;

	float red = 1.f;
	float green = pow(normalize((get_position().y + distance * 0.3) / (distance * 1.3)), 0.2f);
	float blue = pow(normalize((get_position().y + distance * 0.3) / (distance * 1.3)), 2.f);

	glm::vec3 color = glm::vec3(red, green, blue);

	light.set_vector(glm::normalize(glm::vec3(player_pos.x, 0.f, player_pos.z) - get_position()));
	glm::vec3 light_color = color;
	light_color *= pow(normalize((get_position().y + distance * 0.3) / (distance * 1.3)), 0.2);

	light_color.r = std::clamp(light_color.r * 1.5f, 0.05f, 1.f);
	light_color.g = std::clamp(light_color.g * 1.5f, 0.05f, 1.f);
	light_color.b = std::clamp(light_color.b * 2.5f, 0.05f, 1.f);

	light.set_color(Color(light_color, 1.f));
}