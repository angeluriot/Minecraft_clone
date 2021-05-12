#include "Sky.h"
#include "Luminary.h"
#include "Chunk.h"

glm::mat4 Sky::scale_matrix = glm::scale(glm::mat4(1.f), glm::vec3(distance * 2.f, distance * 2.f, distance * 2.f));

// Crée le ciel

Sky::Sky()
{
	high_color = Color(0.f, 0.f, 0.f, 1.f);
	low_color = Color(0.f, 0.f, 0.f, 1.f);
	model = glm::mat4(1.f);
}

// Crée le ciel à partir d'un autre

Sky::Sky(const Sky& other)
{
	*this = other;
}

// Initie le ciel

void Sky::init()
{
	light = Light(Light::Type::Ambient, glm::vec3(), high_color, 1.f);

	scale_matrix = glm::scale(glm::mat4(1.f), glm::vec3(distance * 2.f, distance * 2.f, distance * 2.f));
	glm::mat4 translation_matrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, Chunk::height + 3.f, 0.f));
	model = translation_matrix * scale_matrix;

	object.send_data(Shader::sky, Mesh::sphere_64, DataType::Positions);
}

// Opérateur égal

Sky& Sky::operator=(const Sky& other)
{
	high_color = other.high_color;
	low_color = other.low_color;
	light = other.light;
	model = other.model;
	object = other.object;

	return *this;
}

// Met à jour le ciel

void Sky::update(const glm::vec3& player_pos, float sun_height)
{
	high_color.r = 0.f;
	high_color.g = pow(normalize((sun_height + Luminary::distance * 0.3f) / (Luminary::distance * 1.3f)) * 0.8f, 1.f);
	high_color.b = std::max(0.01f, pow(normalize((sun_height + Luminary::distance * 0.3f) / (Luminary::distance * 1.3f)), 0.7f));

	low_color.r = normalize(high_color.g / 2.f + 0.05f);
	low_color.g = normalize(high_color.g * 2.f + 0.1f);
	low_color.b = normalize(high_color.b * 2.f + 0.2f);

	light.set_color(high_color);

	glm::mat4 translation_matrix = glm::translate(glm::mat4(1.f), player_pos);
	model = translation_matrix * scale_matrix;
}

// Affiche le ciel

void Sky::draw(const Camera& camera) const
{
	glFrontFace(GL_CW);
	Shader::sky.bind();
	object.bind();

	object.send_uniform("u_mvp", camera.get_matrix() * model);
	object.send_uniform("u_high_color", high_color);
	object.send_uniform("u_low_color", low_color);

	object.draw();

	VertexBuffer::unbind();
	Shader::unbind();
	glFrontFace(GL_CCW);
}
