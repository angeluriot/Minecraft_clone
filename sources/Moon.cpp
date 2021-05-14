#include "Moon.h"
#include "Game.h"

// Initie la lune

void Moon::init()
{
	texture = &Texture::moon;

	Luminary::init();

	glm::mat4 player_matrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, Chunk::height + 3.f, 0.f));
	glm::mat4 time_matrix = glm::rotate(glm::mat4(1.f), Game::time * speed + pi + start_time, glm::vec3(1.f, 0.f, 0.f));
	model = player_matrix * rotation_matrix * time_matrix * translation_matrix * scale_matrix;

	light.set_vector(glm::normalize(-get_position()));
	inverted = true;
}

// Met à jour la lune

void Moon::update(const glm::vec3& player_pos)
{
	inverted = int((Game::time * speed + pi / 2.f + start_time) / pi) % 2 == 0;

	glm::mat4 player_matrix = glm::translate(glm::mat4(1.f), player_pos);
	glm::mat4 time_matrix = glm::rotate(glm::mat4(1.f), Game::time * speed + pi + start_time, glm::vec3(1.f, 0.f, 0.f));
	glm::mat4 inverse_matrix = inverted ? glm::rotate(glm::mat4(1.f), pi, glm::vec3(0.f, 0.f, 1.f)) : glm::mat4(1.f);
	model = player_matrix * rotation_matrix * time_matrix * translation_matrix * scale_matrix * inverse_matrix;

	light.set_vector(glm::normalize(player_pos - get_position()));

	const float height = get_position().y - player_pos.y;
	const float max_intensity = distance / 3.f;
	const float min_intensity = -distance / 5.f;

	light.set_color(Color(0.6f, 0.8f, 1.f, 1.f));
	light.set_intensity(std::clamp(ratio(height, min_intensity, max_intensity), 0.f, 1.f) * 0.15);
}