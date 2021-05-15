#include "Mesh.h"
#include "Entity.h"
#include "Block.h"
#include "Game.h"
#include "Texture.h"

// Crée une partie du corps

Entity::Part::Part()
{
	propagated_matrix = glm::mat4(1.f);
	local_matrix = glm::mat4(1.f);
	children.clear();
}

// Crée une partie du corps à partir d'une autre

Entity::Part::Part(const Part& other)
{
	*this = other;
}

// Crée une partie du corps à partir de ces paramètres

Entity::Part::Part(Mesh mesh, const glm::vec2& texture_pos, const glm::vec3& size, const std::vector<Part*>& children, bool horizontal)
{
	mesh.texcoords = init_texcoords(texture_pos / texture_size, size / texture_size, horizontal);
	object.send_data(Shader::entity, mesh);
	propagated_matrix = glm::mat4(1.f);
	local_matrix = glm::mat4(1.f);
	this->children = children;
}

// Opérateur égal

Entity::Part& Entity::Part::operator=(const Part& other)
{
	object = other.object;
	propagated_matrix = other.propagated_matrix;
	local_matrix = other.local_matrix;
	children = other.children;

	return *this;
}

// Affiche la partie du corps

void Entity::Part::draw(const Camera& camera, const std::vector<const Light*>& lights, const Plane& clipping_plane) const
{
	glm::mat4 model = Game::matrices.top() * propagated_matrix * local_matrix;
	Game::matrices.push(Game::matrices.top() * propagated_matrix);

	object.bind();

	ColorRGB water_color = ColorRGB(0.f, 0.f, 0.f);

	for (uint16_t i = 0; i < std::min((int)lights.size(), (int)nb_max_lights); i++)
		water_color += ColorRGB(Material::water.get_color()) * ColorRGB(lights[i]->get_color()) * lights[i]->get_intensity();

	object.send_uniform("u_model", model);
	object.send_uniform("u_inverted_model", glm::transpose(glm::inverse(glm::mat3(model))));
	object.send_uniform("u_mvp", camera.get_matrix() * model);
	object.send_uniform("u_camera", camera.get_position());
	object.send_uniform("u_ambient", Material::entity.get_ambient());
	object.send_uniform("u_diffuse", Material::entity.get_diffuse());
	object.send_uniform("u_specular", Material::entity.get_specular());
	object.send_uniform("u_shininess", Material::entity.get_shininess());
	object.send_uniform("u_fake_cam", (int)Game::fake_cam);
	object.send_uniform("u_water_level", water_level);
	object.send_uniform("u_water_color", water_color);
	object.send_uniform("u_clipping_plane", clipping_plane);
	object.send_texture("u_texture", 0);

	std::vector<int> light_types;
	std::vector<glm::vec3> light_vectors;
	std::vector<ColorRGB> light_colors;
	std::vector<float> light_intensities;

	for (uint16_t i = 0; i < std::min((int)lights.size(), (int)nb_max_lights); i++)
	{
		light_types.push_back((int)lights[i]->get_type());
		light_vectors.push_back(lights[i]->get_vector());
		light_colors.push_back(ColorRGB(lights[i]->get_color()));
		light_intensities.push_back(lights[i]->get_intensity());
	}

	object.send_uniform("u_light_types", light_types);
	object.send_uniform("u_light_vectors", light_vectors);
	object.send_uniform("u_light_colors", light_colors);
	object.send_uniform("u_light_intensities", light_intensities);
	object.send_uniform("u_nb_lights", std::min((int)lights.size(), (int)nb_max_lights));

	object.draw();

	VertexBuffer::unbind();

	for (auto child : children)
		child->draw(camera, lights, clipping_plane);

	Game::matrices.pop();
}

// Donne les coordonnées de textures à partir de la position et de la taille sur l'image

