#include "Player.h"

// Crée un joueur

Player::Player()
{
	previous_pos = glm::vec3();
	position = glm::vec3();
	hitbox.fill(glm::vec3());
	speed = glm::vec3();;
	in_air = false;
	chunk = NULL;
	fly = false;
	first_person = false;
}

// Crée un joueur à partir d'un autre

Player::Player(const Player& other)
{
	*this = other;
}

// Crée un joueur à partir de sa position

Player::Player(const glm::vec3& position)
{
	skin = Skin(glm::vec3(0.f, 0.f, 0.f), glm::vec3());

	speed = glm::vec3(0.f, 0.f, 0.f);
	in_air = true;

	hitbox =
	{
		glm::vec3(-0.5f + epsilon, -1.f + epsilon, -0.5f + epsilon), glm::vec3(-0.5f + epsilon, -1.f + epsilon, 0.5f - epsilon),
		glm::vec3( 0.5f - epsilon, -1.f + epsilon, -0.5f + epsilon), glm::vec3( 0.5f - epsilon, -1.f + epsilon, 0.5f - epsilon),
		glm::vec3(-0.5f + epsilon,  0.f          , -0.5f + epsilon), glm::vec3(-0.5f + epsilon,  0.f          , 0.5f - epsilon),
		glm::vec3( 0.5f - epsilon,  0.f          , -0.5f + epsilon), glm::vec3( 0.5f - epsilon,  0.f          , 0.5f - epsilon),
		glm::vec3(-0.5f + epsilon,  1.f - epsilon, -0.5f + epsilon), glm::vec3(-0.5f + epsilon,  1.f - epsilon, 0.5f - epsilon),
		glm::vec3( 0.5f - epsilon,  1.f - epsilon, -0.5f + epsilon), glm::vec3( 0.5f - epsilon,	 1.f - epsilon, 0.5f - epsilon) 
	};

	chunk = NULL;
	fly = false;

	previous_pos = position;
	this->position = position;

	first_person = true;

	skin.move(this->position);
	camera.set_position((skin.body.propagated_matrix * skin.neck.propagated_matrix * skin.head.propagated_matrix * skin.camera.propagated_matrix) * glm::vec3(0.f, 0.f, 0.f));
}

// Opérateur égal

Player& Player::operator=(const Player& other)
{
	skin = other.skin;
	camera = other.camera;
	previous_pos = other.previous_pos;
	position = other.position;
	hitbox = other.hitbox;
	speed = other.speed;
	in_air = other.in_air;
	chunk = other.chunk;
	fly = other.fly;
	first_person = other.first_person;

	return *this;
}

// Donne la position

glm::vec3 Player::get_position() const
{
	return position;
}

// Changement de la direction de la caméra à partir des mouvements de la souris

void Player::look(const glm::ivec2& mouse_pos)
{
	camera.look(mouse_pos);
}

// Changement de la direction de la tête à partir des mouvements de la souris

void Player::update_head()
{
	skin.rotate_head(camera.get_yaw(), camera.get_pitch());
	camera.set_position((skin.body.propagated_matrix * skin.neck.propagated_matrix * skin.head.propagated_matrix * skin.camera.propagated_matrix) * glm::vec3(0.f, 0.f, 0.f));
}

// Déplacements

