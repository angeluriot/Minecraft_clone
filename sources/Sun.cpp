#include "Sun.h"
#include "Game.h"

// Initie le soleil

void Sun::init()
{
	texture = &Texture::sun;

	Luminary::init();

	glm::mat4 player_matrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, Chunk::height + 3.f, 0.f));
	glm::mat4 time_matrix = glm::rotate(glm::mat4(1.f), Game::time * speed + start_time, glm::vec3(1.f, 0.f, 0.f));
	model = player_matrix * rotation_matrix * time_matrix * translation_matrix * scale_matrix;

	light.set_vector(glm::normalize(-get_position()));
}

// Met à jour le soleil

void Sun::update(const glm::vec3& player_pos)
{
	glm::mat4 player_matrix = glm::translate(glm::mat4(1.f), player_pos);
	glm::mat4 time_matrix = glm::rotate(glm::mat4(1.f), Game::time * speed + start_time, glm::vec3(1.f, 0.f, 0.f));
	model = player_matrix * rotation_matrix * time_matrix * translation_matrix * scale_matrix;

	light.set_vector(glm::normalize(player_pos - get_position()));
	
	const float height = get_position().y - player_pos.y;
	const float max_intensity = distance / 3.f;
	const float min_intensity = -distance / 5.f;
	Color color;

	color.r = 1.f;
	color.g = std::clamp(ratio(height, -max_intensity, max_intensity), 0.f, 1.f);
	color.b = std::clamp(ratio(height, max_intensity, distance), 0.f, 1.f);
	color.a = 1.f;

	light.set_color(color);
	light.set_intensity(std::clamp(ratio(height, min_intensity, max_intensity), 0.f, 1.f));
}