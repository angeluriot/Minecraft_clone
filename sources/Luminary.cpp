#include "Luminary.h"
#include "Game.h"
#include "Mesh.h"
#include "Shader.h"

const glm::mat4	Luminary::scale_matrix = glm::scale(glm::mat4(1.f), glm::vec3(size, size, size));
const glm::mat4	Luminary::translation_matrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, distance));
const glm::mat4	Luminary::rotation_matrix = glm::rotate(glm::mat4(1.f), angle, glm::vec3(0.f, 1.f, 0.f));

// Crée un astre

Luminary::Luminary()
{
	texture = NULL;
	model = glm::mat4(1.f);
}

// Crée un astre à partir d'un autre

Luminary::Luminary(const Luminary& other)
{
	*this = other;
}

// Opérateur égal

Luminary& Luminary::operator=(const Luminary& other)
{
	texture = other.texture;
	light = other.light;
	model = other.model;
	object = other.object;
	
	return *this;
}

// Initie un astre

void Luminary::init()
{
	this->light.set_type(Light::Type::Directional);
	this->light.set_intensity(1.f);

	object.send_data(Shader::luminary, Mesh::square, DataType::Positions | DataType::TexCoords);
}

// Affiche l'astre

void Luminary::draw(const Camera& camera) const
{
	Shader::luminary.bind();
	texture->bind(0);
	object.bind();

	object.send_uniform("u_mvp", camera.get_matrix() * model);
	object.send_texture("u_texture", 0);
	object.draw();
	
	VertexBuffer::unbind();
	Texture::unbind();
	Shader::unbind();
}

// Donne la position de l'astre

glm::vec3 Luminary::get_position() const
{
	return model * glm::vec3(0.f, 0.f, 0.f);
}

// Donne un pointeur vers la lumière de l'astre

const Light* Luminary::get_light() const
{
	return &light;
}