std::vector<float> Entity::Part::init_texcoords(const glm::vec2& texture_pos, const glm::vec3& size, bool horizontal)
{
	std::vector<float> texcoords = Mesh::cube.texcoords;
	std::array<glm::vec2, 6> pos_1;
	std::array<glm::vec2, 6> pos_2;

	if (!horizontal)
	{
		pos_1[3] = texture_pos + glm::vec2(0, size.z);
		pos_1[0] = pos_1[3] + glm::vec2(size.z, 0);
		pos_1[2] = pos_1[0] + glm::vec2(size.x, 0);
		pos_1[1] = pos_1[2] + glm::vec2(size.z, 0);

		pos_1[4] = texture_pos + glm::vec2(size.z, 0);
		pos_1[5] = pos_1[4] + glm::vec2(size.x, 0);

		pos_2[3] = pos_1[3] + glm::vec2(size.z, size.y);
		pos_2[0] = pos_2[3] + glm::vec2(size.x, 0);
		pos_2[2] = pos_2[0] + glm::vec2(size.z, 0);
		pos_2[1] = pos_2[2] + glm::vec2(size.x, 0);

		pos_2[4] = pos_1[4] + glm::vec2(size.x, size.z);
		pos_2[5] = pos_2[4] + glm::vec2(size.x, 0);
	}

	else
	{
		pos_1[3] = texture_pos + glm::vec2(0, size.y);
		pos_1[5] = pos_1[3] + glm::vec2(size.y, 0);
		pos_1[2] = pos_1[5] + glm::vec2(size.x, 0);
		pos_1[4] = pos_1[2] + glm::vec2(size.y, 0);

		pos_1[0] = texture_pos + glm::vec2(size.y, 0);
		pos_1[1] = pos_1[0] + glm::vec2(size.x, 0);

		pos_2[3] = pos_1[3] + glm::vec2(size.y, size.z);
		pos_2[5] = pos_2[3] + glm::vec2(size.x, 0);
		pos_2[2] = pos_2[5] + glm::vec2(size.y, 0);
		pos_2[4] = pos_2[2] + glm::vec2(size.x, 0);

		pos_2[0] = pos_1[0] + glm::vec2(size.x, size.y);
		pos_2[1] = pos_2[0] + glm::vec2(size.x, 0);
	}

	for (uint8_t i = 0; i < Cube::nb_faces; i++)
		for (uint8_t j = 0; j < 2 * Cube::Face::nb_points; j += 2)
		{
			texcoords[i * 2 * Cube::Face::nb_points + j] = texcoords[i * 2 * Cube::Face::nb_points + j] * (pos_2[i].x - pos_1[i].x) + pos_1[i].x;
			texcoords[i * 2 * Cube::Face::nb_points + j + 1] = texcoords[i * 2 * Cube::Face::nb_points + j + 1] * (pos_2[i].y - pos_1[i].y) + pos_1[i].y;
		}

	return texcoords;
}

// Crée une entité

Entity::Entity()
{
	y_offset = 0.f;
	is_blocked = false;
	texture = NULL;
	walking = false;
	body_angle = 0.f;
	head_angle = 0.f;
	previous_pos = glm::vec3();
	position = glm::vec3();
	body_initial = glm::mat4(0.f);
	neck_initial = glm::mat4(0.f);
	speed = glm::vec3();
	in_air = true;
	chunk = NULL;

	hitbox =
	{
		glm::vec3(-0.5f + epsilon, -1.f + epsilon, -0.5f + epsilon), glm::vec3(-0.5f + epsilon, -1.f + epsilon, 0.5f - epsilon),
		glm::vec3(0.5f - epsilon, -1.f + epsilon, -0.5f + epsilon), glm::vec3(0.5f - epsilon, -1.f + epsilon, 0.5f - epsilon),
		glm::vec3(-0.5f + epsilon,  0.f          , -0.5f + epsilon), glm::vec3(-0.5f + epsilon,  0.f          , 0.5f - epsilon),
		glm::vec3(0.5f - epsilon,  0.f          , -0.5f + epsilon), glm::vec3(0.5f - epsilon,  0.f          , 0.5f - epsilon),
		glm::vec3(-0.5f + epsilon,  1.f - epsilon, -0.5f + epsilon), glm::vec3(-0.5f + epsilon,  1.f - epsilon, 0.5f - epsilon),
		glm::vec3(0.5f - epsilon,  1.f - epsilon, -0.5f + epsilon), glm::vec3(0.5f - epsilon,  1.f - epsilon, 0.5f - epsilon)
	};

	body.children = { &neck };
}

// Crée une entité à partir d'une autre

Entity::Entity(const Entity& other)
{
	*this = other;
}

// Destructeur virtuel

Entity::~Entity()
{}

// Opérateur égal

Entity& Entity::operator=(const Entity& other)
{
	y_offset = other.y_offset;
	body = other.body;
	neck = other.neck;
	hitbox = other.hitbox;
	previous_pos = other.previous_pos;
	position = other.position;
	speed = other.speed;
	in_air = other.in_air;
	chunk = other.chunk;
	texture = other.texture;
	is_blocked = other.is_blocked;
	walking = other.walking;
	body_initial = other.body_initial;
	neck_initial = other.neck_initial;
	body_angle = other.body_angle;
	head_angle = other.head_angle;

	body.children = { &neck };

	return *this;
}