void Player::move(const Uint8* keystates, World& world)
{
	previous_pos = position;
	float move_speed = (keystates[SDL_SCANCODE_LSHIFT] ? run_speed : walk_speed);
	skin.walking = false;
	skin.running = false;
	skin.forward = false;
	skin.left = false;
	skin.right = false;
	skin.back = false;

	if (keystates[SDL_SCANCODE_A])
	{
		skin.walking = true;
		skin.left = true;
	}

	if (keystates[SDL_SCANCODE_D])
	{
		skin.walking = true;
		skin.right = true;
	}

	if (keystates[SDL_SCANCODE_W])
	{
		skin.walking = true;
		skin.forward = true;
	}

	if (keystates[SDL_SCANCODE_S])
	{
		skin.walking = true;
		skin.back = true;
	}

	if (fly)
	{
		if (keystates[SDL_SCANCODE_A])
			position -= glm::normalize(glm::cross(camera.get_direction(), glm::vec3(0.f, 1.f, 0.f))) * fly_speed * frame_duration;

		if (keystates[SDL_SCANCODE_D])
			position += glm::normalize(glm::cross(camera.get_direction(), glm::vec3(0.f, 1.f, 0.f))) * fly_speed * frame_duration;

		if (keystates[SDL_SCANCODE_W])
			position += glm::normalize(glm::vec3(camera.get_direction().x, 0.f, camera.get_direction().z)) * fly_speed * frame_duration;

		if (keystates[SDL_SCANCODE_S])
			position -= glm::normalize(glm::vec3(camera.get_direction().x, 0.f, camera.get_direction().z)) * fly_speed * frame_duration;

		if (keystates[SDL_SCANCODE_SPACE])
			position += glm::vec3(0.f, 1.f, 0.f) * fly_speed * frame_duration;

		if (keystates[SDL_SCANCODE_LCTRL])
			position -= glm::vec3(0.f, 1.f, 0.f) * fly_speed * frame_duration;

		find_player_chunk(world);
		in_air = true;
		speed = glm::vec3(0.f, 0.f, 0.f);
	}

	else
	{
		skin.running = keystates[SDL_SCANCODE_LSHIFT];

		if (position.y <= water_level + 1.f && in_air)
		{
			if (keystates[SDL_SCANCODE_A])
				speed -= glm::normalize(glm::cross(camera.get_direction(), glm::vec3(0.f, 1.f, 0.f))) * walk_speed * frame_duration;

			if (keystates[SDL_SCANCODE_D])
				speed += glm::normalize(glm::cross(camera.get_direction(), glm::vec3(0.f, 1.f, 0.f))) * walk_speed * frame_duration;

			if (keystates[SDL_SCANCODE_W])
				speed += glm::normalize(glm::vec3(camera.get_direction().x, 0.f, camera.get_direction().z)) * walk_speed * frame_duration;

			if (keystates[SDL_SCANCODE_S])
				speed -= glm::normalize(glm::vec3(camera.get_direction().x, 0.f, camera.get_direction().z)) * walk_speed * frame_duration;

			if (keystates[SDL_SCANCODE_SPACE])
				speed += glm::vec3(0.f, 1.f, 0.f) * run_speed * frame_duration;

			if (keystates[SDL_SCANCODE_LCTRL])
				speed -= glm::vec3(0.f, 1.f, 0.f) * run_speed * frame_duration;

			in_air = true;
		}

		else if (in_air)
		{
			if (keystates[SDL_SCANCODE_A])
				speed -= glm::normalize(glm::cross(camera.get_direction(), glm::vec3(0.f, 1.f, 0.f))) * move_speed * 2.f * frame_duration;

			if (keystates[SDL_SCANCODE_D])
				speed += glm::normalize(glm::cross(camera.get_direction(), glm::vec3(0.f, 1.f, 0.f))) * move_speed * 2.f * frame_duration;

			if (keystates[SDL_SCANCODE_W])
				speed += glm::normalize(glm::vec3(camera.get_direction().x, 0.f, camera.get_direction().z)) * move_speed * 2.f * frame_duration;

			if (keystates[SDL_SCANCODE_S])
				speed -= glm::normalize(glm::vec3(camera.get_direction().x, 0.f, camera.get_direction().z)) * move_speed * 2.f * frame_duration;

			if (get_horizontal_norm(speed) > move_speed)
				set_horizontal_norm(speed, move_speed);
		}

		else
		{
			if (keystates[SDL_SCANCODE_A])
				position -= glm::normalize(glm::cross(camera.get_direction(), glm::vec3(0.f, 1.f, 0.f))) * move_speed * frame_duration;

			if (keystates[SDL_SCANCODE_D])
				position += glm::normalize(glm::cross(camera.get_direction(), glm::vec3(0.f, 1.f, 0.f))) * move_speed * frame_duration;

			if (keystates[SDL_SCANCODE_W])
				position += glm::normalize(glm::vec3(camera.get_direction().x, 0.f, camera.get_direction().z)) * move_speed * frame_duration;

			if (keystates[SDL_SCANCODE_S])
				position -= glm::normalize(glm::vec3(camera.get_direction().x, 0.f, camera.get_direction().z)) * move_speed * frame_duration;

			if (keystates[SDL_SCANCODE_SPACE])
				jump();
		}

		update_position();

		if (previous_pos != position)
			check_hitbox(world);
	}

	if (first_person)
		skin.camera.propagated_matrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -4.f / 16.f - epsilon));

	else
		skin.camera.propagated_matrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 5.f));

	skin.move(position);
	camera.set_position((skin.body.propagated_matrix * skin.neck.propagated_matrix * skin.head.propagated_matrix * skin.camera.propagated_matrix) * glm::vec3(0.f, 0.f, 0.f));
}

// Saut

void Player::jump()
{
	if (position.y > water_level - 0.5f)
		speed.y = jump_speed;

	in_air = true;
}

// Met à jour la position

void Player::update_position()
{
	if (!in_air)
	{
		speed = glm::vec3(0.f, 0.f, 0.f);
		return;
	}

	if (position.y > water_level + 1.f)
		speed.y -= gravity * frame_duration;

	else if (position.y > water_level)
		speed.y -= 0.5f * gravity * frame_duration;

	else
	{
		speed.y -= 0.02f * gravity * frame_duration;
		speed *= 0.99f;
	}

	position += speed * frame_duration;
}

// Met à jour le chunk dans lequel se trouve le joueur

void Player::find_player_chunk(World& world)
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

void Player::check_hitbox(World& world)
{
	find_player_chunk(world);

	if (chunk == NULL)
		return;

	in_air = true;
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
			}

			else
			{
				pos_temp.z = (*chunk)[round(position + hitbox[i])].get_position().z + z_sign[i % 4] * (0.5f + epsilon_2) - hitbox[i].z;
				speed.z = 0.f;
			}
		}

	position = pos_temp;
}

// Casse un bloc si il peut

void Player::break_block()
{
	for (float i = 0.f; i < break_distance; i += 0.1f)
	{
		glm::vec3 pos = glm::vec3(0.f, 0.f, -i);

		pos = skin.body.propagated_matrix * skin.neck.propagated_matrix * skin.head.propagated_matrix * pos;

		if ((*chunk->world)[round(pos)].get_type() != Block::Type::Air)
		{
			(*chunk->world)[round(pos)].set_type(Block::Type::Air, true);
			return;
		}
	}
}

// Affiche le joueur

void Player::draw(const Camera& camera, const std::vector<const Light*>& lights, const Plane& clipping_plane) const
{
	skin.draw(camera, lights, clipping_plane);
}