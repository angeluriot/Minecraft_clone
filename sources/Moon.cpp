#include "Moon.h"
#include "Game.h"

// Initie la lune

void Moon::init()
{
	texture = &Texture::moon;

	Luminary::init();

	glm::mat4 player_matrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, Chunk::height + 3.f, 0.f));
	glm::mat4 time_matrix = glm::rotate(glm::mat4(1.f), (-Game::time * speed) + pi, glm::vec3(1.f, 0.f, 0.f));
	model = player_matrix * rotation_matrix * time_matrix * translation_matrix * scale_matrix;

	light.set_vector(glm::normalize(-get_position()));
	inverted = true;
}

// Met à jour la lune

void Moon::update(const glm::vec3& player_pos)
{
	inverted = int((Game::time * speed + pi / 2.f) / pi) % 2 == 0;

	glm::mat4 player_matrix = glm::translate(glm::mat4(1.f), player_pos);
	glm::mat4 time_matrix = glm::rotate(glm::mat4(1.f), Game::time * speed + pi, glm::vec3(1.f, 0.f, 0.f));
	glm::mat4 inverse_matrix = inverted ? glm::rotate(glm::mat4(1.f), pi, glm::vec3(0.f, 0.f, 1.f)) : glm::mat4(1.f);

	model = player_matrix * rotation_matrix * time_matrix * translation_matrix * scale_matrix * inverse_matrix;

	float red = 1.f;
	float green = pow(normalize((get_position().y + distance * 0.3f) / (distance * 1.3f)), 0.2f);
	float blue = pow(normalize((get_position().y + distance * 0.3f) / (distance * 1.3f)), 2.f);

	glm::vec3 color = glm::vec3(red, green, blue);

	light.set_vector(glm::normalize(glm::vec3(player_pos.x, 0.f, player_pos.z) - get_position()));
	glm::vec3 light_color = color;
	light_color *= pow(normalize((get_position().y + distance * 0.3f) / (distance * 1.3f)), 0.2f);

	light_color.r = std::min(std::max(0.05f, light_color.r), 1.f);
	light_color.g = std::min(std::max(0.05f, light_color.g), 1.f);
	light_color.b = std::min(std::max(0.05f, light_color.b), 1.f);

	light.set_color(Color(light_color, 1.f));
}