// Déplacements

void Entity::move(const glm::vec3& position)
{
	this->previous_pos = this->position;
	this->position = position;
	body.propagated_matrix = glm::translate(glm::mat4(1.f), position + glm::vec3(0.f, y_offset, 0.f));
}

// Tourne l'entité

void Entity::rotate(float angle)
{
	body_angle = angle;
	body.propagated_matrix = glm::rotate(body.propagated_matrix, body_angle, glm::vec3(0.f, 1.f, 0.f));
}

// Donne la position

glm::vec3 Entity::get_position() const
{
	return position;
}

// Donne le chunk

Chunk* Entity::get_chunk() const
{
	return chunk;
}

// Saut

void Entity::jump()
{
	speed.y = jump_speed;
	in_air = true;
}

// Met à jour la position

void Entity::update_position()
{
	move(position);

	if (!in_air)
	{
		speed = glm::vec3(0.f, 0.f, 0.f);
		return;
	}

	speed.y -= gravity * frame_duration;
	position += speed * frame_duration;
}

// Met à jour la rotation

void Entity::update_rotation()
{
	rotate(body_angle);
}

// Met à jour le chunk dans lequel se trouve l'entité

void Entity::find_entity_chunk(const World& world)
{
	if (chunk == NULL)
	{
		chunk = world.find_chunk(world.block_to_chunk(round(position)));
		return;
	}

	if (chunk->position == world.block_to_chunk(round(position)))
		return;

	for (auto c : chunk->chunks_around)
		if (c != NULL && c->position == world.block_to_chunk(round(position)))
		{
			chunk = c;
			return;
		}

	for (auto chunks_around : chunk->chunks_around)
		if (chunks_around != NULL)
			for (auto c : chunks_around->chunks_around)
				if (c != NULL && c->position == world.block_to_chunk(round(position)))
				{
					chunk = c;
					return;
				}
}

// Prend en compte les collisions

void Entity::check_hitbox(const World& world)
{
	find_entity_chunk(world);

	if (chunk == NULL)
		return;

	in_air = true;
	is_blocked = false;

	speed = (position - previous_pos) / frame_duration;
	glm::vec3 pos_temp = position;
	std::array<float, 4> x_sign = { 1.f, 1.f, -1.f, -1.f };
	std::array<float, 4> z_sign = { 1.f, -1.f, 1.f, -1.f };

	for (uint8_t i = 0; i < hitbox.size(); i++)
		if ((*chunk)[round(position + hitbox[i])].get_type() != Block::Type::Air && !(*chunk)[round(position + hitbox[i])].is_plant())
		{
			float dist_x = abs((position + hitbox[i]).x - ((*chunk)[round(position + hitbox[i])].get_position().x + x_sign[i % 4] * 0.5f));
			float dist_y = FLT_MAX;
			float dist_z = abs((position + hitbox[i]).z - ((*chunk)[round(position + hitbox[i])].get_position().z + z_sign[i % 4] * 0.5f));

			if (i < 4)
				dist_y = abs((position + hitbox[i]).y - ((*chunk)[round(position + hitbox[i])].get_position().y + 0.5f));

			if (dist_y < dist_x && dist_y < dist_z)
			{
				pos_temp.y = (*chunk)[round(position + hitbox[i])].get_position().y + 0.5f + epsilon_2 - hitbox[i].y;
				speed = glm::vec3(0.f, 0.f, 0.f);
				in_air = false;
			}

			else if (dist_x < dist_z)
			{
				pos_temp.x = (*chunk)[round(position + hitbox[i])].get_position().x + x_sign[i % 4] * (0.5f + epsilon_2) - hitbox[i].x;
				speed.x = 0.f;
				is_blocked = true;
			}

			else
			{
				pos_temp.z = (*chunk)[round(position + hitbox[i])].get_position().z + z_sign[i % 4] * (0.5f + epsilon_2) - hitbox[i].z;
				speed.z = 0.f;
				is_blocked = true;
			}
		}

	position = pos_temp;
}

// Met à jour l'entité

void Entity::update(const World& world, const glm::vec3& player_pos)
{
	check_hitbox(world);

	if (chunk == NULL)
		return;

	update_position();
	update_rotation();
}

// Affiche l'entité

void Entity::draw(const Camera& camera, const std::vector<const Light*>& lights, const Plane& clipping_plane) const
{
	Shader::entity.bind();
	texture->bind(0);

	body.draw(camera, lights, clipping_plane);

	Texture::unbind();
	Shader::unbind();
}